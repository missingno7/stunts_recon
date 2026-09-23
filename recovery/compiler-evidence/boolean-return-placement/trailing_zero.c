int classify(long value, int invalid) {
    if (invalid) goto zero;
    return (char)(value > 0);
zero:
    return 0;
}
