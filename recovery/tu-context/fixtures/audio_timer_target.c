void far audio_add_driver_timer(void) {
    unsigned short p = 0x6364;
    do {
        *(unsigned char *)p = 0;
        p += 76;
    } while (p < 0x6ad0);
    *(unsigned short *)0x6ad0 = 22;
    timer_reg_callback(audio_driver_timer);
}
