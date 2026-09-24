short far multiply_and_scale(short a1, short a2) {
    unsigned long product = (unsigned long)(long)a1 * (unsigned long)(long)a2;
    unsigned long scaled = product << 2;
    return (short)((scaled >> 16) + ((scaled & 0x8000UL) >> 15));
}
