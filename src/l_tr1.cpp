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

#define RCSID "$Id: l_tr1.cpp,v 1.16 2002/09/20 15:59:02 crow Exp $"

/** \brief reads rgb colour.
  *
  * Reads three rgb colour components. The read 6-bit values get shifted, so they are 8-bit.
  * The alpha value of tr2_colour_t gets set to 0.
  */
void TR_Level::read_tr_colour(SDL_RWops * const src, tr2_colour_t & colour)
{
	// read 6 bit color and change to 8 bit
	colour.r = read_bitu8(src) << 2;
	colour.g = read_bitu8(src) << 2;
	colour.b = read_bitu8(src) << 2;
	colour.a = 0;
}

/** \brief reads three 16-bit vertex components.
  *
  * The values get converted from bit16 to float. y and z are negated to fit OpenGLs coordinate system.
  */
void TR_Level::read_tr_vertex16(SDL_RWops * const src, tr5_vertex_t & vertex)
{
	// read vertex and change coordinate system
	vertex.x = (float)read_bit16(src);
	vertex.y = (float)-read_bit16(src);
	vertex.z = (float)-read_bit16(src);
}

/** \brief reads three 32-bit vertex components.
  *
  * The values get converted from bit32 to float. y and z are negated to fit OpenGLs coordinate system.
  */
void TR_Level::read_tr_vertex32(SDL_RWops * const src, tr5_vertex_t & vertex)
{
	// read vertex and change coordinate system
	vertex.x = (float)read_bit32(src);
	vertex.y = (float)-read_bit32(src);
	vertex.z = (float)-read_bit32(src);
}

/** \brief reads a triangle definition.
  *
  * The lighting value is set to 0, as it is only in TR4-5.
  */
void TR_Level::read_tr_face3(SDL_RWops * const src, tr4_face3_t & meshface)
{
	meshface.vertices[0] = read_bitu16(src);
	meshface.vertices[1] = read_bitu16(src);
	meshface.vertices[2] = read_bitu16(src);
	meshface.texture = read_bitu16(src);
	// lighting only in TR4-5
	meshface.lighting = 0;
}

/** \brief reads a triangle definition.
  *
  * The lighting value is set to 0, as it is only in TR4-5.
  */
void TR_Level::read_tr_face4(SDL_RWops * const src, tr4_face4_t & meshface)
{
	meshface.vertices[0] = read_bitu16(src);
	meshface.vertices[1] = read_bitu16(src);
	meshface.vertices[2] = read_bitu16(src);
	meshface.vertices[3] = read_bitu16(src);
	meshface.texture = read_bitu16(src);
	// only in TR4-TR5
	meshface.lighting = 0;
}

/// \brief reads a 8-bit 256x256 textile.
void TR_Level::read_tr_textile8(SDL_RWops * const src, tr_textile8_t & textile)
{
	for (int i = 0; i < 256; i++)
		if (SDL_RWread(src, textile.pixels[i], 1, 256) < 256)
			throw TR_ReadError ("read_tr_textile8", __FILE__, __LINE__, RCSID);
}

/// \brief reads the lightmap.
void TR_Level::read_tr_lightmap(SDL_RWops * const src, tr_lightmap_t & lightmap)
{
	for (int i = 0; i < (32 * 256); i++)
		lightmap.map[i] = read_bitu8(src);
}

/// \brief reads the 256 colour palette values.
void TR_Level::read_tr_palette(SDL_RWops * const src, tr2_palette_t & palette)
{
	for (int i = 0; i < 256; i++)
		read_tr_colour(src, palette.colour[i]);
}

/// \brief reads a room sprite definition.
void TR_Level::read_tr_room_sprite(SDL_RWops * const src, tr_room_sprite_t & room_sprite)
{
	room_sprite.vertex = read_bit16(src);
	room_sprite.texture = read_bit16(src);
}

/** \brief reads a room portal definition.
  *
  * A check is preformed to see wether the normal lies on a coordinate axis, if not an exception gets thrown.
  */
void TR_Level::read_tr_room_portal(SDL_RWops * const src, tr_room_portal_t & portal)
{
	portal.adjoining_room = read_bitu16(src);
	read_tr_vertex16(src, portal.normal);
	read_tr_vertex16(src, portal.vertices[0]);
	read_tr_vertex16(src, portal.vertices[1]);
	read_tr_vertex16(src, portal.vertices[2]);
	read_tr_vertex16(src, portal.vertices[3]);
	if ((portal.normal.x == 1.0f) && (portal.normal.y == 0.0f) && (portal.normal.z == 0.0f))
		return;
	if ((portal.normal.x == -1.0f) && (portal.normal.y == 0.0f) && (portal.normal.z == 0.0f))
		return;
	if ((portal.normal.x == 0.0f) && (portal.normal.y == 1.0f) && (portal.normal.z == 0.0f))
		return;
	if ((portal.normal.x == 0.0f) && (portal.normal.y == -1.0f) && (portal.normal.z == 0.0f))
		return;
	if ((portal.normal.x == 0.0f) && (portal.normal.y == 0.0f) && (portal.normal.z == 1.0f))
		return;
	if ((portal.normal.x == 0.0f) && (portal.normal.y == 0.0f) && (portal.normal.z == -1.0f))
		return;
	throw TR_ReadError ("read_tr_room_portal: normal not on world axis", __FILE__, __LINE__, RCSID);
}

/// \brief reads a room sector definition.
void TR_Level::read_tr_room_sector(SDL_RWops * const src, tr_room_sector_t & sector)
{
	sector.fd_index = read_bitu16(src);
	sector.box_index = read_bitu16(src);
	sector.room_below = read_bitu8(src);
	sector.floor = read_bit8(src);
	sector.room_above = read_bitu8(src);
	sector.ceiling = read_bit8(src);
}

/** \brief reads a room light definition.
  *
  * intensity1 gets converted, so it matches the 0-32768 range introduced in TR3.
  * intensity2 and fade2 are introduced in TR2 and are set to intensity1 and fade1 for TR1.
  */
void TR_Level::read_tr_room_light(SDL_RWops * const src, tr5_room_light_t & light)
{
	read_tr_vertex32(src, light.pos);
	// read and make consistent
	light.intensity1 = (8191 - read_bitu16(src)) << 2;
	light.fade1 = read_bitu32(src);
	// only in TR2
	light.intensity2 = light.intensity1;
	light.fade2 = light.fade1;
}

/** \brief reads a room vertex definition.
  *
  * lighting1 gets converted, so it matches the 0-32768 range introduced in TR3.
  * lighting2 is introduced in TR2 and is set to lighting1 for TR1.
  * attributes is introduced in TR2 and is set 0 for TR1.
  * All other values are introduced in TR5 and get set to appropiate values.
  */
void TR_Level::read_tr_room_vertex(SDL_RWops * const src, tr5_room_vertex_t & room_vertex)
{
	read_tr_vertex16(src, room_vertex.vertex);
	// read and make consistent
	room_vertex.lighting1 = (8191 - read_bit16(src)) << 2;
	// only in TR2
	room_vertex.lighting2 = room_vertex.lighting1;
	room_vertex.attributes = 0;
	// only in TR5
	room_vertex.normal.x = 0;
	room_vertex.normal.y = 0;
	room_vertex.normal.z = 0;
	room_vertex.colour.r = room_vertex.lighting1 / 32767.0f;
	room_vertex.colour.g = room_vertex.lighting1 / 32767.0f;
	room_vertex.colour.b = room_vertex.lighting1 / 32767.0f;
	room_vertex.colour.a = 1.0f;
}

/** \brief reads a room staticmesh definition.
  *
  * rotation gets converted to float and scaled appropiatly.
  * intensity1 gets converted, so it matches the 0-32768 range introduced in TR3.
  * intensity2 is introduced in TR2 and is set to intensity1 for TR1.
  */
void TR_Level::read_tr_room_staticmesh(SDL_RWops * const src, tr2_room_staticmesh_t & room_static_mesh)
{
	read_tr_vertex32(src, room_static_mesh.pos);
	room_static_mesh.rotation = (float)read_bitu16(src) / 16384.0f * -90;
	room_static_mesh.intensity1 = read_bit16(src);
	room_static_mesh.object_id = read_bitu16(src);
	// make consistent
	if (room_static_mesh.intensity1 >= 0)
		room_static_mesh.intensity1 = (8191 - room_static_mesh.intensity1) << 2;
	// only in TR2
	room_static_mesh.intensity2 = room_static_mesh.intensity1;
}

/** \brief reads a room definition.
  *
  * intensity1 gets converted, so it matches the 0-32768 range introduced in TR3.
  * intensity2 is introduced in TR2 and is set to intensity1 for TR1.
  * light_mode is only in TR2 and is set 0 for TR1.
  * light_colour is only in TR3-4 and gets set appropiatly.
  */
void TR_Level::read_tr_room(SDL_RWops * const src, tr5_room_t & room)
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
		read_tr_room_vertex(src, room.vertices[i]);

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

	// read and make consistent
	room.intensity1 = (8191 - read_bit16(src)) << 2;
	// only in TR2-TR4
	room.intensity2 = room.intensity1;
	// only in TR2
	room.light_mode = 0;

	room.num_lights = read_bitu16(src);
	room.lights.resize(room.num_lights);
	for (i = 0; i < room.num_lights; i++)
		read_tr_room_light(src, room.lights[i]);

	room.num_static_meshes = read_bitu16(src);
	room.static_meshes.resize(room.num_static_meshes);
	for (i = 0; i < room.num_static_meshes; i++)
		read_tr_room_staticmesh(src, room.static_meshes[i]);

	room.alternate_room = read_bit16(src);
	room.flags = read_bitu16(src);

	// only in TR3-TR4
	room.light_colour.r = room.intensity1 / 32767.0f;
	room.light_colour.g = room.intensity1 / 32767.0f;
	room.light_colour.b = room.intensity1 / 32767.0f;
	room.light_colour.a = 1.0f;
}

/// \brief reads object texture vertex definition.
void TR_Level::read_tr_object_texture_vert(SDL_RWops * const src, tr4_object_texture_vert_t & vert)
{
	vert.xcoordinate = read_bit8(src);
	vert.xpixel = read_bitu8(src);
	vert.ycoordinate = read_bit8(src);
	vert.ypixel = read_bitu8(src);
}

/** \brief reads object texture definition.
  *
  * some sanity checks get done and if they fail an exception gets thrown.
  * all values introduced in TR4 get set appropiatly.
  */
void TR_Level::read_tr_object_texture(SDL_RWops * const src, tr4_object_texture_t & object_texture)
{
	object_texture.transparency_flags = read_bitu16(src);
	object_texture.tile = read_bitu8(src);
	if (object_texture.tile > 64)
		throw TR_ReadError ("object_texture.tile > 64", __FILE__, __LINE__, RCSID);

	object_texture.tile_flags = read_bitu8(src);
	if (object_texture.tile_flags != 0)
		throw TR_ReadError ("object_texture.tile_flags != 0", __FILE__, __LINE__, RCSID);

	// only in TR4
	object_texture.flags = 0;
	read_tr_object_texture_vert(src, object_texture.vertices[0]);
	read_tr_object_texture_vert(src, object_texture.vertices[1]);
	read_tr_object_texture_vert(src, object_texture.vertices[2]);
	read_tr_object_texture_vert(src, object_texture.vertices[3]);
	// only in TR4
	object_texture.unknown1 = 0;
	object_texture.unknown2 = 0;
	object_texture.x_size = 0;
	object_texture.y_size = 0;
}

/** \brief reads sprite texture definition.
  *
  * some sanity checks get done and if they fail an exception gets thrown.
  */
void TR_Level::read_tr_sprite_texture(SDL_RWops * const src, tr_sprite_texture_t & sprite_texture)
{
	sprite_texture.tile = read_bitu16(src);
	if (sprite_texture.tile > 64)
		throw TR_ReadError ("sprite_texture.tile > 64", __FILE__, __LINE__, RCSID);

	sprite_texture.x = read_bitu8(src);
	sprite_texture.y = read_bitu8(src);
	sprite_texture.width = read_bitu16(src);
	sprite_texture.height = read_bitu16(src);
	sprite_texture.left_side = read_bit16(src);
	sprite_texture.top_side = read_bit16(src);
	sprite_texture.right_side = read_bit16(src);
	sprite_texture.bottom_side = read_bit16(src);
}

/** \brief reads sprite sequence definition.
  *
  * length is negative when read and thus gets negated.
  */
void TR_Level::read_tr_sprite_sequence(SDL_RWops * const src, tr_sprite_sequence_t & sprite_sequence)
{
	sprite_sequence.object_id = read_bit32(src);
	sprite_sequence.length = -read_bit16(src);
	sprite_sequence.offset = read_bit16(src);
}

/** \brief reads mesh definition.
  *
  * The read num_normals value is positive when normals are available and negative when light
  * values are available. The values get set appropiatly.
  */
void TR_Level::read_tr_mesh(SDL_RWops * const src, tr4_mesh_t & mesh)
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
		read_tr_face4(src, mesh.textured_rectangles[i]);

	mesh.num_textured_triangles = read_bit16(src);
	mesh.textured_triangles.resize(mesh.num_textured_triangles);
	for (i = 0; i < mesh.num_textured_triangles; i++)
		read_tr_face3(src, mesh.textured_triangles[i]);

	mesh.num_coloured_rectangles = read_bit16(src);
	mesh.coloured_rectangles.resize(mesh.num_coloured_rectangles);
	for (i = 0; i < mesh.num_coloured_rectangles; i++)
		read_tr_face4(src, mesh.coloured_rectangles[i]);

	mesh.num_coloured_triangles = read_bit16(src);
	mesh.coloured_triangles.resize(mesh.num_coloured_triangles);
	for (i = 0; i < mesh.num_coloured_triangles; i++)
		read_tr_face3(src, mesh.coloured_triangles[i]);
}

/// \brief reads an animation definition.
void TR_Level::read_tr_animation(SDL_RWops * const src, tr_animation_t & animation)
{
	animation.frame_offset = read_bitu32(src);
	animation.frame_rate = read_bitu8(src);
	animation.frame_size = read_bitu8(src);
	animation.state_id = read_bitu16(src);

	animation.unknown = read_bit16(src);
	animation.speed = read_bit16(src);
	animation.accel_lo = read_bit16(src);
	animation.accel_hi = read_bit16(src);

	animation.frame_start = read_bitu16(src);
	animation.frame_end = read_bitu16(src);
	animation.next_animation = read_bitu16(src);
	animation.next_frame = read_bitu16(src);

	animation.num_state_changes = read_bitu16(src);
	animation.state_change_offset = read_bitu16(src);
	animation.num_anim_commands = read_bitu16(src);
	animation.anim_command = read_bitu16(src);
}

/// \brief reads a mesh tree value.
void TR_Level::read_tr_meshtree(SDL_RWops * const src, tr_meshtree_t & meshtree)
{
	meshtree.flags = read_bitu32(src);
	read_tr_vertex32(src, meshtree.offset);
}

void TR_Level::read_tr_frame(SDL_RWops * const src, tr_frame_t & frame, bitu32 num_rotations)
{
	bitu32 i;

	read_tr_vertex16(src, frame.bbox_low);
	read_tr_vertex16(src, frame.bbox_high);
	read_tr_vertex16(src, frame.offset);
	if (num_rotations != read_bitu16(src))
		throw TR_ReadError ("read_tr_frame: num_rotations != frame.num_rotations", __FILE__, __LINE__, RCSID);

	frame.rotations.resize(num_rotations);
	for (i = 0; i < frame.rotations.size(); i++) {
		bitu16 temp1;
		bitu16 temp2;

		// this order is intended
		temp2 = read_bitu16(src);
		temp1 = read_bitu16(src);
		frame.rotations[i].x = (float)((temp1 & 0x3ff0) >> 4);
		frame.rotations[i].y = (float)(((temp1 & 0x000f) << 6) | ((temp2 & 0xfc00) >> 10));
		frame.rotations[i].z = (float)(temp2 & 0x03ff);
	}
}

/** \brief reads a moveable definition.
  *
  * some sanity checks get done which throw a exception on failure.
  * frame_offset needs to be corrected later in TR_Level::read_tr_level.
  */
void TR_Level::read_tr_moveable(SDL_RWops * const src, tr_moveable_t & moveable)
{
	moveable.object_id = read_bitu32(src);
	moveable.num_meshes = read_bitu16(src);
	moveable.starting_mesh = read_bitu16(src);
	moveable.mesh_tree_index = read_bitu32(src);
	if ((moveable.mesh_tree_index % 4) != 0)
		throw TR_ReadError ("read_tr_moveable: mesh_tree_index", __FILE__, __LINE__, RCSID);

	moveable.mesh_tree_index /= 4;
	moveable.frame_offset = read_bitu32(src);
	moveable.animation_index = read_bitu16(src);
}

/// \brief reads an item definition.
void TR_Level::read_tr_item(SDL_RWops * const src, tr2_item_t & item)
{
	item.object_id = read_bit16(src);
	item.room = read_bit16(src);
	read_tr_vertex32(src, item.pos);
	item.rotation = (float)read_bitu16(src) / 16384.0f * -90;
	item.intensity1 = read_bitu16(src);
	if (item.intensity1 >= 0)
		item.intensity1 = (8191 - item.intensity1) << 2;
	item.intensity2 = item.intensity1;
	item.flags = read_bitu16(src);
}

/// \brief reads a static mesh definition.
void TR_Level::read_tr_staticmesh(SDL_RWops * const src, tr_staticmesh_t & mesh)
{
	mesh.object_id = read_bitu32(src);
	mesh.mesh = read_bitu16(src);

	mesh.visibility_box[0].x = (float)read_bit16(src);
	mesh.visibility_box[1].x = (float)read_bit16(src);
	mesh.visibility_box[0].y = (float)-read_bit16(src);
	mesh.visibility_box[1].y = (float)-read_bit16(src);
	mesh.visibility_box[0].z = (float)-read_bit16(src);
	mesh.visibility_box[1].z = (float)-read_bit16(src);

	mesh.collision_box[0].x = (float)read_bit16(src);
	mesh.collision_box[1].x = (float)read_bit16(src);
	mesh.collision_box[0].y = (float)-read_bit16(src);
	mesh.collision_box[1].y = (float)-read_bit16(src);
	mesh.collision_box[0].z = (float)-read_bit16(src);
	mesh.collision_box[1].z = (float)-read_bit16(src);

	mesh.flags = read_bitu16(src);
}

void TR_Level::read_tr_level(SDL_RWops * const src, bool demo_or_ub)
{
	bitu32 i;

	// Version
	bitu32 file_version = read_bitu32(src);

	if (file_version != 0x00000020)
		throw TR_ReadError ("Wrong level version", __FILE__, __LINE__, RCSID);

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

	// Unused
	if (read_bitu32(src) != 0)
		throw TR_ReadError ("Bad value for 'unused'", __FILE__, __LINE__, RCSID);

	this->rooms.resize(read_bitu16(src));
	for (i = 0; i < this->rooms.size(); i++)
		read_tr_room(src, this->rooms[i]);

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
		throw TR_ReadError ("read_tr_level: num_mesh_trees % 4 != 0", __FILE__, __LINE__, RCSID);

	num_mesh_trees /= 4;
	this->mesh_trees.resize(num_mesh_trees);
	for (i = 0; i < this->mesh_trees.size(); i++)
		read_tr_meshtree(src, this->mesh_trees[i]);

	read_frame_moveable_data(src);

	this->static_meshes.resize(read_bitu32(src));
	for (i = 0; i < this->static_meshes.size(); i++)
		read_tr_staticmesh(src, this->static_meshes[i]);

	this->object_textures.resize(read_bitu32(src));
	for (i = 0; i < this->object_textures.size(); i++)
		read_tr_object_texture(src, this->object_textures[i]);

	this->sprite_textures.resize(read_bitu32(src));
	for (i = 0; i < this->sprite_textures.size(); i++)
		read_tr_sprite_texture(src, this->sprite_textures[i]);

	this->sprite_sequences.resize(read_bitu32(src));
	for (i = 0; i < this->sprite_sequences.size(); i++)
		read_tr_sprite_sequence(src, this->sprite_sequences[i]);

	if (demo_or_ub)
		read_tr_palette(src, this->palette);

	this->cameras.resize(read_bitu32(src));
	SDL_RWseek(src, this->cameras.size() * 16, SEEK_CUR);

	this->sound_sources.resize(read_bitu32(src));
	SDL_RWseek(src, this->sound_sources.size() * 16, SEEK_CUR);

	this->boxes.resize(read_bitu32(src));
	SDL_RWseek(src, this->boxes.size() * 20, SEEK_CUR);

	this->overlaps.resize(read_bitu32(src));
	SDL_RWseek(src, this->overlaps.size() * 2, SEEK_CUR);

	// Zones
	SDL_RWseek(src, this->boxes.size() * 12, SEEK_CUR);

	this->animated_textures.resize(read_bitu32(src));
	SDL_RWseek(src, this->animated_textures.size() * 2, SEEK_CUR);

	this->items.resize(read_bitu32(src));
	for (i = 0; i < this->items.size(); i++)
		read_tr_item(src, this->items[i]);

	read_tr_lightmap(src, this->lightmap);

	if (!demo_or_ub)
		read_tr_palette(src, this->palette);

	this->cinematic_frames.resize(read_bitu16(src));
	SDL_RWseek(src, this->cinematic_frames.size() * 16, SEEK_CUR);

	this->demo_data.resize(read_bitu16(src));
	SDL_RWseek(src, this->demo_data.size(), SEEK_CUR);

	// Soundmap
	SDL_RWseek(src, 2 * 256, SEEK_CUR);

	this->sound_details.resize(read_bitu32(src));
	SDL_RWseek(src, this->sound_details.size() * 8, SEEK_CUR);

	this->samples.resize(read_bitu32(src));
	SDL_RWseek(src, this->samples.size(), SEEK_CUR);

	this->sample_indices.resize(read_bitu32(src));
	SDL_RWseek(src, this->sample_indices.size() * 4, SEEK_CUR);
}
