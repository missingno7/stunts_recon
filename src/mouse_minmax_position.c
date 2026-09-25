void mouse_set_minmax(int, int, int, int);
void mouse_set_position(int, int);
void mouse_minmax_position(int enabled)
{
    if (enabled) {
        mouse_set_minmax(15, 0, 0x131, 0xC8);
        mouse_set_position(0xA0, 0x64);
        return;
    }
    mouse_set_minmax(0, 0, 0x140, 0xC8);
}
