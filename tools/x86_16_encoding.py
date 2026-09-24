"""Byte-level checks for source mnemonics with ambiguous Capstone aliases."""


def conversion_matches_source(source_mnemonic, encoded):
    # In 16-bit operand size these one-byte opcodes are CBW and CWD. An 0x66
    # prefix changes the operation; Capstone 5.0.3 labels even unprefixed 98/99
    # as CWDE/CDQ, so mnemonic-only comparison loses this distinction.
    return encoded == {'cbw': b'\x98', 'cwd': b'\x99'}.get(source_mnemonic)


def reviewed_nop_literal(source_text):
    """One importer-safe explicit padding spelling; all other DB stays data."""
    return b'\x90' if source_text == 'db 144' else None


STRING_OPCODES = {
    'movsb': 0xA4, 'movsw': 0xA5,
    'cmpsb': 0xA6, 'cmpsw': 0xA7,
    'stosb': 0xAA, 'stosw': 0xAB,
    'lodsb': 0xAC, 'lodsw': 0xAD,
    'scasb': 0xAE, 'scasw': 0xAF,
}


def bare_string_opcode_matches_source(source_mnemonic, encoded):
    """Ignore decoder-rendered implicit operands, but require exact opcode."""
    opcode = STRING_OPCODES.get(source_mnemonic)
    return opcode is not None and encoded == bytes([opcode])
