extern signed char byte_3EBD8;
extern signed char byte_3B8F7;
extern signed char kbormouse;
extern signed char byte_45D0C[];
extern signed char byte_45D14[];

void far input_push_status(void)
{
    byte_45D0C[byte_3EBD8] = byte_3B8F7;
    byte_45D14[byte_3EBD8] = kbormouse;
    ++byte_3EBD8;
}
