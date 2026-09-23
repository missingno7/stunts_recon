unsigned int update_rpm_from_speed(unsigned int currpm, unsigned int speed,
                                   unsigned int gearratio, int changing_gear,
                                   unsigned int idle_rpm)
{
    if (changing_gear == 0) {
        unsigned int a0 = speed & 255U;
        unsigned int a1 = speed >> 8;
        unsigned int b0 = gearratio & 255U;
        unsigned int b1 = gearratio >> 8;
        unsigned int p00 = a0 * b0;
        unsigned int p01 = a0 * b1;
        unsigned int p10 = a1 * b0;
        unsigned int p11 = a1 * b1;
        unsigned int middle = (p00 >> 8) + (p01 & 255U) + (p10 & 255U);
        currpm = p11 + (p01 >> 8) + (p10 >> 8) + (middle >> 8);
    }
    if (currpm >= idle_rpm) return currpm;
    return idle_rpm;
}
