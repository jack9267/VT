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
#ifdef WIN32
# include <tchar.h>
# include <direct.h>
# define chdir _chdir
#else
# include <unistd.h>
#endif
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <hash_set>
#include "SDL.h"
#include "dyngl.h"
#include "vt_level.h"
#include "scaler.h"
#include "glmath.h"
#include "util.h"

#define RCSID "$Id: test.cpp,v 1.17 2002/09/20 15:59:02 crow Exp $"

static vec3_t pos;
static int current_room = 0;
static tr2_item_t *lara;

typedef struct {
	vec3_t clip_normals[5];
} frustum_t;

void draw_tr4_mesh(VT_Level &level, tr4_mesh_t * mesh, int intensity)
{
	int i;

	assert(mesh);

	if (intensity >= 0)
		qglColor4f((float)intensity / 32768.0f, (float)intensity / 32768.0f, (float)intensity / 32768.0f, 1.0f);
	else
		qglColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for (i = 0; i < mesh->num_textured_rectangles; i++) {
		tr4_face4_t *face = &mesh->textured_rectangles[i];
		tr4_object_texture_t *tex = &level.object_textures[face->texture & 0x0fff];
		tr4_object_texture_vert_t *tex_vert;

		qglBindTexture(GL_TEXTURE_2D, (tex->tile & 0x0fff) + 1);

		qglBegin(GL_QUADS);
		tex_vert = &tex->vertices[0];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[0]]);

		tex_vert = &tex->vertices[1];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[1]]);

		tex_vert = &tex->vertices[2];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[2]]);

		tex_vert = &tex->vertices[3];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[3]]);
		qglEnd();
	}
	for (i = 0; i < mesh->num_textured_triangles; i++) {
		tr4_face3_t *face = &mesh->textured_triangles[i];
		tr4_object_texture_t *tex = &level.object_textures[face->texture & 0x0fff];
		tr4_object_texture_vert_t *tex_vert;

		qglBindTexture(GL_TEXTURE_2D, (tex->tile & 0x0fff) + 1);

		qglBegin(GL_TRIANGLES);
		tex_vert = &tex->vertices[0];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[0]]);

		tex_vert = &tex->vertices[1];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[1]]);

		tex_vert = &tex->vertices[2];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[2]]);
		qglEnd();
	}
	for (i = 0; i < mesh->num_coloured_rectangles; i++) {
		tr4_face4_t *face = &mesh->coloured_rectangles[i];
		int col = face->texture & 0xff;

		qglColor3f(level.palette.colour[col].r, level.palette.colour[col].g, level.palette.colour[col].b);

		qglBindTexture(GL_TEXTURE_2D, 0);

		qglBegin(GL_QUADS);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[0]]);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[1]]);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[2]]);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[3]]);
		qglEnd();
	}
	for (i = 0; i < mesh->num_coloured_triangles; i++) {
		tr4_face3_t *face = &mesh->coloured_triangles[i];
		int col = face->texture & 0xff;

		qglColor3f(level.palette.colour[col].r, level.palette.colour[col].g, level.palette.colour[col].b);

		qglBindTexture(GL_TEXTURE_2D, 0);

		qglBegin(GL_TRIANGLES);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[0]]);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[1]]);
		qglVertex3fv((GLfloat *) & mesh->vertices[face->vertices[2]]);
		qglEnd();
	}
}

void draw_staticmesh(VT_Level &level, tr_staticmesh_t * mesh, int intensity)
{
	assert(mesh);
	DBG(qglBindTexture(GL_TEXTURE_2D, 0); qglColor4f(1.0f, 1.0f, 0.0f, 1.0f); draw_box(mesh->visibility_box[0], mesh->visibility_box[1], false); draw_marker(mesh->visibility_box[0], 32, false); qglColor4f(1.0f, 0.0f, 0.0f, 1.0f); draw_marker(mesh->visibility_box[1], 32, false); qglColor4f(0.0f, 1.0f, 0.0f, 1.0f); draw_box(mesh->collision_box[0], mesh->collision_box[1], false); draw_marker(mesh->collision_box[0], 32, false); qglColor4f(0.0f, 0.0f, 1.0f, 1.0f); draw_marker(mesh->collision_box[1], 32, false););	// DBG
	draw_tr4_mesh(level, &level.meshes[level.mesh_indices[mesh->mesh]], intensity);
}

void draw_room_staticmesh(VT_Level &level, tr2_room_staticmesh_t * mesh)
{
	assert(mesh);
	qglPushMatrix();
	qglTranslatef(mesh->pos.x, mesh->pos.y, mesh->pos.z);
	qglRotatef(mesh->rotation, 0.0f, 1.0f, 0.0f);
	draw_staticmesh(level, level.find_staticmesh_id(mesh->object_id), mesh->intensity1);
	qglPopMatrix();
}

void draw_room_portal(VT_Level &level, tr_room_portal_t & portal)
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	int i;

	qglBindTexture(GL_TEXTURE_2D, 0);
	qglColor4f(1.0f, 0.0f, 0.0f, 0.6f);
	qglBegin(GL_QUADS);
	qglVertex3fv((GLfloat *) & portal.vertices[0]);
	qglColor4f(0.0f, 1.0f, 0.0f, 0.6f);
	qglVertex3fv((GLfloat *) & portal.vertices[1]);
	qglColor4f(0.0f, 0.0f, 1.0f, 0.6f);
	qglVertex3fv((GLfloat *) & portal.vertices[2]);
	qglColor4f(1.0f, 1.0f, 1.0f, 0.6f);
	qglVertex3fv((GLfloat *) & portal.vertices[3]);
	qglEnd();

	for (i = 0; i < 4; i++) {
		x += portal.vertices[i].x;
		y += portal.vertices[i].y;
		z += portal.vertices[i].z;
	}
	x /= 4.0f;
	y /= 4.0f;
	z /= 4.0f;
	qglColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	qglBegin(GL_LINES);
	qglVertex3f(x, y, z);
	qglVertex3f(x + portal.normal.x * 128, y + portal.normal.y * 128, z + portal.normal.z * 128);
	qglEnd();
}

static bitu8 *room_drawn;
static int rooms_drawn;
static int rooms_visited;
static std::hash_set<int> room_stack;

void draw_room(VT_Level &level, int room_num, frustum_t frustum)
{
	tr5_room_t &room = level.rooms[room_num];
	bitu32 i;
#if 1
	frustum_t new_frustum;

	rooms_visited++;

	if (room_stack.count(room_num))
		return;
	room_stack.insert(room_num);

	for (i = 0; i < room.portals.size(); i++) {
		vec3_t tempvec[4];
		vec_t dist1;
		vec_t dist2;

		tempvec[0] = room.portals[i].vertices[0] + room.offset - pos;
		tempvec[1] = room.portals[i].vertices[1] + room.offset - pos;
		tempvec[2] = room.portals[i].vertices[2] + room.offset - pos;
		tempvec[3] = room.portals[i].vertices[3] + room.offset - pos;

		// 0
		dist1 = frustum.clip_normals[0].dot(tempvec[0]);
		dist2 = frustum.clip_normals[0].dot(tempvec[2]);
		if ((dist1 < 0) && (dist2 < 0))
			continue;

		// 1
		dist1 = frustum.clip_normals[1].dot(tempvec[0]);
		dist2 = frustum.clip_normals[1].dot(tempvec[2]);
		if ((dist1 < 0) && (dist2 < 0)) // not visible
			break;
		if ((dist1 >= 0) && (dist2 >= 0)) { // fully visible
			new_frustum.clip_normals[1] = tempvec[0].cross(tempvec[1]);
			new_frustum.clip_normals[1].normalize();
		} else // partly visible
			new_frustum.clip_normals[1] = frustum.clip_normals[1];

		// 2
		dist1 = frustum.clip_normals[2].dot(tempvec[0]);
		dist2 = frustum.clip_normals[2].dot(tempvec[2]);
		if ((dist1 < 0) && (dist2 < 0)) // not visible
			break;
		if ((dist1 >= 0) && (dist2 >= 0)) { // fully visible
			new_frustum.clip_normals[2] = tempvec[1].cross(tempvec[2]);
			new_frustum.clip_normals[2].normalize();
		} else // partly visible
			new_frustum.clip_normals[2] = frustum.clip_normals[2];

		// 3
		dist1 = frustum.clip_normals[3].dot(tempvec[0]);
		dist2 = frustum.clip_normals[3].dot(tempvec[2]);
		if ((dist1 < 0) && (dist2 < 0)) // not visible
			break;
		if ((dist1 >= 0) && (dist2 >= 0)) { // fully visible
			new_frustum.clip_normals[3] = tempvec[2].cross(tempvec[3]);
			new_frustum.clip_normals[3].normalize();
		} else // partly visible
			new_frustum.clip_normals[3] = frustum.clip_normals[3];

		// 4
		dist1 = frustum.clip_normals[4].dot(tempvec[0]);
		dist2 = frustum.clip_normals[4].dot(tempvec[2]);
		if ((dist1 < 0) && (dist2 < 0)) // not visible
			break;
		if ((dist1 >= 0) && (dist2 >= 0)) { // fully visible
			new_frustum.clip_normals[4] = tempvec[3].cross(tempvec[0]);
			new_frustum.clip_normals[4].normalize();
		} else // partly visible
			new_frustum.clip_normals[4] = frustum.clip_normals[4];

		new_frustum.clip_normals[0] = frustum.clip_normals[0];

		draw_room(level, room.portals[i].adjoining_room, new_frustum);
/*
		bitu32 j;
		vec3_t tempvec;
		vec_t dist1;
		vec_t dist2;
		bool visible = true;

		tempvec = room.portals[i].vertices[0] + room.offset - pos;
		dist1 = frustum.clip_normals[0].dot(tempvec);
		tempvec = room.portals[i].vertices[2] + room.offset - pos;
		dist2 = frustum.clip_normals[0].dot(tempvec);
		if ((dist1 < 0) && (dist2 < 0))
			continue;

		for (j = 1; j < 5; j++) {
			tempvec = room.portals[i].vertices[0] + room.offset - pos;
			dist1 = frustum.clip_normals[j].dot(tempvec);
			tempvec = room.portals[i].vertices[2] + room.offset - pos;
			dist2 = frustum.clip_normals[j].dot(tempvec);
			if ((dist1 < 0) && (dist2 < 0)) {
				visible = false;
				break;
			}
		}
		if (visible == 0)
			continue;
		tempvec = room.portals[i].vertices[0] + room.offset - pos;
		new_frustum.clip_normals[1] = tempvec.cross(room.portals[i].vertices[1] + room.offset - pos);
		new_frustum.clip_normals[1].normalize();
		tempvec = room.portals[i].vertices[1] + room.offset - pos;
		new_frustum.clip_normals[2] = tempvec.cross(room.portals[i].vertices[2] + room.offset - pos);
		new_frustum.clip_normals[2].normalize();
		tempvec = room.portals[i].vertices[2] + room.offset - pos;
		new_frustum.clip_normals[3] = tempvec.cross(room.portals[i].vertices[3] + room.offset - pos);
		new_frustum.clip_normals[3].normalize();
		tempvec = room.portals[i].vertices[3] + room.offset - pos;
		new_frustum.clip_normals[4] = tempvec.cross(room.portals[i].vertices[0] + room.offset - pos);
		new_frustum.clip_normals[4].normalize();
		draw_room(level, room.portals[i].adjoining_room, new_frustum);
*/
	}

	room_stack.erase(room_num);

	if (room_drawn[room_num])
		return;
	room_drawn[room_num] = 1;

	rooms_drawn++;
#endif
	qglPushMatrix();
	qglTranslatef(room.offset.x, 0, room.offset.z);
	for (i = 0; i < room.num_rectangles; i++) {
		tr4_face4_t *face = &room.rectangles[i];
		tr4_object_texture_t *tex = &level.object_textures[face->texture & 0x0fff];
		tr4_object_texture_vert_t *tex_vert;
		tr5_room_vertex_t *vert;

		qglBindTexture(GL_TEXTURE_2D, (tex->tile & 0x0fff) + 1);

		qglBegin(GL_QUADS);
		//qglBegin(GL_LINE_LOOP);
		vert = &room.vertices[face->vertices[0]];
		qglColor3f(vert->colour.r, vert->colour.g, vert->colour.b);
		tex_vert = &tex->vertices[0];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3f(vert->vertex.x, vert->vertex.y, vert->vertex.z);
		vert = &room.vertices[face->vertices[1]];
		qglColor3f(vert->colour.r, vert->colour.g, vert->colour.b);
		tex_vert = &tex->vertices[1];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3f(vert->vertex.x, vert->vertex.y, vert->vertex.z);
		vert = &room.vertices[face->vertices[2]];
		qglColor3f(vert->colour.r, vert->colour.g, vert->colour.b);
		tex_vert = &tex->vertices[2];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3f(vert->vertex.x, vert->vertex.y, vert->vertex.z);
		vert = &room.vertices[face->vertices[3]];
		qglColor3f(vert->colour.r, vert->colour.g, vert->colour.b);
		tex_vert = &tex->vertices[3];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3f(vert->vertex.x, vert->vertex.y, vert->vertex.z);
		qglEnd();
	}
	for (i = 0; i < room.num_triangles; i++) {
		tr4_face3_t *face = &room.triangles[i];
		tr4_object_texture_t *tex = &level.object_textures[face->texture & 0x0fff];
		tr4_object_texture_vert_t *tex_vert;
		tr5_room_vertex_t *vert;

		qglBindTexture(GL_TEXTURE_2D, (tex->tile & 0x0fff) + 1);

		qglBegin(GL_TRIANGLES);
		//qglBegin(GL_LINE_LOOP);
		vert = &room.vertices[face->vertices[0]];
		qglColor3f(vert->colour.r, vert->colour.g, vert->colour.b);
		tex_vert = &tex->vertices[0];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3f(vert->vertex.x, vert->vertex.y, vert->vertex.z);
		vert = &room.vertices[face->vertices[1]];
		qglColor3f(vert->colour.r, vert->colour.g, vert->colour.b);
		tex_vert = &tex->vertices[1];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3f(vert->vertex.x, vert->vertex.y, vert->vertex.z);
		vert = &room.vertices[face->vertices[2]];
		qglColor3f(vert->colour.r, vert->colour.g, vert->colour.b);
		tex_vert = &tex->vertices[2];
		qglTexCoord2f(((float)tex_vert->xpixel + (float)tex_vert->xcoordinate * 0.5f) / 256.0f, ((float)tex_vert->ypixel + (float)tex_vert->ycoordinate * 0.5f) / 256.0f);
		qglVertex3f(vert->vertex.x, vert->vertex.y, vert->vertex.z);
		qglEnd();
	}
	qglPopMatrix();
	DBG(for (i = 0; i < room.num_lights; i++) {
	    tr5_room_light_t & light = room.lights[i]; qglBindTexture(GL_TEXTURE_2D, 0); qglColor3f(1.0f, 1.0f, 1.0f); draw_marker(light.pos, 32, true);}
	) ;
	for (i = 0; i < room.num_static_meshes; i++)
		draw_room_staticmesh(level, &room.static_meshes[i]);
}

void draw_sprite_texture(tr_sprite_texture_t * sprite_texture)
{
	float x1, x2, y1, y2;
	int width, height;

	width = (sprite_texture->width) / 256;
	height = (sprite_texture->height) / 256;
	x1 = (float)sprite_texture->x / 256.0f;
	x2 = x1 + (float)width / 256.0f;
	y1 = (float)sprite_texture->y / 256.0f;
	y2 = y1 + (float)height / 256.0f;
	qglBindTexture(GL_TEXTURE_2D, (sprite_texture->tile & 0x0fff) + 1);
	qglColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	qglBegin(GL_QUADS);
	qglTexCoord2f(x1, y1);
	qglVertex2f(320.0f + (float)sprite_texture->left_side, 240.0f + (float)sprite_texture->top_side);
	qglTexCoord2f(x2, y1);
	qglVertex2f(320.0f + (float)sprite_texture->right_side, 240.0f + (float)sprite_texture->top_side);
	qglTexCoord2f(x2, y2);
	qglVertex2f(320.0f + (float)sprite_texture->right_side, 240.0f + (float)sprite_texture->bottom_side);
	qglTexCoord2f(x1, y2);
	qglVertex2f(320.0f + (float)sprite_texture->left_side, 240.0f + (float)sprite_texture->bottom_side);
	qglEnd();
}

void draw_item(VT_Level &level, tr2_item_t & item, int object_id)
{
	tr_moveable_t *moveable;
	tr_meshtree_t *mesh_tree;
	int tos = 1;

	moveable = level.find_moveable_id(object_id);
	/*if (!moveable)
		return;
	if (moveable->mesh_tree_index >= level.mesh_trees.size())
		return;
	if (moveable->frame_index >= level.frames.size())
		return;
	if (level.frames[moveable->frame_index].byte_offset < 0)
		return;*/

	qglPushMatrix();
	qglTranslatef(item.pos.x, item.pos.y, item.pos.z);
	qglRotatef(item.rotation, 0.0f, 1.0f, 0.0f);
	mesh_tree = &level.mesh_trees[moveable->mesh_tree_index];
	for (int i = 0; i < moveable->num_meshes; i++) {
		draw_tr4_mesh(level, &level.meshes[level.mesh_indices[moveable->starting_mesh] + i], item.intensity1);
		if (mesh_tree[i].flags & 1) {
			qglPopMatrix();
			tos--;
		}
		if (mesh_tree[i].flags & 2) {
			qglPushMatrix();
			tos++;
		}
		qglTranslatef(mesh_tree[i].offset.x, mesh_tree[i].offset.y, mesh_tree[i].offset.z);
	}
	while (tos-- > 0)
		qglPopMatrix();
}

void init_textures(VT_Level &level)
{
	bitu8 *buffer;
	unsigned int i;

	buffer = new bitu8[512 * 512 * 4];

	for (i = 0; i < level.textile32.size(); i++) {
		qglBindTexture(GL_TEXTURE_2D, i + 1);
		//Super2xSaI((bitu8 *)&textures[i].pixels, 1024, 4, buffer, 2048, 4, 256, 256, NULL);
		//qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, level.textile32[i].pixels);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	delete [] buffer;
}

bool main_loop(VT_Level &level, SDL_Surface * const screen)
{
	static bitu32 room = 0;
	static bit32 sprite = 0;
	static ang3_t angles;
	static int redraw = 1;

	bitu32 i;
	int quit = 0;
	SDL_Event event;
	vec3_t forward, right, up;
	float scale;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			quit = 1;
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			redraw = 1;
			angles.pitch += (float)event.motion.yrel / 2.5f;//270.0f * F_PI;
			if (angles.pitch > 90)
				angles.pitch = 90;
			if (angles.pitch < -90)
				angles.pitch = -90;
			angles.yaw -= (float)event.motion.xrel / 2.0f;//180.0f * F_PI;
			break;
		case SDL_KEYDOWN:
			redraw = 1;
			switch (event.key.keysym.sym) {
			case SDLK_PAGEUP:
				if ((--current_room) < 0)
					current_room = level.rooms.size() - 1;
				break;
			case SDLK_PAGEDOWN:
				if ((++current_room) >= level.rooms.size())
					current_room = 0;
				break;
			case SDLK_LEFT:
				angles.roll += 10.0f;
				break;
			case SDLK_RIGHT:
				angles.roll -= 10.0f;
				break;
			case SDLK_ESCAPE:
				quit = 1;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	AngleVectors(angles, &forward, &right, &up);

	if (SDL_GetModState() & KMOD_SHIFT)
		scale = 192.0f;
	else
		scale = 368.0f;
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)) {
		pos += forward * scale;
		redraw = 1;
	}
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(3)) {
		pos -= forward * scale;
		redraw = 1;
	}
	if (redraw) {
		frustum_t frustum;

		//qglViewport((screen->w / 2), 0, (screen->w / 2), (screen->h / 2));
		qglViewport(0, 0, screen->w, screen->h);
		qglMatrixMode(GL_PROJECTION);
		qglLoadIdentity();
		infinitePerspective(90.0, 1.3333, 64.0);
		qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		qglMatrixMode(GL_MODELVIEW);
		qglLoadIdentity();
		qglRotatef(-angles.roll, 0.0, 0.0, 1.0);
		qglRotatef(-angles.pitch, 1.0, 0.0, 0.0);
		qglRotatef(-angles.yaw, 0.0, 1.0, 0.0);
		qglTranslatef(-pos[0], -pos[1], -pos[2]);
		for (i = 0; i < 5; i++)
			frustum.clip_normals[i] = forward;

		for (i = 0; i < level.rooms.size(); i++)
			draw_room(level, i, frustum);

		memset(room_drawn, 0, level.rooms.size());
		rooms_drawn = 0;
		rooms_visited = 0;
		//draw_room(level, current_room, frustum);
		room_stack.clear();

		for (i = 0; i < level.items.size(); i++) {
			if (level.items[i].object_id == 0) {
				switch (level.game_version) {
				case TR_III:
					draw_item(level, level.items[i], 315);
					break;
				case TR_IV:
				case TR_V:
					draw_item(level, level.items[i], 8);
					draw_item(level, level.items[i], 9);
					break;
				case TR_IV_DEMO:
					draw_item(level, level.items[i], 10);
					draw_item(level, level.items[i], 11);
					break;
				default:
					draw_item(level, level.items[i], level.items[i].object_id);
				}
			} else
				draw_item(level, level.items[i], level.items[i].object_id);
		}

		redraw = 0;
		qglLoadIdentity();
		qglTranslatef(0, 0, -100);
		qglScalef(10,10,10);
		qglBindTexture(GL_TEXTURE_2D, 0);
		qglBegin(GL_LINES);
		qglColor3f(1, 0, 0);
		qglVertex3fv(&vec3_origin.x);
		qglVertex3fv(&forward.x);
		qglColor3f(0, 1, 0);
		qglVertex3fv(&vec3_origin.x);
		qglVertex3fv(&right.x);
		qglColor3f(0, 0, 1);
		qglVertex3fv(&vec3_origin.x);
		qglVertex3fv(&up.x);
		qglEnd();

		qglViewport(0, 0, screen->w, screen->h);
		qglMatrixMode(GL_PROJECTION);
		qglLoadIdentity();
		qglOrtho(0, 640, 480, 0, -1, 1);
		qglMatrixMode(GL_MODELVIEW);
		qglLoadIdentity();
		draw_string(0, 0, 1.0f, 1.0f, 1.0f, 1.0f, "x: %5.0f, y: %5.0f, z: %5.0f\nyaw: %6.2f, pitch: %6.2f, roll: %6.2f\nrooms_drawn %4i\nrooms_visited %4i", pos[0], pos[1], pos[2], angles[0], angles[1], angles[2], rooms_drawn, rooms_visited);
/*
		draw_string(0, 0, 1.0f, 1.0f, 1.0f, 1.0f, "Sprite: %i", sprite);
		draw_sprite_texture(&level.sprite_textures[sprite]);
*/
		SDL_GL_SwapBuffers();
	}

	if (quit)
		return false;
	else
		return true;
}

#if (defined(_DEBUG) && defined(_MSC_VER))
static int normal_count = 0;
static int free_count = 0;

int MyAllocHook(int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char *szFileName, int nLine)
{
	char *operation[] = { "", "allocating", "re-allocating", "freeing" };
	char *blockType[] = { "Free", "Normal", "CRT", "Ignore", "Client" };

	if (nBlockUse == _CRT_BLOCK)	// Ignore internal C runtime library allocations
		return (TRUE);

	_ASSERT((nAllocType > 0) && (nAllocType < 4));
	_ASSERT((nBlockUse >= 0) && (nBlockUse < 5));

	if (nAllocType == 3)
		free_count++;
	if (nAllocType == 1)
		normal_count++;
	fprintf(stdout, "Memory operation in %s, line %d: %s a %d-byte '%s' block (# %ld)", szFileName, nLine, operation[nAllocType], nSize, blockType[nBlockUse], lRequest);
	if (pvData != NULL)
		fprintf(stdout, " at %X", pvData);
	fprintf(stdout, "\n");
	fflush(stdout);

	return (TRUE);		// Allow the memory operation to proceed
}
#endif

int _tmain(int argc, _TCHAR* argv[])
//int main(int argc, char **argv)
{
	VT_Level *level = NULL;
	int game_num, level_num;
	SDL_Surface *screen;

#if 0
#if (defined(_DEBUG) && defined(_MSC_VER))
	int tmpDbgFlag;
	_CrtMemState s1, s2, s3;

	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_ALLOC_MEM_DF;
	tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	tmpDbgFlag |= _CRTDBG_CHECK_CRT_DF;
	//tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

	//_CrtSetAllocHook( MyAllocHook );
#endif

	for (game_num = 0; gamepath_info[game_num].path != NULL; game_num++) {
		printf("Changing to %s\n", gamepath_info[game_num].path);
		fprintf(stderr, "Changing to %s\n", gamepath_info[game_num].path);
		chdir(gamepath_info[game_num].path);
		for (level_num = 0; gamepath_info[game_num].levels[level_num].filename != NULL; level_num++) {
			level = NULL;
			try {
				printf("Loading Level %s\n", gamepath_info[game_num].levels[level_num].filename);
				fflush(stdout);
				fprintf(stderr, "Loading Level %s\n", gamepath_info[game_num].levels[level_num].filename);
				level = new VT_Level;

				level->read_level(SDL_RWFromFile(gamepath_info[game_num].levels[level_num].filename, "rb"), gamepath_info[game_num].version);
				level->prepare_level();
			}
			catch(TR_ReadError except) {
				printf("Error: %s in %s:%i\n%s\n", except.m_message, except.m_file, except.m_line, except.m_rcsid);
				fflush(stdout);
				fprintf(stderr, "Error: %s in %s:%i\n%s\n", except.m_message, except.m_file, except.m_line, except.m_rcsid);
			}
			delete level;

			MSCDBG(if (free_count != normal_count) printf("free_count(%i) != normal_count(%i)\n", free_count, normal_count);) ;
		}
	}

	MSCDBG(_CrtDumpMemoryLeaks();
		);

	return 0;
#endif

	game_num = 10;
	level_num = 4;

	//chdir(gamepath_info[game_num].path);
	try {
		level = new VT_Level;

		level->read_level(SDL_RWFromFile("DATA/TUT1.TR4", "rb"), gamepath_info[game_num].version);
		//level->read_level(SDL_RWFromFile(gamepath_info[game_num].levels[level_num].filename, "rb"), gamepath_info[game_num].version);
		level->prepare_level();
	}
	catch(TR_ReadError *except) {
		printf("Error: %s in %s:%i\n%s\n", except->m_message, except->m_file, except->m_line, except->m_rcsid);
		fprintf(stderr, "Error: %s in %s:%i\n%s\n", except->m_message, except->m_file, except->m_line, except->m_rcsid);
		delete level;

		return 1;
	}

	room_drawn = new bitu8[level->rooms.size()];

	//DBG(dump_textures(*level););

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	if (DynGL_LoadLibrary("OPENGL32.DLL") == SDL_FALSE) {
		fprintf(stderr, "DynGL_LoadLibrary failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if ((screen = SDL_SetVideoMode(640, 480, 0, SDL_OPENGL)) == NULL)
		return 1;

	if (DynGL_GetFunctions(NULL) == SDL_FALSE) {
		fprintf(stderr, "DynGL_GetFunctions failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_ShowCursor(0);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	qglEnable(GL_DEPTH_TEST);
	qglDepthFunc(GL_LESS);
	qglClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	qglEnable(GL_TEXTURE_2D);
	qglEnable(GL_CULL_FACE);
	qglCullFace(GL_FRONT);
	qglEnable(GL_BLEND);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	qglEnable(GL_ALPHA_TEST);
	qglAlphaFunc(GL_GEQUAL, 0.5f);

	//init_font("c:/home/dev/cvs/vt2/data/font.bmp", 64);
	init_font("font.bmp", 64);
	init_textures(*level);

	if ((lara = level->find_item_id(0)) != NULL) {
		pos[0] = lara->pos.x;
		pos[1] = lara->pos.y + 490.0f;
		pos[2] = lara->pos.z;
		current_room = lara->room;
	}

	while (main_loop(*level, screen)) ;

	delete level;

	DynGL_CloseLibrary();
	SDL_Quit();

	return 0;
}
