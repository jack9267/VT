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
#include "SDL_endian.h"
#include "l_main.h"
#include "zlib.h"

#define RCSID "$Id: l_tr4.cpp,v 1.14 2002/09/20 15:59:02 crow Exp $"

void TR_Level::read_tr4_vertex_float(SDL_RWops * const src, tr5_vertex_t & vertex)
{
	vertex.x = read_float(src);
	vertex.y = -read_float(src);
	vertex.z = -read_float(src);
}

void TR_Level::read_tr4_textile32(SDL_RWops * const src, tr4_textile32_t & textile)
{
	for (int i = 0; i < 256; i++) {
		if (SDL_RWread(src, textile.pixels[i], 4, 256) < 256)
			throw TR_ReadError ("read_tr4_textile32", __FILE__, __LINE__, RCSID);

		for (int j = 0; j < 256; j++)
			textile.pixels[i][j] = SDL_SwapLE32(textile.pixels[i][j] & 0xff00ff00 | ((textile.pixels[i][j] & 0x00ff0000) >> 16) | ((textile.pixels[i][j] & 0x000000ff) << 16));
	}
}

void TR_Level::read_tr4_face3(SDL_RWops * const src, tr4_face3_t & meshface)
{
	meshface.vertices[0] = read_bitu16(src);
	meshface.vertices[1] = read_bitu16(src);
	meshface.vertices[2] = read_bitu16(src);
	meshface.texture = read_bitu16(src);
	meshface.lighting = read_bitu16(src);
}

void TR_Level::read_tr4_face4(SDL_RWops * const src, tr4_face4_t & meshface)
{
	meshface.vertices[0] = read_bitu16(src);
	meshface.vertices[1] = read_bitu16(src);
	meshface.vertices[2] = read_bitu16(src);
	meshface.vertices[3] = read_bitu16(src);
	meshface.texture = read_bitu16(src);
	meshface.lighting = read_bitu16(src);
}

void TR_Level::read_tr4_room_light(SDL_RWops * const src, tr5_room_light_t & light)
{
	read_tr_vertex32(src, light.pos);
	read_tr_colour(src, light.color);
	light.light_type = read_bitu8(src);
	light.unknown = read_bitu8(src);
	light.intensity1 = read_bitu8(src);
	light.r_inner = read_float(src);
	light.r_outer = read_float(src);
	light.length = read_float(src);
	light.cutoff = read_float(src);
	read_tr4_vertex_float(src, light.dir);
}

void TR_Level::read_tr4_room_vertex(SDL_RWops * const src, tr5_room_vertex_t & room_vertex)
{
	read_tr_vertex16(src, room_vertex.vertex);
	// read and make consistent
	room_vertex.lighting1 = read_bit16(src);
	room_vertex.attributes = read_bitu16(src);
	room_vertex.lighting2 = read_bit16(src);
	// only in TR5
	room_vertex.normal.x = 0;
	room_vertex.normal.y = 0;
	room_vertex.normal.z = 0;
	room_vertex.colour.r = room_vertex.lighting1 / 16383.0f;
	room_vertex.colour.g = room_vertex.lighting1 / 16383.0f;
	room_vertex.colour.b = room_vertex.lighting1 / 16383.0f;
	room_vertex.colour.a = 255;
}

void TR_Level::read_tr4_room(SDL_RWops * const src, tr5_room_t & room)
{
	bitu32 num_data_words;
	bitu32 i;
	int pos;

	// read and change coordinate system
	room.offset.x = (float)read_bit32(src);
	room.offset.y = 0;
	room.offset.z = (float)-read_bit32(src);
	room.y_bottom = (float)-read_bit32(src);
	room.y_top = (float)-read_bit32(src);

	num_data_words = read_bitu32(src);

	pos = SDL_RWseek(src, 0, SEEK_CUR);

	room.num_layers = 0;

	room.num_vertices = read_bitu16(src);
	room.vertices.resize(room.num_vertices);
	for (i = 0; i < room.num_vertices; i++)
		read_tr4_room_vertex(src, room.vertices[i]);

	room.num_rectangles = read_bitu16(src);
	room.rectangles.resize(room.num_rectangles);
	for (i = 0; i < room.num_rectangles; i++)
		read_tr_face4(src, room.rectangles[i]);

	room.num_triangles = read_bitu16(src);
	room.triangles.resize(room.num_triangles);
	for (i = 0; i < room.num_triangles; i++)
		read_tr_face3(src, room.triangles[i]);

	room.num_sprites = read_bitu16(src);
	room.sprites.resize(room.num_sprites);
	for (i = 0; i < room.num_sprites; i++)
		read_tr_room_sprite(src, room.sprites[i]);

	// set to the right position in case that there is some unused data
	SDL_RWseek(src, pos + (num_data_words * 2), SEEK_SET);

	room.num_portals = read_bitu16(src);
	room.portals.resize(room.num_portals);
	for (i = 0; i < room.num_portals; i++)
		read_tr_room_portal(src, room.portals[i]);

	room.num_zsectors = read_bitu16(src);
	room.num_xsectors = read_bitu16(src);
	room.sector_list.resize(room.num_zsectors * room.num_xsectors);
	for (i = 0; i < (bitu32)(room.num_zsectors * room.num_xsectors); i++)
		read_tr_room_sector(src, room.sector_list[i]);

	room.intensity1 = read_bit16(src);
	room.intensity2 = read_bit16(src);

	// only in TR2
	room.light_mode = 0;

	room.num_lights = read_bitu16(src);
	room.lights.resize(room.num_lights);
	for (i = 0; i < room.num_lights; i++)
		read_tr4_room_light(src, room.lights[i]);

	room.num_static_meshes = read_bitu16(src);
	room.static_meshes.resize(room.num_static_meshes);
	for (i = 0; i < room.num_static_meshes; i++)
		read_tr3_room_staticmesh(src, room.static_meshes[i]);

	room.alternate_room = read_bit16(src);
	room.flags = read_bitu16(src);

	// only in TR3-TR4
	read_tr_colour(src, room.fog_colour);
	room.light_colour.r = room.intensity1 / 32767.0f;
	room.light_colour.g = room.intensity1 / 32767.0f;
	room.light_colour.b = room.intensity1 / 32767.0f;
	room.light_colour.a = 1.0f;
}

void TR_Level::read_tr4_object_texture_vert(SDL_RWops * const src, tr4_object_texture_vert_t & vert)
{
	vert.xcoordinate = read_bit8(src);
	vert.xpixel = read_bitu8(src);
	vert.ycoordinate = read_bit8(src);
	vert.ypixel = read_bitu8(src);
	if (vert.xcoordinate == 0)
		vert.xcoordinate = 1;
	if (vert.ycoordinate == 0)
		vert.ycoordinate = 1;
}

void TR_Level::read_tr4_object_texture(SDL_RWops * const src, tr4_object_texture_t & object_texture)
{
	object_texture.transparency_flags = read_bitu16(src);
	object_texture.tile = read_bitu8(src);
	if (object_texture.tile > 64)
		throw TR_ReadError ("object_texture.tile > 64", __FILE__, __LINE__, RCSID);

	object_texture.tile_flags = read_bitu8(src);
	if ((object_texture.tile_flags != 0) && (object_texture.tile_flags != 128))
		throw TR_ReadError ("object_texture.tile_flags != 0 && 128", __FILE__, __LINE__, RCSID);

	object_texture.flags = read_bitu16(src);
	read_tr4_object_texture_vert(src, object_texture.vertices[0]);
	read_tr4_object_texture_vert(src, object_texture.vertices[1]);
	read_tr4_object_texture_vert(src, object_texture.vertices[2]);
	read_tr4_object_texture_vert(src, object_texture.vertices[3]);
	object_texture.unknown1 = read_bitu32(src);
	object_texture.unknown2 = read_bitu32(src);
	object_texture.x_size = read_bitu32(src);
	object_texture.y_size = read_bitu32(src);
}

void TR_Level::read_tr4_mesh(SDL_RWops * const src, tr4_mesh_t & mesh)
{
	int i;

	read_tr_vertex16(src, mesh.centre);
	mesh.collision_size = read_bit32(src);

	mesh.num_vertices = read_bit16(src);
	mesh.vertices.resize(mesh.num_vertices);
	for (i = 0; i < mesh.num_vertices; i++)
		read_tr_vertex16(src, mesh.vertices[i]);

	mesh.num_normals = read_bit16(src);
	if (mesh.num_normals >= 0) {
		mesh.num_lights = 0;
		mesh.normals.resize(mesh.num_normals);
		for (i = 0; i < mesh.num_normals; i++)
			read_tr_vertex16(src, mesh.normals[i]);
	} else {
		mesh.num_lights = -mesh.num_normals;
		mesh.num_normals = 0;
		mesh.lights.resize(mesh.num_lights);
		for (i = 0; i < mesh.num_lights; i++)
			mesh.lights[i] = read_bit16(src);
	}

	mesh.num_textured_rectangles = read_bit16(src);
	mesh.textured_rectangles.resize(mesh.num_textured_rectangles);
	for (i = 0; i < mesh.num_textured_rectangles; i++)
		read_tr4_face4(src, mesh.textured_rectangles[i]);

	mesh.num_textured_triangles = read_bit16(src);
	mesh.textured_triangles.resize(mesh.num_textured_triangles);
	for (i = 0; i < mesh.num_textured_triangles; i++)
		read_tr4_face3(src, mesh.textured_triangles[i]);

	mesh.num_coloured_rectangles = 0;
	mesh.num_coloured_triangles = 0;
}

/// \brief reads an animation definition.
void TR_Level::read_tr4_animation(SDL_RWops * const src, tr_animation_t & animation)
{
	animation.frame_offset = read_bitu32(src);
	animation.frame_rate = read_bitu8(src);
	animation.frame_size = read_bitu8(src);
	animation.state_id = read_bitu16(src);

	animation.unknown = read_bit16(src);
	animation.speed = read_bit16(src);
	animation.accel_lo = read_bit16(src);
	animation.accel_hi = read_bit16(src);

	animation.unknown2 = read_bit16(src);
	animation.speed2 = read_bit16(src);
	animation.accel_lo2 = read_bit16(src);
	animation.accel_hi2 = read_bit16(src);

	animation.frame_start = read_bitu16(src);
	animation.frame_end = read_bitu16(src);
	animation.next_animation = read_bitu16(src);
	animation.next_frame = read_bitu16(src);

	animation.num_state_changes = read_bitu16(src);
	animation.state_change_offset = read_bitu16(src);
	animation.num_anim_commands = read_bitu16(src);
	animation.anim_command = read_bitu16(src);
}

void TR_Level::read_tr4_level(SDL_RWops * const _src)
{
	SDL_RWops *src = _src;
	bitu32 i;
	bitu8 *uncomp_buffer = NULL;
	bitu8 *comp_buffer = NULL;
	SDL_RWops *newsrc = NULL;

	try {
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

		{
			bitu32 uncomp_size;
			bitu32 comp_size;
			unsigned long size;

			this->num_room_textiles = read_bitu16(src);
			this->num_obj_textiles = read_bitu16(src);
			this->num_bump_textiles = read_bitu16(src);
			this->num_misc_textiles = 2;
			this->num_textiles = this->num_room_textiles + this->num_obj_textiles + this->num_bump_textiles + this->num_misc_textiles;

			uncomp_size = read_bitu32(src);
			if (uncomp_size == 0)
				throw TR_ReadError ("read_tr4_level: textiles32 uncomp_size == 0", __FILE__, __LINE__, RCSID);

			comp_size = read_bitu32(src);
			if (comp_size > 0) {
				uncomp_buffer = new bitu8[uncomp_size];

				this->textile32.resize(this->num_textiles);
				comp_buffer = new bitu8[comp_size];

				if (SDL_RWread(src, comp_buffer, 1, comp_size) < (int)comp_size)
					throw TR_ReadError ("read_tr4_level: textiles32", __FILE__, __LINE__, RCSID);

				size = uncomp_size;
				if (uncompress(uncomp_buffer, &size, comp_buffer, comp_size) != Z_OK)
					throw TR_ReadError ("read_tr4_level: uncompress", __FILE__, __LINE__, RCSID);

				if (size != uncomp_size)
					throw TR_ReadError ("read_tr4_level: uncompress size mismatch", __FILE__, __LINE__, RCSID);
				delete [] comp_buffer;

				comp_buffer = NULL;
				if ((newsrc = SDL_RWFromMem(uncomp_buffer, uncomp_size)) == NULL)
					throw TR_ReadError ("read_tr4_level: SDL_RWFromMem", __FILE__, __LINE__, RCSID);

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
				throw TR_ReadError ("read_tr4_level: textiles16 uncomp_size == 0", __FILE__, __LINE__, RCSID);

			comp_size = read_bitu32(src);
			if (comp_size > 0) {
				if (this->textile32.empty()) {
					uncomp_buffer = new bitu8[uncomp_size];

					this->textile16.resize(this->num_textiles);
					comp_buffer = new bitu8[comp_size];

					if (SDL_RWread(src, comp_buffer, 1, comp_size) < (int)comp_size)
						throw TR_ReadError ("read_tr4_level: textiles16", __FILE__, __LINE__, RCSID);

					size = uncomp_size;
					if (uncompress(uncomp_buffer, &size, comp_buffer, comp_size) != Z_OK)
						throw TR_ReadError ("read_tr4_level: uncompress", __FILE__, __LINE__, RCSID);

					if (size != uncomp_size)
						throw TR_ReadError ("read_tr4_level: uncompress size mismatch", __FILE__, __LINE__, RCSID);
					delete [] comp_buffer;

					comp_buffer = NULL;
					if ((newsrc = SDL_RWFromMem(uncomp_buffer, uncomp_size)) == NULL)
						throw TR_ReadError ("read_tr4_level: SDL_RWFromMem", __FILE__, __LINE__, RCSID);

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
				throw TR_ReadError ("read_tr4_level: textiles32d uncomp_size == 0", __FILE__, __LINE__, RCSID);

			comp_size = read_bitu32(src);
			if (comp_size > 0) {
				uncomp_buffer = new bitu8[uncomp_size];

				if ((uncomp_size / (256 * 256 * 4)) > 2)
					throw TR_ReadError ("read_tr4_level: num_misc_textiles > 2", __FILE__, __LINE__, RCSID);

				if (this->textile32.empty())
					this->textile32.resize(this->num_textiles);
				comp_buffer = new bitu8[comp_size];

				if (SDL_RWread(src, comp_buffer, 1, comp_size) < (int)comp_size)
					throw TR_ReadError ("read_tr4_level: misc_textiles", __FILE__, __LINE__, RCSID);

				size = uncomp_size;
				if (uncompress(uncomp_buffer, &size, comp_buffer, comp_size) != Z_OK)
					throw TR_ReadError ("read_tr4_level: uncompress", __FILE__, __LINE__, RCSID);

				if (size != uncomp_size)
					throw TR_ReadError ("read_tr4_level: uncompress size mismatch", __FILE__, __LINE__, RCSID);
				delete [] comp_buffer;

				comp_buffer = NULL;
				if ((newsrc = SDL_RWFromMem(uncomp_buffer, uncomp_size)) == NULL)
					throw TR_ReadError ("read_tr4_level: SDL_RWFromMem", __FILE__, __LINE__, RCSID);

				for (i = (this->num_textiles - this->num_misc_textiles); i < this->num_textiles; i++)
					read_tr4_textile32(newsrc, this->textile32[i]);
				SDL_RWclose(newsrc);
				newsrc = NULL;
				delete [] uncomp_buffer;

				uncomp_buffer = NULL;
			}

			uncomp_size = read_bitu32(src);
			comp_size = read_bitu32(src);
			if (!comp_size)
				throw TR_ReadError ("read_tr4_level: packed geometry", __FILE__, __LINE__, RCSID);

			uncomp_buffer = new bitu8[uncomp_size];
			comp_buffer = new bitu8[comp_size];

			if (SDL_RWread(src, comp_buffer, 1, comp_size) < (int)comp_size)
				throw TR_ReadError ("read_tr4_level: packed geometry", __FILE__, __LINE__, RCSID);

			size = uncomp_size;
			if (uncompress(uncomp_buffer, &size, comp_buffer, comp_size) != Z_OK)
				throw TR_ReadError ("read_tr4_level: uncompress", __FILE__, __LINE__, RCSID);

			if (size != uncomp_size)
				throw TR_ReadError ("read_tr4_level: uncompress size mismatch", __FILE__, __LINE__, RCSID);
			delete [] comp_buffer;

			comp_buffer = NULL;
			if ((src = SDL_RWFromMem(uncomp_buffer, uncomp_size)) == NULL)
				throw TR_ReadError ("read_tr4_level: SDL_RWFromMem", __FILE__, __LINE__, RCSID);
		}

		// Unused
		if (read_bitu32(src) != 0)
			throw TR_ReadError ("Bad value for 'unused'", __FILE__, __LINE__, RCSID);

		this->rooms.resize(read_bitu16(src));
		for (i = 0; i < this->rooms.size(); i++)
			read_tr4_room(src, this->rooms[i]);

		this->floor_data.resize(read_bitu32(src));
		SDL_RWseek(src, this->floor_data.size() * 2, SEEK_CUR);

		read_mesh_data(src);

		this->animations.resize(read_bitu32(src));
		for (i = 0; i < this->animations.size(); i++)
			read_tr4_animation(src, this->animations[i]);

		this->state_changes.resize(read_bitu32(src));
		SDL_RWseek(src, this->state_changes.size() * 6, SEEK_CUR);

		this->anim_dispatches.resize(read_bitu32(src));
		SDL_RWseek(src, this->anim_dispatches.size() * 8, SEEK_CUR);

		this->anim_commands.resize(read_bitu32(src));
		SDL_RWseek(src, this->anim_commands.size() * 2, SEEK_CUR);

		bitu32 num_mesh_trees = read_bitu32(src);

		if ((num_mesh_trees % 4) != 0)
			throw TR_ReadError ("read_tr4_level: num_mesh_trees % 4 != 0", __FILE__, __LINE__, RCSID);

		num_mesh_trees /= 4;
		this->mesh_trees.resize(num_mesh_trees);
		for (i = 0; i < this->mesh_trees.size(); i++)
			read_tr_meshtree(src, this->mesh_trees[i]);

		read_frame_moveable_data(src);

		this->static_meshes.resize(read_bitu32(src));
		for (i = 0; i < this->static_meshes.size(); i++)
			read_tr_staticmesh(src, this->static_meshes[i]);

		if (read_bit8(src) != 'S')
			throw TR_ReadError ("read_tr4_level: 'SPR' not found", __FILE__, __LINE__, RCSID);

		if (read_bit8(src) != 'P')
			throw TR_ReadError ("read_tr4_level: 'SPR' not found", __FILE__, __LINE__, RCSID);

		if (read_bit8(src) != 'R')
			throw TR_ReadError ("read_tr4_level: 'SPR' not found", __FILE__, __LINE__, RCSID);

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

		if ((unknown != 0) && (unknown != 1) && (unknown != 2) && (unknown != 4))
			throw TR_ReadError ("read_tr4_level: unknown before TEX has bad value", __FILE__, __LINE__, RCSID);

		if (read_bit8(src) != 'T')
			throw TR_ReadError ("read_tr4_level: '\\0TEX' not found", __FILE__, __LINE__, RCSID);

		if (read_bit8(src) != 'E')
			throw TR_ReadError ("read_tr4_level: '\\0TEX' not found", __FILE__, __LINE__, RCSID);

		if (read_bit8(src) != 'X')
			throw TR_ReadError ("read_tr4_level: '\\0TEX' not found", __FILE__, __LINE__, RCSID);

		this->object_textures.resize(read_bitu32(src));
		for (i = 0; i < this->object_textures.size(); i++)
			read_tr4_object_texture(src, this->object_textures[i]);

		this->items.resize(read_bitu32(src));
		for (i = 0; i < this->items.size(); i++)
			read_tr3_item(src, this->items[i]);

		this->ai_objects.resize(read_bitu32(src));
		SDL_RWseek(src, this->ai_objects.size() * 24, SEEK_CUR);

		this->demo_data.resize(read_bitu16(src));
		SDL_RWseek(src, this->demo_data.size(), SEEK_CUR);

		// Soundmap
		SDL_RWseek(src, 2 * 370, SEEK_CUR);

		this->sound_details.resize(read_bitu32(src));
		SDL_RWseek(src, this->sound_details.size() * 8, SEEK_CUR);

		this->sample_indices.resize(read_bitu32(src));
		SDL_RWseek(src, this->sample_indices.size() * 4, SEEK_CUR);

		bitu16 temp;

		temp = read_bitu16(src);
		if ((temp != 0) && (temp != 0xCDCD))
			throw TR_ReadError ("read_tr4_level: filler1 has wrong value", __FILE__, __LINE__, RCSID);

		temp = read_bitu16(src);
		if ((temp != 0) && (temp != 0xCDCD))
			throw TR_ReadError ("read_tr4_level: filler2 has wrong value", __FILE__, __LINE__, RCSID);

		temp = read_bitu16(src);
		if ((temp != 0) && (temp != 0xCDCD))
			throw TR_ReadError ("read_tr4_level: filler3 has wrong value", __FILE__, __LINE__, RCSID);

		SDL_RWclose(src);
		delete [] uncomp_buffer;

		uncomp_buffer = NULL;
	}
	catch(...) {
		if (newsrc)
			SDL_RWclose(newsrc);
		SDL_RWclose(src);
		delete [] uncomp_buffer;
		delete [] comp_buffer;

		throw;
	}
}
