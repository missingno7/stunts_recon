short far multiply_and_scale(short a1, short a2) {
    long mul = (long)a1 * (long)a2 * 4L;
    return (mul >> 16) + ((mul & 0x8000L) >> 15);
}
