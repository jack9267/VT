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
#include "zlib.h"

#define RCSID "$Id: l_tr5.cpp,v 1.14 2002/09/20 15:59:02 crow Exp $"

void TR_Level::read_tr5_room_light(SDL_RWops * const src, tr5_room_light_t & light)
{
	bitu32 temp;

	read_tr4_vertex_float(src, light.pos);
	//read_tr_colour(src, light.color);
	read_float(src);	// r
	read_float(src);	// g
	read_float(src);	// b
	temp = read_bitu32(src);
/*
	if ((temp != 0) && (temp != 0xCDCDCDCD))
		throw TR_ReadError("read_tr5_room_light: seperator1 has wrong value");
*/
	light.r_inner = read_float(src);
	light.r_outer = read_float(src);
	read_float(src);	// rad_input
	read_float(src);	// rad_output
	read_float(src);	// range
	read_tr4_vertex_float(src, light.dir);
	read_tr_vertex32(src, light.pos2);
	read_tr_vertex32(src, light.dir2);
	light.light_type = read_bitu8(src);
	temp = read_bitu8(src);
	if (temp != 0xCD)
		throw TR_ReadError ("read_tr5_room_light: seperator2 has wrong value", __FILE__, __LINE__, RCSID);

	temp = read_bitu8(src);
	if (temp != 0xCD)
		throw TR_ReadError ("read_tr5_room_light: seperator3 has wrong value", __FILE__, __LINE__, RCSID);

	temp = read_bitu8(src);
	if (temp != 0xCD)
		throw TR_ReadError ("read_tr5_room_light: seperator4 has wrong value", __FILE__, __LINE__, RCSID);
}

void TR_Level::read_tr5_room_layer(SDL_RWops * const src, tr5_room_layer_t & layer)
{
	layer.num_vertices = read_bitu16(src);
	layer.unknown_l1 = read_bitu16(src);
	if (layer.unknown_l1 != 0)
		layer.unknown_l1 = layer.unknown_l1;
	layer.unknown_l2 = read_bitu16(src);
	if (layer.unknown_l2 != 0)
		layer.unknown_l2 = layer.unknown_l2;
	layer.num_rectangles = read_bitu16(src);
	layer.num_triangles = read_bitu16(src);
	layer.unknown_l3 = read_bitu16(src);
	if (layer.unknown_l3 != 0)
		layer.unknown_l3 = layer.unknown_l3;
	layer.unknown_l4 = read_bitu16(src);
	if (layer.unknown_l4 != 0)
		layer.unknown_l4 = layer.unknown_l4;
	if (read_bitu16(src) != 0)
		throw TR_ReadError ("read_tr5_room_layer: filler2 has wrong value", __FILE__, __LINE__, RCSID);

	layer.bounding_box_x1 = read_float(src);
	layer.bounding_box_y1 = -read_float(src);
	layer.bounding_box_z1 = -read_float(src);
	layer.bounding_box_x2 = read_float(src);
	layer.bounding_box_y2 = -read_float(src);
	layer.bounding_box_z2 = -read_float(src);
	if (read_bitu32(src) != 0)
		throw TR_ReadError ("read_tr5_room_layer: filler3 has wrong value", __FILE__, __LINE__, RCSID);

	layer.unknown_l6a = read_bit16(src);
	layer.unknown_l6b = read_bit16(src);
	layer.unknown_l7a = read_bit16(src);
	layer.unknown_l7b = read_bit16(src);
	layer.unknown_l8a = read_bit16(src);
	layer.unknown_l8b = read_bit16(src);
}

void TR_Level::read_tr5_room_vertex(SDL_RWops * const src, tr5_room_vertex_t & vert)
{
	read_tr4_vertex_float(src, vert.vertex);
	read_tr4_vertex_float(src, vert.normal);
	vert.colour.b = read_bitu8(src) / 255.0f;
	vert.colour.g = read_bitu8(src) / 255.0f;
	vert.colour.r = read_bitu8(src) / 255.0f;
	vert.colour.a = read_bitu8(src) / 255.0f;
}

void TR_Level::read_tr5_room(SDL_RWops * const src, tr5_room_t & room)
{
	bitu32 room_data_size;
	bitu32 portal_offset;
	bitu32 sector_data_offset;
	bitu32 static_meshes_offset;
	bitu32 layer_offset;
	bitu32 vertices_offset;
	bitu32 poly_offset;
	bitu32 poly_offset2;
	bitu32 vertices_size;
	bitu32 light_size;

	SDL_RWops *newsrc = NULL;
	bitu32 temp;
	bitu32 i;
	bitu8 *buffer;

	if (read_bitu32(src) != 0x414C4558)
		throw TR_ReadError ("read_tr5_room: 'XELA' not found", __FILE__, __LINE__, RCSID);

	room_data_size = read_bitu32(src);
	buffer = new bitu8[room_data_size];

	try {
		if (SDL_RWread(src, buffer, 1, room_data_size) < (int)room_data_size)
			throw TR_ReadError ("read_tr5_room: room_data", __FILE__, __LINE__, RCSID);

		if ((newsrc = SDL_RWFromMem(buffer, room_data_size)) == NULL)
			throw TR_ReadError ("read_tr5_room: SDL_RWFromMem", __FILE__, __LINE__, RCSID);

		room.intensity1 = 32767;
		room.intensity2 = 32767;
		room.light_mode = 0;
		room.alternate_room = 0;

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator1 has wrong value", __FILE__, __LINE__, RCSID);

		portal_offset = read_bit32(newsrc);	// StartPortalOffset?
		sector_data_offset = read_bitu32(newsrc);	// StartSDOffset
		temp = read_bitu32(newsrc);
		if ((temp != 0) && (temp != 0xCDCDCDCD))
			throw TR_ReadError ("read_tr5_room: seperator2 has wrong value", __FILE__, __LINE__, RCSID);

		static_meshes_offset = read_bitu32(newsrc);

		// read and change coordinate system
		room.offset.x = (float)read_bit32(src);
		room.offset.y = (float)-read_bit32(src);
		room.offset.z = (float)-read_bit32(src);
		room.y_bottom = (float)-read_bit32(newsrc);
		room.y_top = (float)-read_bit32(newsrc);

		room.num_zsectors = read_bitu16(newsrc);
		room.num_xsectors = read_bitu16(newsrc);

		room.light_colour.b = read_bitu8(newsrc) / 255.0f;
		room.light_colour.g = read_bitu8(newsrc) / 255.0f;
		room.light_colour.r = read_bitu8(newsrc) / 255.0f;
		room.light_colour.a = read_bitu8(newsrc) / 255.0f;

		room.num_lights = read_bitu16(newsrc);
		if (room.num_lights > 512)
			throw TR_ReadError ("read_tr5_room: num_lights > 512", __FILE__, __LINE__, RCSID);

		room.num_static_meshes = read_bitu16(newsrc);
		if (room.num_static_meshes > 512)
			throw TR_ReadError ("read_tr5_room: num_static_meshes > 512", __FILE__, __LINE__, RCSID);

		room.unknown_r1 = read_bitu16(newsrc);
		room.unknown_r2 = read_bitu16(newsrc);

		if (read_bitu32(newsrc) != 0x00007FFF)
			throw TR_ReadError ("read_tr5_room: filler1 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0x00007FFF)
			throw TR_ReadError ("read_tr5_room: filler2 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator4 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator5 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xFFFFFFFF)
			throw TR_ReadError ("read_tr5_room: seperator6 has wrong value", __FILE__, __LINE__, RCSID);

		room.unknown_r3 = read_bit16(newsrc);

		room.flags = read_bitu16(newsrc);

		room.unknown_r4 = read_bitu32(newsrc);
		room.unknown_r5 = read_bitu32(newsrc);
		room.unknown_r6 = read_bitu32(newsrc);

		temp = read_bitu32(newsrc);
		if ((temp != 0) && (temp != 0xCDCDCDCD))
			throw TR_ReadError ("read_tr5_room: seperator7 has wrong value", __FILE__, __LINE__, RCSID);

		room.unknown_r7a = read_bitu16(newsrc);
		room.unknown_r7b = read_bitu16(newsrc);

		room.room_x = read_float(newsrc);
		room.unknown_r8 = read_bitu32(newsrc);
		room.room_z = -read_float(newsrc);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator8 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator9 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator10 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator11 has wrong value", __FILE__, __LINE__, RCSID);

		temp = read_bitu32(newsrc);
		if ((temp != 0) && (temp != 0xCDCDCDCD))
			throw TR_ReadError ("read_tr5_room: seperator12 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator13 has wrong value", __FILE__, __LINE__, RCSID);

		room.num_triangles = read_bitu32(newsrc);
		if (room.num_triangles == 0xCDCDCDCD)
			room.num_triangles = 0;
		if (room.num_triangles > 512)
			throw TR_ReadError ("read_tr5_room: num_triangles > 512", __FILE__, __LINE__, RCSID);

		room.num_rectangles = read_bitu32(newsrc);
		if (room.num_rectangles == 0xCDCDCDCD)
			room.num_rectangles = 0;
		if (room.num_rectangles > 1024)
			throw TR_ReadError ("read_tr5_room: num_rectangles > 1024", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0)
			throw TR_ReadError ("read_tr5_room: seperator14 has wrong value", __FILE__, __LINE__, RCSID);

		light_size = read_bitu32(newsrc);
		if (read_bitu32(newsrc) != room.num_lights)
			throw TR_ReadError ("read_tr5_room: room.num_lights2 != room.num_lights", __FILE__, __LINE__, RCSID);

		room.unknown_r9 = read_bitu32(newsrc);
		room.room_y_top = -read_float(newsrc);
		room.room_y_bottom = -read_float(newsrc);

		room.num_layers = read_bitu32(newsrc);

		/*
		   if (room.num_layers != 0) {
		   if (room.x != room.room_x)
		   throw TR_ReadError("read_tr5_room: x != room_x");
		   if (room.z != room.room_z)
		   throw TR_ReadError("read_tr5_room: z != room_z");
		   if (room.y_top != room.room_y_top)
		   throw TR_ReadError("read_tr5_room: y_top != room_y_top");
		   if (room.y_bottom != room.room_y_bottom)
		   throw TR_ReadError("read_tr5_room: y_bottom != room_y_bottom");
		   }
		 */

		layer_offset = read_bitu32(newsrc);
		vertices_offset = read_bitu32(newsrc);
		poly_offset = read_bitu32(newsrc);
		poly_offset2 = read_bitu32(newsrc);
		if (poly_offset != poly_offset2)
			throw TR_ReadError ("read_tr5_room: poly_offset != poly_offset2", __FILE__, __LINE__, RCSID);

		vertices_size = read_bitu32(newsrc);
		if ((vertices_size % 28) != 0)
			throw TR_ReadError ("read_tr5_room: vertices_size has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator15 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator16 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator17 has wrong value", __FILE__, __LINE__, RCSID);

		if (read_bitu32(newsrc) != 0xCDCDCDCD)
			throw TR_ReadError ("read_tr5_room: seperator18 has wrong value", __FILE__, __LINE__, RCSID);

		room.lights.resize(room.num_lights);
		for (i = 0; i < room.num_lights; i++)
			read_tr5_room_light(newsrc, room.lights[i]);

		SDL_RWseek(newsrc, 208 + sector_data_offset, SEEK_SET);

		room.sector_list.resize(room.num_zsectors * room.num_xsectors);
		for (i = 0; i < (bitu32)(room.num_zsectors * room.num_xsectors); i++)
			read_tr_room_sector(newsrc, room.sector_list[i]);

		/*
		   if (room.portal_offset != 0xFFFFFFFF) {
		   if (room.portal_offset != (room.sector_data_offset + (room.num_zsectors * room.num_xsectors * 8)))
		   throw TR_ReadError("read_tr5_room: portal_offset has wrong value");

		   SDL_RWseek(newsrc, 208 + room.portal_offset, SEEK_SET);
		   }
		 */

		room.num_portals = read_bit16(newsrc);
		room.portals.resize(room.num_portals);
		for (i = 0; i < room.num_portals; i++)
			read_tr_room_portal(newsrc, room.portals[i]);

		SDL_RWseek(newsrc, 208 + static_meshes_offset, SEEK_SET);

		room.static_meshes.resize(room.num_static_meshes);
		for (i = 0; i < room.num_static_meshes; i++)
			read_tr3_room_staticmesh(newsrc, room.static_meshes[i]);

		SDL_RWseek(newsrc, 208 + layer_offset, SEEK_SET);

		room.layers.resize(room.num_layers);
		for (i = 0; i < room.num_layers; i++)
			read_tr5_room_layer(newsrc, room.layers[i]);

		SDL_RWseek(newsrc, 208 + poly_offset, SEEK_SET);

		{
			bitu32 vertex_index = 0;
			bitu32 rectangle_index = 0;
			bitu32 triangle_index = 0;

			room.rectangles.resize(room.num_rectangles);
			room.triangles.resize(room.num_triangles);
			for (i = 0; i < room.num_layers; i++) {
				bitu32 j;

				for (j = 0; j < room.layers[i].num_rectangles; j++) {
					read_tr4_face4(newsrc, room.rectangles[rectangle_index]);
					room.rectangles[rectangle_index].vertices[0] += vertex_index;
					room.rectangles[rectangle_index].vertices[1] += vertex_index;
					room.rectangles[rectangle_index].vertices[2] += vertex_index;
					room.rectangles[rectangle_index].vertices[3] += vertex_index;
					rectangle_index++;
				}
				for (j = 0; j < room.layers[i].num_triangles; j++) {
					read_tr4_face3(newsrc, room.triangles[triangle_index]);
					room.triangles[triangle_index].vertices[0] += vertex_index;
					room.triangles[triangle_index].vertices[1] += vertex_index;
					room.triangles[triangle_index].vertices[2] += vertex_index;
					triangle_index++;
				}
				vertex_index += room.layers[i].num_vertices;
			}
		}

		SDL_RWseek(newsrc, 208 + vertices_offset, SEEK_SET);

		{
			bitu32 vertex_index = 0;
			int temp1;

			room.num_vertices = vertices_size / 28;
			temp1 = room_data_size - (208 + vertices_offset + vertices_size);
			room.vertices.resize(room.num_vertices);
			for (i = 0; i < room.num_layers; i++) {
				bitu32 j;

				for (j = 0; j < room.layers[i].num_vertices; j++)
					read_tr5_room_vertex(newsrc, room.vertices[vertex_index++]);
			}
		}

		SDL_RWseek(newsrc, room_data_size, SEEK_SET);

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

void TR_Level::read_tr5_moveable(SDL_RWops * const src, tr_moveable_t & moveable)
{
	read_tr_moveable(src, moveable);
	if (read_bitu16(src) != 0xFFEF)
		throw TR_ReadError ("read_tr5_moveable: filler has wrong value", __FILE__, __LINE__, RCSID);
}

void TR_Level::read_tr5_level(SDL_RWops * const src)
{
	bitu32 i;
	bitu8 *comp_buffer = NULL;
	bitu8 *uncomp_buffer = NULL;
	SDL_RWops *newsrc = NULL;

	// Version
	bitu32 file_version = read_bitu32(src);

	if (file_version != 0x00345254)
		throw TR_ReadError ("Wrong level version", __FILE__, __LINE__, RCSID);

	this->num_textiles = 0;
	this->num_room_textiles = 0;
	this->num_obj_textiles = 0;
	this->num_bump_textiles = 0;
	this->num_misc_textiles = 0;
	this->read_32bit_textiles = false;

	try {
		bitu32 uncomp_size;
		bitu32 comp_size;
		unsigned long size;

		this->num_room_textiles = read_bitu16(src);
		this->num_obj_textiles = read_bitu16(src);
		this->num_bump_textiles = read_bitu16(src);
		this->num_misc_textiles = 3;
		this->num_textiles = this->num_room_textiles + this->num_obj_textiles + this->num_bump_textiles + this->num_misc_textiles;

		uncomp_size = read_bitu32(src);
		if (uncomp_size == 0)
			throw TR_ReadError ("read_tr5_level: textiles32 uncomp_size == 0", __FILE__, __LINE__, RCSID);

		comp_size = read_bitu32(src);
		if (comp_size > 0) {
			uncomp_buffer = new bitu8[uncomp_size];

			this->textile32.resize(this->num_textiles);
			comp_buffer = new bitu8[comp_size];

			if (SDL_RWread(src, comp_buffer, 1, comp_size) < (int)comp_size)
				throw TR_ReadError ("read_tr5_level: textiles32", __FILE__, __LINE__, RCSID);

			size = uncomp_size;
			if (uncompress(uncomp_buffer, &size, comp_buffer, comp_size) != Z_OK)
				throw TR_ReadError ("read_tr5_level: uncompress", __FILE__, __LINE__, RCSID);

			if (size != uncomp_size)
				throw TR_ReadError ("read_tr5_level: uncompress size mismatch", __FILE__, __LINE__, RCSID);
			delete [] comp_buffer;

			comp_buffer = NULL;
			if ((newsrc = SDL_RWFromMem(uncomp_buffer, uncomp_size)) == NULL)
				throw TR_ReadError ("read_tr5_level: SDL_RWFromMem", __FILE__, __LINE__, RCSID);

			for (i = 0; i < (this->num_textiles - this->num_misc_textiles); i++)
				read_tr4_textile32(newsrc, this->textile32[i]);
			SDL_RWclose(newsrc);
			newsrc = NULL;
			delete [] uncomp_buffer;

			uncomp_buffer = NULL;
			this->read_32bit_textiles = true;
		}

		uncomp_size = read_bitu32(src);
		if (uncomp_size == 0)
			throw TR_ReadError ("read_tr5_level: textiles16 uncomp_size == 0", __FILE__, __LINE__, RCSID);

		comp_size = read_bitu32(src);
		if (comp_size > 0) {
			if (this->textile32.empty()) {
				uncomp_buffer = new bitu8[uncomp_size];

				this->textile16.resize(this->num_textiles);
				comp_buffer = new bitu8[comp_size];

				if (SDL_RWread(src, comp_buffer, 1, comp_size) < (int)comp_size)
					throw TR_ReadError ("read_tr5_level: textiles16", __FILE__, __LINE__, RCSID);

				size = uncomp_size;
				if (uncompress(uncomp_buffer, &size, comp_buffer, comp_size) != Z_OK)
					throw TR_ReadError ("read_tr5_level: uncompress", __FILE__, __LINE__, RCSID);

				if (size != uncomp_size)
					throw TR_ReadError ("read_tr5_level: uncompress size mismatch", __FILE__, __LINE__, RCSID);
				delete [] comp_buffer;

				comp_buffer = NULL;
				if ((newsrc = SDL_RWFromMem(uncomp_buffer, uncomp_size)) == NULL)
					throw TR_ReadError ("read_tr5_level: SDL_RWFromMem", __FILE__, __LINE__, RCSID);

				for (i = 0; i < (this->num_textiles - this->num_misc_textiles); i++)
					read_tr2_textile16(newsrc, this->textile16[i]);
				SDL_RWclose(newsrc);
				newsrc = NULL;
				delete [] uncomp_buffer;

				uncomp_buffer = NULL;
			} else {
				SDL_RWseek(src, comp_size, SEEK_CUR);
			}
		}

		uncomp_size = read_bitu32(src);
		if (uncomp_size == 0)
			throw TR_ReadError ("read_tr5_level: textiles32d uncomp_size == 0", __FILE__, __LINE__, RCSID);

		comp_size = read_bitu32(src);
		if (comp_size > 0) {
			uncomp_buffer = new bitu8[uncomp_size];

			if ((uncomp_size / (256 * 256 * 4)) > 3)
				throw TR_ReadError ("read_tr5_level: num_misc_textiles > 3", __FILE__, __LINE__, RCSID);

			if (this->textile32.empty())
				this->textile32.resize(this->num_misc_textiles);
			comp_buffer = new bitu8[comp_size];

			if (SDL_RWread(src, comp_buffer, 1, comp_size) < (int)comp_size)
				throw TR_ReadError ("read_tr5_level: misc_textiles", __FILE__, __LINE__, RCSID);

			size = uncomp_size;
			if (uncompress(uncomp_buffer, &size, comp_buffer, comp_size) != Z_OK)
				throw TR_ReadError ("read_tr5_level: uncompress", __FILE__, __LINE__, RCSID);

			if (size != uncomp_size)
				throw TR_ReadError ("read_tr5_level: uncompress size mismatch", __FILE__, __LINE__, RCSID);
			delete [] comp_buffer;

			comp_buffer = NULL;
			if ((newsrc = SDL_RWFromMem(uncomp_buffer, uncomp_size)) == NULL)
				throw TR_ReadError ("read_tr5_level: SDL_RWFromMem", __FILE__, __LINE__, RCSID);

			for (i = (this->num_textiles - this->num_misc_textiles); i < this->num_textiles; i++)
				read_tr4_textile32(newsrc, this->textile32[i]);
			SDL_RWclose(newsrc);
			newsrc = NULL;
			delete [] uncomp_buffer;

			uncomp_buffer = NULL;
		}
	}
	catch(...) {
		if (newsrc)
			SDL_RWclose(newsrc);
		delete [] uncomp_buffer;
		delete [] comp_buffer;

		throw;
	}

	// flags?
	/*
	   I found 2 flags in the TR5 file format. Directly after the sprite textures are 2 ints as a flag. The first one is the lara type:
	   0 Normal
	   3 Catsuit
	   4 Divesuit
	   6 Invisible

	   The second one is the weather type (effects all outside rooms):
	   0 No weather
	   1 Rain
	   2 Snow (in title.trc these are red triangles falling from the sky).
	 */
	i = read_bitu16(src);
	i = read_bitu16(src);
	if (read_bitu32(src) != 0)
		throw TR_ReadError ("Bad value for flags[1]", __FILE__, __LINE__, RCSID);

	if (read_bitu32(src) != 0)
		throw TR_ReadError ("Bad value for flags[2]", __FILE__, __LINE__, RCSID);

	if (read_bitu32(src) != 0)
		throw TR_ReadError ("Bad value for flags[3]", __FILE__, __LINE__, RCSID);

	if (read_bitu32(src) != 0)
		throw TR_ReadError ("Bad value for flags[4]", __FILE__, __LINE__, RCSID);

	if (read_bitu32(src) != 0)
		throw TR_ReadError ("Bad value for flags[5]", __FILE__, __LINE__, RCSID);

	if (read_bitu32(src) != 0)
		throw TR_ReadError ("Bad value for flags[6]", __FILE__, __LINE__, RCSID);

	if (read_bitu32(src) != 0)
		throw TR_ReadError ("Bad value for flags[7]", __FILE__, __LINE__, RCSID);

	// LevelDataSize1
	read_bitu32(src);
	// LevelDataSize2
	read_bitu32(src);

	// Unused
	if (read_bitu32(src) != 0)
		throw TR_ReadError ("Bad value for 'unused'", __FILE__, __LINE__, RCSID);

	this->rooms.resize(read_bitu32(src));
	for (i = 0; i < this->rooms.size(); i++)
		read_tr5_room(src, this->rooms[i]);

	this->floor_data.resize(read_bitu32(src));
	SDL_RWseek(src, this->floor_data.size() * 2, SEEK_CUR);

	read_mesh_data(src);

	this->animations.resize(read_bitu32(src));
	for (i = 0; i < this->animations.size(); i++)
		read_tr_animation(src, this->animations[i]);

	this->state_changes.resize(read_bitu32(src));
	SDL_RWseek(src, this->state_changes.size() * 6, SEEK_CUR);

	this->anim_dispatches.resize(read_bitu32(src));
	SDL_RWseek(src, this->anim_dispatches.size() * 8, SEEK_CUR);

	this->anim_commands.resize(read_bitu32(src));
	SDL_RWseek(src, this->anim_commands.size() * 2, SEEK_CUR);

	bitu32 num_mesh_trees = read_bitu32(src);

	if ((num_mesh_trees % 4) != 0)
		throw TR_ReadError ("read_tr5_level: num_mesh_trees % 4 != 0", __FILE__, __LINE__, RCSID);

	num_mesh_trees /= 4;
	this->mesh_trees.resize(num_mesh_trees);
	for (i = 0; i < this->mesh_trees.size(); i++)
		read_tr_meshtree(src, this->mesh_trees[i]);

	read_frame_moveable_data(src);

	this->static_meshes.resize(read_bitu32(src));
	for (i = 0; i < this->static_meshes.size(); i++)
		read_tr_staticmesh(src, this->static_meshes[i]);

	if (read_bit8(src) != 'S')
		throw TR_ReadError ("read_tr5_level: 'SPR' not found", __FILE__, __LINE__, RCSID);

	if (read_bit8(src) != 'P')
		throw TR_ReadError ("read_tr5_level: 'SPR' not found", __FILE__, __LINE__, RCSID);

	if (read_bit8(src) != 'R')
		throw TR_ReadError ("read_tr5_level: 'SPR' not found", __FILE__, __LINE__, RCSID);

	if (read_bit8(src) != 0)
		throw TR_ReadError ("read_tr5_level: 'SPR' not found", __FILE__, __LINE__, RCSID);

	this->sprite_textures.resize(read_bitu32(src));
	for (i = 0; i < this->sprite_textures.size(); i++)
		read_tr_sprite_texture(src, this->sprite_textures[i]);

	this->sprite_sequences.resize(read_bitu32(src));
	for (i = 0; i < this->sprite_sequences.size(); i++)
		read_tr_sprite_sequence(src, this->sprite_sequences[i]);

	this->cameras.resize(read_bitu32(src));
	SDL_RWseek(src, this->cameras.size() * 16, SEEK_CUR);

	this->flyby_cameras.resize(read_bitu32(src));
	SDL_RWseek(src, this->flyby_cameras.size() * 40, SEEK_CUR);

	this->sound_sources.resize(read_bitu32(src));
	SDL_RWseek(src, this->sound_sources.size() * 16, SEEK_CUR);

	this->boxes.resize(read_bitu32(src));
	SDL_RWseek(src, this->boxes.size() * 8, SEEK_CUR);

	this->overlaps.resize(read_bitu32(src));
	SDL_RWseek(src, this->overlaps.size() * 2, SEEK_CUR);

	// Zones
	SDL_RWseek(src, this->boxes.size() * 20, SEEK_CUR);

	this->animated_textures.resize(read_bitu32(src));
	SDL_RWseek(src, this->animated_textures.size() * 2, SEEK_CUR);

	int unknown = read_bit8(src);

	if ((unknown != 0) && (unknown != 1) && (unknown != 2) && (unknown != 3) && (unknown != 4))
		throw TR_ReadError ("read_tr5_level: unknown before TEX has bad value", __FILE__, __LINE__, RCSID);

	if (read_bit8(src) != 'T')
		throw TR_ReadError ("read_tr5_level: '\\0TEX' not found", __FILE__, __LINE__, RCSID);

	if (read_bit8(src) != 'E')
		throw TR_ReadError ("read_tr5_level: '\\0TEX' not found", __FILE__, __LINE__, RCSID);

	if (read_bit8(src) != 'X')
		throw TR_ReadError ("read_tr5_level: '\\0TEX' not found", __FILE__, __LINE__, RCSID);

	if (read_bit8(src) != 0)
		throw TR_ReadError ("read_tr5_level: '\\0TEX' not found", __FILE__, __LINE__, RCSID);

	this->object_textures.resize(read_bitu32(src));
	for (i = 0; i < this->object_textures.size(); i++) {
		read_tr4_object_texture(src, this->object_textures[i]);
		if (read_bitu16(src) != 0)
			throw TR_ReadError ("read_tr5_level: obj_tex trailing bitu16 != 0", __FILE__, __LINE__, RCSID);
	}

	this->items.resize(read_bitu32(src));
	for (i = 0; i < this->items.size(); i++)
		read_tr3_item(src, this->items[i]);

	this->ai_objects.resize(read_bitu32(src));
	SDL_RWseek(src, this->ai_objects.size() * 24, SEEK_CUR);

	this->demo_data.resize(read_bitu16(src));
	SDL_RWseek(src, this->demo_data.size(), SEEK_CUR);

	// Soundmap
	SDL_RWseek(src, 2 * 450, SEEK_CUR);

	this->sound_details.resize(read_bitu32(src));
	SDL_RWseek(src, this->sound_details.size() * 8, SEEK_CUR);

	this->sample_indices.resize(read_bitu32(src));
	SDL_RWseek(src, this->sample_indices.size() * 4, SEEK_CUR);
}
