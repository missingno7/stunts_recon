/* Shared input declarations; DOS medium-model types. */
#ifndef STUNTS_INPUT_H
#define STUNTS_INPUT_H
extern signed char copy_mouse_modes[8];
extern char HKeyFlag;
extern signed char input_device_modestack[8];
extern int input_framecount;
extern int input_framecount2;
extern int input_framecount3;
extern int input_framecounter;
extern signed char input_status_stack_depth;
extern char kbormouse;
extern unsigned int mouse_api_y;
extern int mouse_button_state_cache;
extern int mouse_oldbut;
extern int mouse_oldx;
extern int mouse_oldy;
extern char mouse_transparent_mode;
extern int mousebutinputcode;
extern unsigned int mousehorscale;
extern char mouseonscreen;
extern int mouseshown;
extern void far clear_keyboardbuffer(void);
extern void far dohidemouse(void);
extern void far doshowmouse(void);
extern void far initmousedrive(int);
extern int far input_checking(int);
extern int far input_do_checking(int);
extern int far input_repeat_check(int);
extern void far kb_init_interrupt(void);
extern void far kb_line_edit_noop(void);
extern void far kb_shift_checking2(void);
extern void far keyboard_exit_handler(void);
extern void far keyboard_shift_checking1(void);
extern void far loadpalandmouse(void);
extern void far mouse_set_pixratio(unsigned int, unsigned int);
extern int far mouse_timer_sprite_unknown(int, int *, int *, int *, int *, int, int);
extern void far mousesteer(void);
extern void far pullmouse(void);
extern void far pushmouse(void);
extern void far reset_joystick_selection(void);
extern void far setmousex(int, int);
extern void far setmousey(unsigned int, unsigned int);
extern int far testkey(void);
#endif
