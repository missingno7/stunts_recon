typedef void (far *readchar_callback_t)(void);
extern readchar_callback_t kb_readchar_callback;
readchar_callback_t nopsub_kb_get_readchar_callback(void) {
    return kb_readchar_callback;
}
