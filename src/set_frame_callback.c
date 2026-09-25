typedef void (far *callback_t)(void);
extern void far frame_callback(void);
extern void far timer_reg_callback(callback_t callback);
extern unsigned word_46468;
extern unsigned char byte_442E4;
void far set_frame_callback(void) {
    word_46468 = 0;
    timer_reg_callback(frame_callback);
    byte_442E4 = 0;
}
