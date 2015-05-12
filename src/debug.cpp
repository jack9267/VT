/*
 * Copyright 2002 - Florian Schulze <crow@icculus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * This file is part of vt.
 *
 */

#include "debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static FILE *debug_file = NULL;
static int debug_file_inited = 0;

static void d_cleanup(void)
{
	if (debug_file != NULL) {
		dbg_printf("</body>\n");
		fclose(debug_file);
	}
	debug_file = NULL;
}

int dbg_printf(const char *fmt, ...)
{
	va_list v;
	int r;

	if (debug_file_inited == 0) {
		debug_file_inited = 1;
		atexit(d_cleanup);
		debug_file = fopen("debug.html", "w");
		setvbuf(debug_file, NULL, _IONBF, 0);
		dbg_printf("<html><head><title>Debug Output</title><style type=\"text/css\">\n");
		dbg_printf("<!--\n");
		dbg_printf("table.gray { background:#a0a0a0; }\n");
		dbg_printf("//-->\n");
		dbg_printf("</style></head>\n<body>\n");
	}
	va_start(v, fmt);
	r = 0;
	if (debug_file != NULL)
		r = vfprintf(debug_file, fmt, v);
	va_end(v);
	return r;
}
