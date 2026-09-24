"""Strict single-value numeric DB/DW parser for imported source emissions."""
import re

_NUMERIC = re.compile(
    r"(?:(?:[a-z_?@][a-z0-9_?@$]*)\s+)?(db|dw)\s+([0-9][0-9a-f]*h|[0-9]+)",
    re.I,
)


def numeric_literal_bytes(source):
    """Return source-declared bytes, or None for every unsupported expression."""
    match = _NUMERIC.fullmatch(source.strip())
    if match is None:
        return None
    directive, token = match.groups()
    number = int(token[:-1], 16) if token.lower().endswith('h') else int(token)
    width = 1 if directive.lower() == 'db' else 2
    if number >= (1 << (8 * width)):
        return None
    return number.to_bytes(width, 'little')
