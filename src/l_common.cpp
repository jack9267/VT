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
#include <assert.h>
#include <string.h>
#include "SDL.h"
#include "SDL_endian.h"
#include "l_main.h"

#define RCSID "$Id: l_common.cpp,v 1.9 2002/09/20 15:59:02 crow Exp $"

/** \brief reads signed 8-bit value.
  *
  * uses current position from src. throws TR_ReadError when not successful.
  */
bit8 TR_Level::read_bit8(SDL_RWops * const src)
{
	bit8 data;

	if (src == NULL)
		throw TR_ReadError ("read_bit8: src == NULL", __FILE__, __LINE__, RCSID);

	if (SDL_RWread(src, &data, 1, 1) < 1)
		throw TR_ReadError ("read_bit8", __FILE__, __LINE__, RCSID);

	return data;
}

/** \brief reads unsigned 8-bit value.
  *
  * uses current position from src. throws TR_ReadError when not successful.
  */
bitu8 TR_Level::read_bitu8(SDL_RWops * const src)
{
	bitu8 data;

	if (src == NULL)
		throw TR_ReadError ("read_bitu8: src == NULL", __FILE__, __LINE__, RCSID);

	if (SDL_RWread(src, &data, 1, 1) < 1)
		throw TR_ReadError ("read_bitu8", __FILE__, __LINE__, RCSID);

	return data;
}

/** \brief reads signed 16-bit value.
  *
  * uses current position from src. does endian correction. throws TR_ReadError when not successful.
  */
bit16 TR_Level::read_bit16(SDL_RWops * const src)
{
	bit16 data;

	if (src == NULL)
		throw TR_ReadError ("read_bit16: src == NULL", __FILE__, __LINE__, RCSID);

	if (SDL_RWread(src, &data, 2, 1) < 1)
		throw TR_ReadError ("read_bit16", __FILE__, __LINE__, RCSID);

	data = SDL_SwapLE16(data);

	return data;
}

/** \brief reads unsigned 16-bit value.
  *
  * uses current position from src. does endian correction. throws TR_ReadError when not successful.
  */
bitu16 TR_Level::read_bitu16(SDL_RWops * const src)
{
	bitu16 data;

	if (src == NULL)
		throw TR_ReadError ("read_bitu16: src == NULL", __FILE__, __LINE__, RCSID);

	if (SDL_RWread(src, &data, 2, 1) < 1)
		throw TR_ReadError ("read_bitu16", __FILE__, __LINE__, RCSID);

	data = SDL_SwapLE16(data);

	return data;
}

/** \brief reads signed 32-bit value.
  *
  * uses current position from src. does endian correction. throws TR_ReadError when not successful.
  */
bit32 TR_Level::read_bit32(SDL_RWops * const src)
{
	bit32 data;

	if (src == NULL)
		throw TR_ReadError ("read_bit32: src == NULL", __FILE__, __LINE__, RCSID);

	if (SDL_RWread(src, &data, 4, 1) < 1)
		throw TR_ReadError ("read_bit32", __FILE__, __LINE__, RCSID);

	data = SDL_SwapLE32(data);

	return data;
}

/** \brief reads unsigned 32-bit value.
  *
  * uses current position from src. does endian correction. throws TR_ReadError when not successful.
  */
bitu32 TR_Level::read_bitu32(SDL_RWops * const src)
{
	bitu32 data;

	if (src == NULL)
		throw TR_ReadError ("read_bitu32: src == NULL", __FILE__, __LINE__, RCSID);

	if (SDL_RWread(src, &data, 4, 1) < 1)
		throw TR_ReadError ("read_bitu32", __FILE__, __LINE__, RCSID);

	data = SDL_SwapLE32(data);

	return data;
}

/** \brief reads float value.
  *
  * uses current position from src. does endian correction. throws TR_ReadError when not successful.
  */
float TR_Level::read_float(SDL_RWops * const src)
{
	float data;

	if (src == NULL)
		throw TR_ReadError ("read_float: src == NULL", __FILE__, __LINE__, RCSID);

	if (SDL_RWread(src, &data, 4, 1) < 1)
		throw TR_ReadError ("read_float", __FILE__, __LINE__, RCSID);

	data = SDL_SwapLE32(data);

	return data;
}
