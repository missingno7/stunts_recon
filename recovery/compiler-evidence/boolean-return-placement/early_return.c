int classify(long value, int invalid) {
    if (invalid) return 0;
    return (char)(value > 0);
}
