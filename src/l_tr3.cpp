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

#define RCSID "$Id: l_tr3.cpp,v 1.15 2002/09/20 15:59:02 crow Exp $"

void TR_Level::read_tr3_room_vertex(SDL_RWops * const src, tr5_room_vertex_t & room_vertex)
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
	room_vertex.colour.r = room_vertex.lighting1 / 32767.0f;
	room_vertex.colour.g = room_vertex.lighting1 / 32767.0f;
	room_vertex.colour.b = room_vertex.lighting1 / 32767.0f;
	room_vertex.colour.a = 255;
}

void TR_Level::read_tr3_room_staticmesh(SDL_RWops * const src, tr2_room_staticmesh_t & room_static_mesh)
{
	read_tr_vertex32(src, room_static_mesh.pos);
	room_static_mesh.rotation = (float)read_bitu16(src) / 16384.0f * -90;
	room_static_mesh.intensity1 = read_bit16(src);
	room_static_mesh.intensity2 = read_bit16(src);
	room_static_mesh.object_id = read_bitu16(src);
}

void TR_Level::read_tr3_room(SDL_RWops * const src, tr5_room_t & room)
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
		read_tr3_room_vertex(src, room.vertices[i]);

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
		read_tr2_room_light(src, room.lights[i]);

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

void TR_Level::read_tr3_item(SDL_RWops * const src, tr2_item_t & item)
{
	item.object_id = read_bit16(src);
	item.room = read_bit16(src);
	read_tr_vertex32(src, item.pos);
	item.rotation = (float)read_bitu16(src) / 16384.0f * -90;
	item.intensity1 = read_bitu16(src);
	item.intensity2 = read_bitu16(src);
	item.flags = read_bitu16(src);
}

void TR_Level::read_tr3_level(SDL_RWops * const src)
{
	bitu32 i;

	// Version
	bitu32 file_version = read_bitu32(src);

	if ((file_version != 0xFF080038) && (file_version != 0xFF180038) /*&& (file_version != 0xFF180034) */ )
		throw TR_ReadError ("Wrong level version", __FILE__, __LINE__, RCSID);

	read_tr_palette(src, this->palette);
	read_tr2_palette16(src, this->palette16);

	this->num_textiles = 0;
	this->num_room_textiles = 0;
	this->num_obj_textiles = 0;
	this->num_bump_textiles = 0;
	this->num_misc_textiles = 0;
	this->read_32bit_textiles = false;

	this->num_textiles = read_bitu32(src);
	this->textile8.resize(this->num_textiles);
	for (i = 0; i < this->num_textiles; i++)
		read_tr_textile8(src, this->textile8[i]);
	this->textile16.resize(this->num_textiles);
	for (i = 0; i < this->num_textiles; i++)
		read_tr2_textile16(src, this->textile16[i]);

	// Unused
	if (read_bitu32(src) != 0)
		throw TR_ReadError ("Bad value for 'unused'", __FILE__, __LINE__, RCSID);

	this->rooms.resize(read_bitu16(src));
	for (i = 0; i < this->rooms.size(); i++)
		read_tr3_room(src, this->rooms[i]);

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
		throw TR_ReadError ("read_tr3_level: num_mesh_trees % 4 != 0", __FILE__, __LINE__, RCSID);

	num_mesh_trees /= 4;
	this->mesh_trees.resize(num_mesh_trees);
	for (i = 0; i < this->mesh_trees.size(); i++)
		read_tr_meshtree(src, this->mesh_trees[i]);

	read_frame_moveable_data(src);

	this->static_meshes.resize(read_bitu32(src));
	for (i = 0; i < this->static_meshes.size(); i++)
		read_tr_staticmesh(src, this->static_meshes[i]);

	this->sprite_textures.resize(read_bitu32(src));
	for (i = 0; i < this->sprite_textures.size(); i++)
		read_tr_sprite_texture(src, this->sprite_textures[i]);

	this->sprite_sequences.resize(read_bitu32(src));
	for (i = 0; i < this->sprite_sequences.size(); i++)
		read_tr_sprite_sequence(src, this->sprite_sequences[i]);

	this->cameras.resize(read_bitu32(src));
	SDL_RWseek(src, this->cameras.size() * 16, SEEK_CUR);

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

	this->object_textures.resize(read_bitu32(src));
	for (i = 0; i < this->object_textures.size(); i++)
		read_tr_object_texture(src, this->object_textures[i]);

	this->items.resize(read_bitu32(src));
	for (i = 0; i < this->items.size(); i++)
		read_tr3_item(src, this->items[i]);

	read_tr_lightmap(src, this->lightmap);

	this->cinematic_frames.resize(read_bitu16(src));
	SDL_RWseek(src, this->cinematic_frames.size() * 16, SEEK_CUR);

	this->demo_data.resize(read_bitu16(src));
	SDL_RWseek(src, this->demo_data.size(), SEEK_CUR);

	// Soundmap
	SDL_RWseek(src, 2 * 370, SEEK_CUR);

	this->sound_details.resize(read_bitu32(src));
	SDL_RWseek(src, this->sound_details.size() * 8, SEEK_CUR);

	this->sample_indices.resize(read_bitu32(src));
	SDL_RWseek(src, this->sample_indices.size() * 4, SEEK_CUR);
}
