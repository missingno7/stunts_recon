"""Keep observed references, label gaps, and proven storage extents separate."""


def minimum_observed_access_bytes(accesses):
    """Return the furthest directly attributed operand end as a lower bound.

    Each access is an (offset, width) pair relative to the public label. Dynamic
    indexed references contribute only the addressed base/displacement recorded
    by the disassembler; callers must report separately that an index was seen.
    """
    ends = [offset + max(width, 1) for offset, width in accesses]
    return max(ends, default=0)


def array_dimension(allocated_extent_bytes, element_width):
    """Return a fixed element count only when a proven allocation permits it."""
    if not isinstance(allocated_extent_bytes, int) or allocated_extent_bytes <= 0:
        return None
    if not isinstance(element_width, int) or element_width <= 0:
        return None
    if allocated_extent_bytes % element_width:
        return None
    return allocated_extent_bytes // element_width
