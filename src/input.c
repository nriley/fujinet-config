\
/** 
 * Input routines
 */

#include <msx.h>
#include <eos.h>
#include <conio.h>
#include <stdbool.h>
#include "input.h"

static GameControllerData cont;
static unsigned char key=0;
static unsigned char joystick=0;
static unsigned char joystick_copy=0;
static unsigned char button=0;
static unsigned char button_copy=0;
static unsigned char keypad=0;
static unsigned char keypad_copy=0;
static unsigned char repeat=0;

/**
 * Get input from keyboard/joystick
 * @return keycode (or synthesized keycode if joystick)
 */
unsigned char input()
{
  key = eos_end_read_keyboard();
  
  if (key > 1)
    {
      eos_start_read_keyboard();
      return key;
    }
  else
    {
      eos_read_game_controller(0x03,&cont);
      joystick = cont.joystick1 | cont.joystick2;
      button = cont.joystick1_button_left | cont.joystick1_button_right | cont.joystick2_button_left | cont.joystick2_button_right;
      keypad = cont.joystick1_keypad;

      if ((button > 0) && (button != button_copy))
	{
	  key=0x0d; // same as RETURN
	}
      else if ((keypad != 0x0F) && (keypad != keypad_copy))
	{
	  switch (keypad)
	    {
	    case 1: // Slot 1
	      key=0x83;
	      break;
	    case 2: // Slot 2
	      key=0x84;
	      break;
	    case 3: // Slot 3
	      key=0x85;
	      break;
	    case 4: // Slot 4
	      key=0x86;
	      break;
	    }
	}
      else if ((joystick > 0) && (joystick == joystick_copy))
	repeat++;
      else
	repeat=0;

      if (repeat > 16)
	{
	  repeat=0;
	  switch(joystick)
	    {
	    case 1: // UP
	      key=0xA0;
	      break;
	    case 4: // DOWN
	      key=0xA2;
	      break;
	    }
	}
      
      joystick_copy = joystick;
      button_copy = button;
      keypad_copy = keypad;
    }
  
  return key;
}

unsigned char input_ucase(void)
{
  unsigned char c = input();
  if ((c>='a') && (c<='z')) c&=~32;
  return c;
}

static void input_clear_bottom(void)
{
  msx_vfill(0x1200,0x00,768);
}

unsigned char input_line(char *c, bool password)
{
  unsigned char pos=0;
  
  input_clear_bottom();

  gotoxy(0,18);

  while (key = eos_read_keyboard())
    {
      if (key == 0x0d)
	break;
      else if (key == 0x08)
	{
	  if (pos > 0)
	    {
	      pos--;
	      c--;
	      putchar(0x08);
	      putchar(0x20);
	      putchar(0x08);
	    }
	}
      else if (key > 0x1F && key < 0x7F) 
	{
	  pos++;
	  *c=key;
	  c++;
	  putchar(password ? 0x7F : key);
	}
    }
}