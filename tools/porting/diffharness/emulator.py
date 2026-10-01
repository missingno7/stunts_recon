"""Small 16-bit real-mode call engine for the locked historical image."""
from __future__ import annotations

from unicorn import (Uc, UcError, UC_ARCH_X86, UC_HOOK_CODE, UC_HOOK_INSN,
                     UC_HOOK_INTR, UC_MODE_16)
from unicorn.x86_const import (UC_X86_REG_AX, UC_X86_REG_BX, UC_X86_REG_CX,
                               UC_X86_REG_DX, UC_X86_REG_SI, UC_X86_REG_DI,
                               UC_X86_REG_BP, UC_X86_REG_SP, UC_X86_REG_CS,
                               UC_X86_REG_DS, UC_X86_REG_ES, UC_X86_REG_SS,
                               UC_X86_REG_IP, UC_X86_REG_EFLAGS,
                               UC_X86_INS_IN, UC_X86_INS_OUT)

from .model import CallResult, HarnessError, MemoryRegion, RoutineCase, TrapError
from .oracle import (DEFAULT_LOAD_SEGMENT, OracleImage, RoutineAddress,
                     SymbolMap, dgroup_segment)


REGS = {"ax": UC_X86_REG_AX, "bx": UC_X86_REG_BX, "cx": UC_X86_REG_CX,
        "dx": UC_X86_REG_DX, "si": UC_X86_REG_SI, "di": UC_X86_REG_DI,
        "bp": UC_X86_REG_BP, "sp": UC_X86_REG_SP, "cs": UC_X86_REG_CS,
        "ds": UC_X86_REG_DS, "es": UC_X86_REG_ES, "ss": UC_X86_REG_SS,
        "ip": UC_X86_REG_IP}


def linear(segment: int, offset: int) -> int:
    return ((segment << 4) + offset) & 0xFFFFF


class RealModeRunner:
    def __init__(self, oracle: OracleImage | None = None, *,
                 load_segment: int = DEFAULT_LOAD_SEGMENT,
                 instruction_budget: int = 1_000_000,
                 io_ports: dict[int, int] | None = None):
        self.oracle = oracle or OracleImage.load()
        self.load_segment = load_segment & 0xFFFF
        self.instruction_budget = instruction_budget
        self.io_ports = io_ports or {}
        self.symbols = SymbolMap()

    def call(self, case: RoutineCase) -> CallResult:
        if case.call not in {"near", "far"}:
            raise HarnessError(f"{case.name}: call must be 'near' or 'far'")
        address = self.symbols.resolve(case.routine)
        code_segment, entry_ip = address.far_at(self.load_segment)
        if (self.load_segment << 4) + len(self.oracle.load_image) > 0x100000:
            raise HarnessError("the load image exceeds the 20-bit real-mode address space")
        uc = Uc(UC_ARCH_X86, UC_MODE_16)
        uc.mem_map(0, 0x100000)
        relocated = self.oracle.relocated(self.load_segment)
        image_linear = self.load_segment << 4
        uc.mem_write(image_linear, relocated)

        for write in case.memory:
            uc.mem_write(linear(write.segment, write.offset), bytes(write.data))

        # Small-model C passes stack locals as DGROUP near pointers. The
        # original game runs with SS == DS; a separate default stack silently
        # breaks nested calls to matrix/vector helpers. Honor explicit stack
        # registers when a case intentionally exercises a different ABI.
        dgroup = dgroup_segment(self.load_segment)
        registers = {name.lower(): value for name, value in case.registers.items()}
        stack_segment = int(registers.get("ss", dgroup)) & 0xFFFF
        initial_sp = int(registers.get("sp", 0xF000)) & 0xFFFF
        sentinel_cs, sentinel_ip = ((code_segment, 0xFFFE) if case.call == "near"
                                    else (0x7000, 0x0000))
        stack_words = [sentinel_ip]
        if case.call == "far":
            stack_words.append(sentinel_cs)
        stack_words.extend(int(word) & 0xFFFF for word in case.args)
        stack = b"".join(int(word & 0xFFFF).to_bytes(2, "little")
                         for word in stack_words)
        uc.mem_write(linear(stack_segment, initial_sp), stack)

        defaults = {"cs": code_segment, "ip": entry_ip, "ds": dgroup,
                    "es": 0xA000, "ss": stack_segment, "sp": initial_sp,
                    "bp": 0, "ax": 0, "bx": 0, "cx": 0, "dx": 0,
                    "si": 0, "di": 0}
        defaults.update(registers)
        for name, value in defaults.items():
            uc.reg_write(REGS[name], int(value) & 0xFFFF)
        uc.reg_write(UC_X86_REG_EFLAGS, 0x0202)

        state = {"count": 0, "sentinel": False, "trap": None}
        sentinel_linear = linear(sentinel_cs, sentinel_ip)

        def on_code(machine, address_value, size, _user):
            state["count"] += 1
            if address_value == sentinel_linear:
                state["sentinel"] = True
                machine.emu_stop()

        def on_intr(machine, intno, _user):
            state["trap"] = f"INT {intno:#04x} at {self._pc(machine)}"
            machine.emu_stop()

        def on_in(machine, port, size, _user):
            if port not in self.io_ports:
                state["trap"] = f"IN port {port:#06x} width {size} at {self._pc(machine)}"
                machine.emu_stop()
                return 0
            return self.io_ports[port] & ((1 << (size * 8)) - 1)

        def on_out(machine, port, size, value, _user):
            state["trap"] = f"OUT port {port:#06x}={value:#x} width {size} at {self._pc(machine)}"
            machine.emu_stop()

        uc.hook_add(UC_HOOK_CODE, on_code)
        uc.hook_add(UC_HOOK_INTR, on_intr)
        uc.hook_add(UC_HOOK_INSN, on_in, None, 1, 0, UC_X86_INS_IN)
        uc.hook_add(UC_HOOK_INSN, on_out, None, 1, 0, UC_X86_INS_OUT)
        try:
            uc.emu_start(linear(code_segment, entry_ip), 0,
                         count=self.instruction_budget)
        except UcError as error:
            raise HarnessError(f"{case.name}: Unicorn stopped at {self._pc(uc)}: {error}") from error
        if state["trap"]:
            raise TrapError(f"{case.name}: trapped {state['trap']}")
        if not state["sentinel"]:
            raise HarnessError(f"{case.name}: did not return within "
                               f"{self.instruction_budget} instructions; PC={self._pc(uc)}")

        regs = {name: uc.reg_read(reg) & 0xFFFF for name, reg in REGS.items()}
        output = {region.name: bytes(uc.mem_read(linear(region.segment, region.offset),
                                                 region.size))
                  for region in case.compare}
        return CallResult(regs, uc.reg_read(UC_X86_REG_EFLAGS) & 0xFFFF,
                          output, state["count"], "RET" if case.call == "near" else "RETF")

    @staticmethod
    def _pc(machine) -> str:
        cs = machine.reg_read(UC_X86_REG_CS) & 0xFFFF
        ip = machine.reg_read(UC_X86_REG_IP) & 0xFFFF
        return f"{cs:04X}:{ip:04X}"
