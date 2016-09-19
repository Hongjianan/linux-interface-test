/*
 * cursesTest.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: hong
 */

/*	Note: Dec 27, 2014
 *	1. Programming Model
 *		1.1 initial terminal
 *		1.2 operate terminal (input/output/fresh/setcur)
 *		1.3 release terminal
 *
 *	2. Display
 *		2.1 grapy output
 *			2.1.1 border(argc == 8): output border
 *					wborder
 *			2.1.2 box
 *			2.1.3 hline(stdscr)
 *				  whline(specified window)
 *				  mvhline(specified position)
 *				  mvwhline(specified position in specified window)
 *			2.1.4 vline
 *
 *			notice:
 *				1. chtype = type byte + character byte
 *				2. initsrc() will return WINDOW*.(stardar screen)
 *				3. curses has defined a global variable called stdscr.
 *					eg:
 *						WINDOW *win = initsrc();
 *						box(win, 0, 0);
 *						is equal to
 *						box(stdscr, 0, 0);
 *				4. function named rules
 *					xxx		stdscr
 *					w***	specified window
 *					mv***	specified position in stdscr
 *					mvw***	specified position in specified window
 *
 *		2.2 fresh
 *			2.2.1 void refresh(void)
 *				  void wrefresh(WINDOW *)
 *
 *		2.3 character output
 *			2.3.1 normal character
 *			2.3.2 attribute character: 'A' | A_BOLD
 *			2.3.3 special attribute character: ACS_PI
 *
 *		2.4 string output
 *			2.4.1 addstr:
 *
 *	3. color attribute
 *		3.1 has_colors: judge terminal support for color
 *		3.2 start_color: initial color
 *		3.3 init_pair: set color pair
 *		3.4 bkgd: set back color and fill character
 *		3.5 COLOR_PAIR(short): use color pair
 *		3.6 attron: turn on character color attribute
 *		3.7 attroff: turn off character color attribute
 *
 *		notice:
 *			1. these functions must use after initscr().
 *
 *
 *
 *
 *
 * */
#include "../config.h"
#if CURSESTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <curses.h>


int CursesTest_border(int argc, char *argv[]);
int CursesTest_addch(int argc, char *argv[]);

/*===============main===============*/
int CursesTest(int argc, char *argv[])
{
	int ret;

//	ret = CursesTest_border(argc, argv);
	ret = CursesTest_addch(argc, argv);

	return ret;
}


int CursesTest_border(int argc, char *argv[])
{
	/* 1. initial terminal */
	WINDOW *win = initscr();

	/* 2. draw border */
//	border('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h');
//	border(0, 0, 0, 0, 0, 0, 0, 0);

//	box(win, 0, 0);
	box(stdscr, 0, 0);

	/* 3. draw line */
	mvhline(2, 10, '-', 20);
	mvvline(2, 10, '|', 20);

	/* 4. fresh */
	refresh();

	/* 5. wait here */
	getch();

	/* 6. release terminal */
	endwin();

	return 0;
}


int CursesTest_addch(int argc, char *argv[])
{
	WINDOW *win = initscr();

	/* set color pair */
	if (TRUE == has_colors())
	{
		start_color();
		init_pair(1, COLOR_RED, COLOR_WHITE);
		init_pair(2, COLOR_BLUE, COLOR_WHITE);
		init_pair(3, COLOR_BLACK, COLOR_WHITE);
		bkgd(COLOR_PAIR(3));
//		bkgd('a' | COLOR_PAIR(3));
	}

	box(stdscr, 0, 0);

	addch('H');
	mvaddch(10, 10, 'O');
	mvaddch(10, 11, 'O' | A_UNDERLINE | A_BOLD | A_BLINK | A_REVERSE | COLOR_PAIR(1));
	mvaddch(10, 12, 'G' | A_UNDERLINE | A_BOLD | A_BLINK | A_REVERSE);	/* man addron */
	mvaddch(10, 13, ACS_PI | COLOR_PAIR(2));	/* man addch */

	attron(COLOR_PAIR(1) | A_UNDERLINE);
	mvaddstr(12, 10, "hello hongjianan" );
	attroff(COLOR_PAIR(1));

	mvaddstr(13, 10, "hello hongjianan" );

	getch();

	endwin();

	return 0;
}

#endif /* CURSESTEST_CPP */
/* end file */
