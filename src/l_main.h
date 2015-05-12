#ifndef _L_MAIN_H_
#define _L_MAIN_H_

#include "tr_types.h"

typedef enum {
	TR_I,
	TR_I_DEMO,
	TR_I_UB,
	TR_II,
	TR_II_DEMO,
	TR_III,
	TR_IV,
	TR_IV_DEMO,
	TR_V
} tr_version_e;

class TR_ReadError {
      public:
	char *m_message;///< \brief takes the pointer to the error string.
	char *m_file;	///< \brief takes the pointer to the file string.
	int m_line;	///< \brief takes the line of the error.
	char *m_rcsid;	///< \brief takes the pointer to the rcsid string.

	TR_ReadError(char *const message, char *const file = NULL, int line = 0, char *const rcsid = NULL)
	{
		m_message = message;
		m_file = file;
		m_line = line;
		m_rcsid = rcsid;
	}
};

/** \brief A complete TR level.
  *
  * This contains all necessary functions to load a TR level.
  * Some corrections to the data are done, like converting to OpenGLs coordinate system.
  * All indexes are converted, so they can be used directly.
  * Endian conversion is done at the lowest possible layer, most of the time this is in the read_bitxxx functions.
  */
class TR_Level {
      public:
	tr_version_e game_version;	///< \brief game engine version.
	tr_textile8_array_t textile8;	///< \brief 8-bit 256x256 textiles(TR1-3).
	tr2_textile16_array_t textile16;	///< \brief 16-bit 256x256 textiles(TR2-5).
	tr4_textile32_array_t textile32;	///< \brief 32-bit 256x256 textiles(TR4-5).
	tr5_room_array_t rooms;	///< \brief all rooms (normal and alternate).
	bitu16_array_t floor_data;	///< \brief the floor data.
	tr4_mesh_array_t meshes;	///< \brief all meshes (static and moveables).
	bitu32_array_t mesh_indices;	///< \brief mesh index table.
	tr_animation_array_t animations;	///< \brief animations for moveables.
	tr_state_change_array_t state_changes;	///< \brief state changes for moveables.
	tr_anim_dispatch_array_t anim_dispatches;	///< \brief animation dispatches for moveables.
	tr_anim_command_array_t anim_commands;	///< \brief animation commands for moveables.
	tr_meshtree_array_t mesh_trees;	///< \brief mesh trees for moveables.
	tr_frame_array_t frames;	///< \brief frames for moveables.
	tr_moveable_array_t moveables;	///< \brief data for the moveables.
	tr_staticmesh_array_t static_meshes;	///< \brief data for the static meshes.
	tr4_object_texture_array_t object_textures;	///< \brief object texture definitions.
	tr_animated_texture_array_t animated_textures;	///< \brief animated textures.
	tr_sprite_texture_array_t sprite_textures;	///< \brief sprite texture definitions.
	tr_sprite_sequence_array_t sprite_sequences;	///< \brief sprite sequences for animation.
	tr_camera_array_t cameras;	///< \brief cameras.
	tr4_flyby_camera_array_t flyby_cameras;	///< \brief flyby cameras.
	tr_sound_source_array_t sound_sources;	///< \brief sound sources.
	tr_box_array_t boxes;	///< \brief boxes.
	bitu16_array_t overlaps;	///< \brief overlaps.
	bit16_array_t zones;	///< \brief zones.
	tr2_item_array_t items;	///< \brief items.
	tr_lightmap_t lightmap;	///< \brief ligthmap (TR1-3).
	tr2_palette_t palette;	///< \brief colour palette (TR1-3).
	tr2_palette_t palette16;	///< \brief colour palette (TR2-3).
	tr4_ai_object_array_t ai_objects;	///< \brief ai objects (TR4-5).
	tr_cinematic_frame_array_t cinematic_frames;	///< \brief cinematic frames (TR1-3).
	bitu8_array_t demo_data;	///< \brief demo data.
	bit16 soundmap[450];	///< \brief soundmap (TR: 256 values TR2-4: 370 values TR5: 450 values).
	tr_sound_detail_array_t sound_details;	///< \brief sound details.
	bitu8_array_t samples;	///< \brief samples.
	bitu32_array_t sample_indices;	///< \brief sample indices.

	void read_level(const char *filename, tr_version_e game_version);
	void read_level(SDL_RWops * const src, tr_version_e game_version);

      protected:
	bitu32 num_textiles;	///< \brief number of 256x256 textiles.
	bitu32 num_room_textiles;	///< \brief number of 256x256 room textiles (TR4-5).
	bitu32 num_obj_textiles;	///< \brief number of 256x256 object textiles (TR4-5).
	bitu32 num_bump_textiles;	///< \brief number of 256x256 bump textiles (TR4-5).
	bitu32 num_misc_textiles;	///< \brief number of 256x256 misc textiles (TR4-5).
	bool read_32bit_textiles;	///< \brief are other 32bit textiles than misc ones read?

	bit8 read_bit8(SDL_RWops * const src);
	bitu8 read_bitu8(SDL_RWops * const src);
	bit16 read_bit16(SDL_RWops * const src);
	bitu16 read_bitu16(SDL_RWops * const src);
	bit32 read_bit32(SDL_RWops * const src);
	bitu32 read_bitu32(SDL_RWops * const src);
	float read_float(SDL_RWops * const src);

	void read_mesh_data(SDL_RWops * const src);
	void read_frame_moveable_data(SDL_RWops * const src);

	void read_tr_colour(SDL_RWops * const src, tr2_colour_t & colour);
	void read_tr_vertex16(SDL_RWops * const src, tr5_vertex_t & vertex);
	void read_tr_vertex32(SDL_RWops * const src, tr5_vertex_t & vertex);
	void read_tr_face3(SDL_RWops * const src, tr4_face3_t & face);
	void read_tr_face4(SDL_RWops * const src, tr4_face4_t & face);
	void read_tr_textile8(SDL_RWops * const src, tr_textile8_t & textile);
	void read_tr_lightmap(SDL_RWops * const src, tr_lightmap_t & lightmap);
	void read_tr_palette(SDL_RWops * const src, tr2_palette_t & palette);
	void read_tr_room_sprite(SDL_RWops * const src, tr_room_sprite_t & room_sprite);
	void read_tr_room_portal(SDL_RWops * const src, tr_room_portal_t & portal);
	void read_tr_room_sector(SDL_RWops * const src, tr_room_sector_t & room_sector);
	void read_tr_room_light(SDL_RWops * const src, tr5_room_light_t & light);
	void read_tr_room_vertex(SDL_RWops * const src, tr5_room_vertex_t & room_vertex);
	void read_tr_room_staticmesh(SDL_RWops * const src, tr2_room_staticmesh_t & room_static_mesh);
	void read_tr_room(SDL_RWops * const src, tr5_room_t & room);
	void read_tr_object_texture_vert(SDL_RWops * const src, tr4_object_texture_vert_t & vert);
	void read_tr_object_texture(SDL_RWops * const src, tr4_object_texture_t & object_texture);
	void read_tr_sprite_texture(SDL_RWops * const src, tr_sprite_texture_t & sprite_texture);
	void read_tr_sprite_sequence(SDL_RWops * const src, tr_sprite_sequence_t & sprite_sequence);
	void read_tr_mesh(SDL_RWops * const src, tr4_mesh_t & mesh);
	void read_tr_animation(SDL_RWops * const src, tr_animation_t & animation);
	void read_tr_meshtree(SDL_RWops * const src, tr_meshtree_t & meshtree);
	void read_tr_frame(SDL_RWops * const src, tr_frame_t & frame, bitu32 num_rotations);
	void read_tr_moveable(SDL_RWops * const src, tr_moveable_t & moveable);
	void read_tr_item(SDL_RWops * const src, tr2_item_t & item);
	void read_tr_staticmesh(SDL_RWops * const src, tr_staticmesh_t & mesh);
	void read_tr_level(SDL_RWops * const src, bool demo_or_ub);

	void read_tr2_colour4(SDL_RWops * const src, tr2_colour_t & colour);
	void read_tr2_palette16(SDL_RWops * const src, tr2_palette_t & palette16);
	void read_tr2_textile16(SDL_RWops * const src, tr2_textile16_t & textile);
	void read_tr2_room_light(SDL_RWops * const src, tr5_room_light_t & light);
	void read_tr2_room_vertex(SDL_RWops * const src, tr5_room_vertex_t & room_vertex);
	void read_tr2_room_staticmesh(SDL_RWops * const src, tr2_room_staticmesh_t & room_static_mesh);
	void read_tr2_room(SDL_RWops * const src, tr5_room_t & room);
	void read_tr2_frame(SDL_RWops * const src, tr_frame_t & frame, bitu32 num_rotations);
	void read_tr2_item(SDL_RWops * const src, tr2_item_t & item);
	void read_tr2_level(SDL_RWops * const src, bool demo);

	void read_tr3_room_vertex(SDL_RWops * const src, tr5_room_vertex_t & room_vertex);
	void read_tr3_room_staticmesh(SDL_RWops * const src, tr2_room_staticmesh_t & room_static_mesh);
	void read_tr3_room(SDL_RWops * const src, tr5_room_t & room);
	void read_tr3_item(SDL_RWops * const src, tr2_item_t & item);
	void read_tr3_level(SDL_RWops * const src);

	void read_tr4_vertex_float(SDL_RWops * const src, tr5_vertex_t & vertex);
	void read_tr4_textile32(SDL_RWops * const src, tr4_textile32_t & textile);
	void read_tr4_face3(SDL_RWops * const src, tr4_face3_t & meshface);
	void read_tr4_face4(SDL_RWops * const src, tr4_face4_t & meshface);
	void read_tr4_room_light(SDL_RWops * const src, tr5_room_light_t & light);
	void read_tr4_room_vertex(SDL_RWops * const src, tr5_room_vertex_t & room_vertex);
	void read_tr4_room(SDL_RWops * const src, tr5_room_t & room);
	void read_tr4_object_texture_vert(SDL_RWops * const src, tr4_object_texture_vert_t & vert);
	void read_tr4_object_texture(SDL_RWops * const src, tr4_object_texture_t & object_texture);
	void read_tr4_mesh(SDL_RWops * const src, tr4_mesh_t & mesh);
	void read_tr4_animation(SDL_RWops * const src, tr_animation_t & animation);
	void read_tr4_level(SDL_RWops * const _src);

	void read_tr5_room_light(SDL_RWops * const src, tr5_room_light_t & light);
	void read_tr5_room_layer(SDL_RWops * const src, tr5_room_layer_t & layer);
	void read_tr5_room_vertex(SDL_RWops * const src, tr5_room_vertex_t & vert);
	void read_tr5_room(SDL_RWops * const orgsrc, tr5_room_t & room);
	void read_tr5_moveable(SDL_RWops * const src, tr_moveable_t & moveable);
	void read_tr5_level(SDL_RWops * const src);
};

#endif // _L_MAIN_H_
