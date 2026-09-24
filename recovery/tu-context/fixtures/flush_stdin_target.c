void flush_stdin(void) { while (kb_call_readchar_callback() == 0) {} }
