extern unsigned char byte_44A8A;
extern unsigned char byte_4552F;
extern unsigned short word_32D02;
extern unsigned char byte_449DA;
extern unsigned char byte_4393C;
extern short word_44DCA;
typedef void (far *callback_t)(void);
extern void far frame_callback(void);
extern void far timer_reg_callback(callback_t callback);
extern unsigned word_46468;
extern unsigned char byte_442E4;
extern unsigned long far timer_get_counter_unk(unsigned long ticks);
extern void far timer_remove_callback(callback_t callback);
void init_unknown(void)
{
	register int zero;
	byte_44A8A = 1;
	byte_4552F = 2;
	zero = 0;
	word_32D02 = zero;
	byte_4393C = byte_449DA = 0;
	word_44DCA = zero;
}

void far set_frame_callback(void) {
    word_46468 = 0;
    timer_reg_callback(frame_callback);
    byte_442E4 = 0;
}

void far remove_frame_callback(void)
{
    (void)timer_get_counter_unk(10L);
    timer_remove_callback(frame_callback);
}
