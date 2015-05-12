%module tr_level

%{
#include "SDL.h"
#include "prtl.h"
#include "tr_types.h"
#include "l_main.h"
%}

%include exception.i

%immutable;

%include "glmath.h"

// __getitem__ is required to raise an IndexError for for-loops to work
%exception prtl::array::__getitem__ {
    try {
        $action
    } catch (prtl::prtl_exception &e) {
        SWIG_exception(SWIG_IndexError,const_cast<char*>(e.m_message));
    }
}

namespace prtl {
	class prtl_exception {
	      public:
		char *m_message;	///< \brief takes the pointer to the error string.
		char *m_file;	///< \brief takes the pointer to the file string.
		int m_line;	///< \brief takes the line of the error.
		char *m_rcsid;	///< \brief takes the pointer to the rcsid string.

		prtl_exception(char *const message, char *const file = NULL, int line = 0, char *const rcsid = NULL)
		{
			m_message = message;
			m_file = file;
			m_line = line;
			m_rcsid = rcsid;
		}
	};

	template <class T> class array {
	      public:
		%rename(__len__) size;
		unsigned int size();
		%extend {
			T& __getitem__(int i) {
				int size = int(self->size());
				if (i<0) i += size;
				if (i>=0 && i<size)
					return (*self)[i];
				else
					throw prtl::prtl_exception("array[] out of bounds");
			}
		}
	};
}

%template(bitu8_array_t) prtl::array<bitu8>;
%template(bit16_array_t) prtl::array<bit16>;
%template(bitu16_array_t) prtl::array<bitu16>;
%template(bitu32_array_t) prtl::array<bitu32>;
%template(tr5_vertex_array_t) prtl::array<tr5_vertex_t>;
%template(tr4_face3_array_t) prtl::array<tr4_face3_t>;
%template(tr4_face4_array_t) prtl::array<tr4_face4_t>;
%template(tr_textile8_array_t) prtl::array<tr_textile8_t>;
%template(tr2_textile16_array_t) prtl::array<tr2_textile16_t>;
%template(tr4_textile32_array_t) prtl::array<tr4_textile32_t>;
%template(tr_room_portal_array_t) prtl::array<tr_room_portal_t>;
%template(tr_room_sector_array_t) prtl::array<tr_room_sector_t>;
%template(tr5_room_light_array_t) prtl::array<tr5_room_light_t>;
%template(tr_room_sprite_array_t) prtl::array<tr_room_sprite_t>;
%template(tr5_room_layer_array_t) prtl::array<tr5_room_layer_t>;
%template(tr5_room_vertex_array_t) prtl::array<tr5_room_vertex_t>;
%template(tr2_room_staticmesh_array_t) prtl::array<tr2_room_staticmesh_t>;
%template(tr5_room_array_t) prtl::array<tr5_room_t>;
%template(tr4_mesh_array_t) prtl::array<tr4_mesh_t>;
%template(tr_staticmesh_array_t) prtl::array<tr_staticmesh_t>;
%template(tr_meshtree_array_t) prtl::array<tr_meshtree_t>;
%template(tr_frame_array_t) prtl::array<tr_frame_t>;
%template(tr_moveable_array_t) prtl::array<tr_moveable_t>;
%template(tr2_item_array_t) prtl::array<tr2_item_t>;
%template(tr_sprite_texture_array_t) prtl::array<tr_sprite_texture_t>;
%template(tr_sprite_sequence_array_t) prtl::array<tr_sprite_sequence_t>;
%template(tr_animation_array_t) prtl::array<tr_animation_t>;
%template(tr_state_change_array_t) prtl::array<tr_state_change_t>;
%template(tr_anim_dispatch_array_t) prtl::array<tr_anim_dispatch_t>;
%template(tr_anim_command_array_t) prtl::array<tr_anim_command_t>;
%template(tr_box_array_t) prtl::array<tr_box_t>;
%template(tr_sound_source_array_t) prtl::array<tr_sound_source_t>;
%template(tr_sound_detail_array_t) prtl::array<tr_sound_details_t>;
%template(tr4_object_texture_array_t) prtl::array<tr4_object_texture_t>;
%template(tr_animated_texture_array_t) prtl::array<tr_animated_textures_t>;
%template(tr_camera_array_t) prtl::array<tr_camera_t>;
%template(tr4_flyby_camera_array_t) prtl::array<tr4_flyby_camera_t>;
%template(tr4_ai_object_array_t) prtl::array<tr4_ai_object_t>;
%template(tr_cinematic_frame_array_t) prtl::array<tr_cinematic_frame_t>;

%typemap(out) bitu16 [ANY] {
	int i;
	$result = PyList_New($1_dim0);
	for (i = 0; i < $1_dim0; i++) {
		PyObject *o = PyInt_FromLong((long) $1[i]);
		PyList_SetItem($result,i,o);
	}
}

%include "tr_types.h"

%exception TR_Level::read_level {
    try {
        $action
    } catch (TR_ReadError &e) {
        SWIG_exception(SWIG_IOError,const_cast<char*>(e.m_message));
    }
}

%include "l_main.h"
