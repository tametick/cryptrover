//modified for cryptrover
/*
    mdport.c - Machine Dependent Code for Porting Unix/Curses games

    Copyright (C) 2005 Nicholas J. Kisseberth
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name(s) of the author(s) nor the names of other contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

#ifdef __PDC__
#include "../lib/curses.h"
#else
#include <ncurses.h>
#endif
#include "mdport.h"

#define KP_5 69
#define MOD_MOVE(c) (toupper(c) )

/*
    All cursor/keypad results are translated into standard rogue movement
    commands.

    Unmodified keys are translated to walk commands: hjklyubn
    Modified (shift,control,alt) are translated to run commands: HJKLYUBN
*/

#define M_NORMAL 0
#define M_ESC    1
#define M_KEYPAD 2
#define M_TRAIL  3

int md_readchar(WINDOW * win) {
	int ch = 0;
	int lastch = 0;
	int mode = M_NORMAL;
	int mode2 = M_NORMAL;

	for (;;) {
		ch = wgetch(win);

		/* timed out waiting for valid sequence
		   flush input so far and start over */
		if (ch == ERR) {
			mode = M_NORMAL;
			nocbreak();
			raw();
			ch = 27;
			break;
		}

		if (mode == M_TRAIL) {
			if (ch == '^') /* msys console  : 7,5,6,8: modified */
				ch = MOD_MOVE(toupper(lastch));

			if (ch == '~') /* cygwin console: 1,5,6,4: normal  */
				ch = tolower(lastch); /* windows telnet: 1,5,6,4: normal  */
			/* msys console  : 7,5,6,8: normal  */

			if (mode2 == M_ESC) /* cygwin console: 1,5,6,4: modified */
				ch = MOD_MOVE(toupper(ch));

			break;
		}

		if (mode == M_ESC) {
			if (ch == 27) {
				mode2 = M_ESC;
				continue;
			}

			if ((ch == 'F') || (ch == 'O') || (ch == '[')) {
				mode = M_KEYPAD;
				continue;
			}

			switch (ch) {
				/* Cygwin Console   */
				/* PuTTY            */
			case KEY_LEFT:
				ch = MOD_MOVE('H');
				break;
			case KEY_RIGHT:
				ch = MOD_MOVE('L');
				break;
			case KEY_UP:
				ch = MOD_MOVE('K');
				break;
			case KEY_DOWN:
				ch = MOD_MOVE('J');
				break;
			case KEY_HOME:
				ch = MOD_MOVE('Y');
				break;
			case KEY_PPAGE:
				ch = MOD_MOVE('U');
				break;
			case KEY_NPAGE:
				ch = MOD_MOVE('N');
				break;
			case KEY_END:
				ch = MOD_MOVE('B');
				break;

			default:
				break;
			}

			break;
		}

		if (mode == M_KEYPAD) {
			switch (ch) {
				/* ESC F - Interix Console codes */
			case '^':
				ch = MOD_MOVE('H');
				break; /* Shift-Left       */
			case '$':
				ch = MOD_MOVE('L');
				break; /* Shift-Right      */

				/* ESC [ - Interix Console codes */
			case 'H':
				ch = 'y';
				break; /* Home             */
			case 1:
				ch = MOD_MOVE('K');
				break; /* Ctl-Keypad Up    */
			case 2:
				ch = MOD_MOVE('J');
				break; /* Ctl-Keypad Down  */
			case 3:
				ch = MOD_MOVE('L');
				break; /* Ctl-Keypad Right */
			case 4:
				ch = MOD_MOVE('H');
				break; /* Ctl-Keypad Left  */
			case 263:
				ch = MOD_MOVE('Y');
				break; /* Ctl-Keypad Home  */
			case 19:
				ch = MOD_MOVE('U');
				break; /* Ctl-Keypad PgUp  */
			case 20:
				ch = MOD_MOVE('N');
				break; /* Ctl-Keypad PgDn  */
			case 21:
				ch = MOD_MOVE('B');
				break; /* Ctl-Keypad End   */

				/* ESC [ - Cygwin Console codes */
			case 'G':
				ch = '.';
				break; /* Keypad 5         */
			case '7':
				lastch = 'Y';
				mode = M_TRAIL;
				break; /* Ctl-Home */
			case '5':
				lastch = 'U';
				mode = M_TRAIL;
				break; /* Ctl-PgUp */
			case '6':
				lastch = 'N';
				mode = M_TRAIL;
				break; /* Ctl-PgDn */

				/* ESC [ - Win32 Telnet, PuTTY */
			case '1':
				lastch = 'y';
				mode = M_TRAIL;
				break; /* Home     */
			case '4':
				lastch = 'b';
				mode = M_TRAIL;
				break; /* End      */

				/* ESC O - PuTTY */
			case 'D':
				ch = MOD_MOVE('H');
				break;
			case 'C':
				ch = MOD_MOVE('L');
				break;
			case 'A':
				ch = MOD_MOVE('K');
				break;
			case 'B':
				ch = MOD_MOVE('J');
				break;
			case 't':
				ch = 'h';
				break;
			case 'v':
				ch = 'l';
				break;
			case 'x':
				ch = 'k';
				break;
			case 'r':
				ch = 'j';
				break;
			case 'w':
				ch = 'y';
				break;
			case 'y':
				ch = 'u';
				break;
			case 's':
				ch = 'n';
				break;
			case 'q':
				ch = 'b';
				break;
			case 'u':
			case KP_5:
				ch = '.';
				break;
			}

			if (mode != M_KEYPAD)
				continue;
		}

		if (ch == 27) {
			halfdelay(1);
			mode = M_ESC;
			continue;
		}

		switch (ch) {
		case KEY_LEFT:
			ch = 'h';
			break;
		case KEY_DOWN:
			ch = 'j';
			break;
		case KEY_UP:
			ch = 'k';
			break;
		case KEY_RIGHT:
			ch = 'l';
			break;
		case KEY_HOME:
			ch = 'y';
			break;
		case KEY_PPAGE:
			ch = 'u';
			break;
		case KEY_END:
			ch = 'b';
			break;
#ifdef KEY_LL
		case KEY_LL:
			ch = 'b';
			break;
#endif
		case KEY_NPAGE:
			ch = 'n';
			break;

#ifdef KEY_B1
		case KEY_B1:
			ch = 'h';
			break;
		case KEY_C2:
			ch = 'j';
			break;
		case KEY_A2:
			ch = 'k';
			break;
		case KEY_B3:
			ch = 'l';
			break;
#endif
		case KEY_A1:
			ch = 'y';
			break;
		case KEY_A3:
			ch = 'u';
			break;
		case KEY_C1:
			ch = 'b';
			break;
		case KEY_C3:
			ch = 'n';
			break;
		case KEY_B2:
			ch = '.';
			break;

#ifdef KEY_SLEFT
		case KEY_SRIGHT:
			ch = MOD_MOVE('L');
			break;
		case KEY_SLEFT:
			ch = MOD_MOVE('H');
			break;
#ifdef KEY_SUP
		case KEY_SUP:
			ch = MOD_MOVE('K');
			break;
		case KEY_SDOWN:
			ch = MOD_MOVE('J');
			break;
#endif
		case KEY_SHOME:
			ch = MOD_MOVE('Y');
			break;
		case KEY_SPREVIOUS:
			ch = MOD_MOVE('U');
			break;
		case KEY_SEND:
			ch = MOD_MOVE('B');
			break;
		case KEY_SNEXT:
			ch = MOD_MOVE('N');
			break;
#endif
		case 0x146:
			ch = MOD_MOVE('K');
			break; /* Shift-Up */
		case 0x145:
			ch = MOD_MOVE('J');
			break; /* Shift-Down */

#ifdef CTL_RIGHT
		case CTL_RIGHT:
			ch = MOD_MOVE('L');
			break;
		case CTL_LEFT:
			ch = MOD_MOVE('H');
			break;
		case CTL_UP:
			ch = MOD_MOVE('K');
			break;
		case CTL_DOWN:
			ch = MOD_MOVE('J');
			break;
		case CTL_HOME:
			ch = MOD_MOVE('Y');
			break;
		case CTL_PGUP:
			ch = MOD_MOVE('U');
			break;
		case CTL_END:
			ch = MOD_MOVE('B');
			break;
		case CTL_PGDN:
			ch = MOD_MOVE('N');
			break;
#endif
#ifdef KEY_EOL
		case KEY_EOL:
			ch = MOD_MOVE('B');
			break;
#endif

#ifndef CTL_PAD1
			/* MSYS rxvt console */
		case 511:
			ch = MOD_MOVE('J');
			break;/* Shift Dn */
		case 512:
			ch = MOD_MOVE('J');
			break;/* Ctl Down */
		case 514:
			ch = MOD_MOVE('H');
			break;/* Ctl Left */
		case 516:
			ch = MOD_MOVE('L');
			break;/* Ctl Right */
		case 518:
			ch = MOD_MOVE('K');
			break;/* Shift Up */
		case 519:
			ch = MOD_MOVE('K');
			break;/* Ctl Up   */
#endif

#ifdef CTL_PAD1
		case CTL_PAD1:
			ch = MOD_MOVE('B');
			break;
		case CTL_PAD2:
			ch = MOD_MOVE('J');
			break;
		case CTL_PAD3:
			ch = MOD_MOVE('N');
			break;
		case CTL_PAD4:
			ch = MOD_MOVE('H');
			break;
		case CTL_PAD5:
			ch = '.';
			break;
		case CTL_PAD6:
			ch = MOD_MOVE('L');
			break;
		case CTL_PAD7:
			ch = MOD_MOVE('Y');
			break;
		case CTL_PAD8:
			ch = MOD_MOVE('K');
			break;
		case CTL_PAD9:
			ch = MOD_MOVE('U');
			break;
#endif

#ifdef ALT_RIGHT
		case ALT_RIGHT:
			ch = MOD_MOVE('L');
			break;
		case ALT_LEFT:
			ch = MOD_MOVE('H');
			break;
		case ALT_DOWN:
			ch = MOD_MOVE('J');
			break;
		case ALT_HOME:
			ch = MOD_MOVE('Y');
			break;
		case ALT_PGUP:
			ch = MOD_MOVE('U');
			break;
		case ALT_END:
			ch = MOD_MOVE('B');
			break;
		case ALT_PGDN:
			ch = MOD_MOVE('N');
			break;
#endif

#ifdef ALT_PAD1
		case ALT_PAD1:
			ch = MOD_MOVE('B');
			break;
		case ALT_PAD2:
			ch = MOD_MOVE('J');
			break;
		case ALT_PAD3:
			ch = MOD_MOVE('N');
			break;
		case ALT_PAD4:
			ch = MOD_MOVE('H');
			break;
		case ALT_PAD5:
			ch = '.';
			break;
		case ALT_PAD6:
			ch = MOD_MOVE('L');
			break;
		case ALT_PAD7:
			ch = MOD_MOVE('Y');
			break;
		case ALT_PAD8:
			ch = MOD_MOVE('K');
			break;
		case ALT_PAD9:
			ch = MOD_MOVE('U');
			break;
#endif
		}

		break;
	}

	nocbreak(); /* disable halfdelay mode if on */
	raw();

	return (ch & 0x7F);
}
