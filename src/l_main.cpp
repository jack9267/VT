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
#include "SDL.h"
#include "l_main.h"

#define RCSID "$Id: l_main.cpp,v 1.10 2002/09/20 15:59:02 crow Exp $"

/// \brief reads the mesh data.
void TR_Level::read_mesh_data(SDL_RWops * const src)
{
	bitu8 *buffer;
	SDL_RWops *newsrc = NULL;
	bitu32 size;
	bitu32 pos = 0;
	int mesh = 0;
	bitu32 i;
	bitu32 num_mesh_data;

	num_mesh_data = read_bitu32(src);

	size = num_mesh_data * 2;
	buffer = new bitu8[size];

	try {
		if (SDL_RWread(src, buffer, 1, size) < (int)size)
			throw TR_ReadError ("read_tr_mesh_data: SDL_RWread(buffer)", __FILE__, __LINE__, RCSID);

		if ((newsrc = SDL_RWFromMem(buffer, size)) == NULL)
			throw TR_ReadError ("read_tr_mesh_data: SDL_RWFromMem", __FILE__, __LINE__, RCSID);

		this->mesh_indices.resize(read_bitu32(src));
		for (i = 0; i < this->mesh_indices.size(); i++)
			this->mesh_indices[i] = read_bitu32(src);

		this->meshes.resize(this->mesh_indices.size());

		for (i = 0; i < this->mesh_indices.size(); i++) {
			bitu32 j;

			for (j = 0; j < this->mesh_indices.size(); j++)
				if (this->mesh_indices[j] == pos)
					this->mesh_indices[j] = mesh;

			SDL_RWseek(newsrc, pos, SEEK_SET);

			if (this->game_version >= TR_IV)
				read_tr4_mesh(newsrc, this->meshes[mesh]);
			else
				read_tr_mesh(newsrc, this->meshes[mesh]);

			mesh++;

			for (j = 0; j < this->mesh_indices.size(); j++)
				if (this->mesh_indices[j] > pos) {
					pos = this->mesh_indices[j];
					break;
				}
		}
		SDL_RWclose(newsrc);
		newsrc = NULL;
		delete [] buffer;
	}
	catch(...) {
		if (newsrc)
			SDL_RWclose(newsrc);
		delete [] buffer;

		throw;
	}
}

/// \brief reads frame and moveable data.
void TR_Level::read_frame_moveable_data(SDL_RWops * const src)
{
	bitu32 i;
	bitu32 frame_data_size = read_bitu32(src) * 2;
	bitu8 *buffer = NULL;
	SDL_RWops *newsrc = NULL;
	bitu32 pos = 0;
	bitu32 frame = 0;

	buffer = new bitu8[frame_data_size];

	try {
		if (SDL_RWread(src, buffer, 1, frame_data_size) < (int)frame_data_size)
			throw TR_ReadError ("read_tr_level: frame_data: SDL_RWread(buffer)", __FILE__, __LINE__, RCSID);

		if ((newsrc = SDL_RWFromMem(buffer, frame_data_size)) == NULL)
			throw TR_ReadError ("read_tr_level: frame_data: SDL_RWFromMem", __FILE__, __LINE__, RCSID);

		this->moveables.resize(read_bitu32(src));
		for (i = 0; i < this->moveables.size(); i++)
			if (this->game_version < TR_V)
				read_tr_moveable(src, this->moveables[i]);
			else
				read_tr5_moveable(src, this->moveables[i]);

		//this->frames.reserve(this->moveables.size());
		for (i = 0; i < this->moveables.size(); i++) {
			bitu32 j;
			//tr_frame_t tr_frame;

			for (j = 0; j < this->moveables.size(); j++)
				if (this->moveables[j].frame_offset == pos) {
					this->moveables[j].frame_index = frame;
					this->moveables[j].frame_offset = 0;
				}

			SDL_RWseek(newsrc, pos, SEEK_SET);

/*
			try {
				if (this->game_version < TR_II)
					read_tr_frame(newsrc, tr_frame, this->moveables[i].num_meshes);
				else
					read_tr2_frame(newsrc, tr_frame, this->moveables[i].num_meshes);
				tr_frame.byte_offset = pos;
			}
			catch(TR_ReadError) {
				tr_frame.byte_offset = -(bit32)pos;
			}
			this->frames.push_back(tr_frame);
*/
			frame++;

			pos = 0;
			for (j = 0; j < this->moveables.size(); j++)
				if (this->moveables[j].frame_offset > pos) {
					pos = this->moveables[j].frame_offset;
					break;
				}
		}

		SDL_RWclose(newsrc);
		newsrc = NULL;
		delete [] buffer;
	}
	catch(...) {
		if (newsrc)
			SDL_RWclose(newsrc);
		delete [] buffer;

		throw;
	}
}

void TR_Level::read_level(const char *filename, tr_version_e game_version)
{
	this->read_level(SDL_RWFromFile(filename, "rb"), game_version);
}

/** \brief reads the level.
  *
  * Takes a SDL_RWop and the game_version of the file and reads the structures into the members of TR_Level.
  */
void TR_Level::read_level(SDL_RWops * const src, tr_version_e game_version)
{
	if (!src)
		throw TR_ReadError ("Invalid SDL_RWops", __FILE__, __LINE__, RCSID);

	this->game_version = game_version;

	switch (game_version) {
	case TR_I:
		read_tr_level(src, 0);
		break;
	case TR_I_DEMO:
	case TR_I_UB:
		read_tr_level(src, 1);
		break;
	case TR_II:
		read_tr2_level(src, 0);
		break;
	case TR_II_DEMO:
		read_tr2_level(src, 1);
		break;
	case TR_III:
		read_tr3_level(src);
		break;
	case TR_IV:
	case TR_IV_DEMO:
		read_tr4_level(src);
		break;
	case TR_V:
		read_tr5_level(src);
		break;
	default:
		throw TR_ReadError ("Invalid game version", __FILE__, __LINE__, RCSID);

		break;
	}

	SDL_RWclose(src);
}
