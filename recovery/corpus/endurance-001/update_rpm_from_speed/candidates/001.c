unsigned int update_rpm_from_speed(unsigned int currpm, unsigned int speed,
                                   unsigned int gearratio, int changing_gear,
                                   unsigned int idle_rpm)
{
    if (changing_gear == 0) {
        currpm = ((unsigned long)speed * gearratio) >> 16;
    }
    if (currpm >= idle_rpm) return currpm;
    return idle_rpm;
}
