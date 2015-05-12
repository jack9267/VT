#ifndef _TR_TYPES_H_
#define _TR_TYPES_H_

#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

#include <vector>
#include "prtl.h"
#include "glmath.h"

typedef signed char bit8;
typedef unsigned char bitu8;
typedef signed short bit16;
typedef unsigned short bitu16;
typedef signed int bit32;
typedef unsigned int bitu32;

typedef prtl::array < bitu8 > bitu8_array_t;
typedef prtl::array < bit16 > bit16_array_t;
typedef prtl::array < bitu16 > bitu16_array_t;
typedef prtl::array < bitu32 > bitu32_array_t;


/// \brief RGBA colour using bitu8. For palette etc.
typedef struct {
	bitu8 r;		///< \brief the red component.
	bitu8 g;		///< \brief the green component.
	bitu8 b;		///< \brief the blue component.
	bitu8 a;		///< \brief the alpha component.
} tr2_colour_t;

/// \brief RGBA colour using float. For single colours.
typedef struct {
	float r;		///< \brief the red component.
	float g;		///< \brief the green component.
	float b;		///< \brief the blue component.
	float a;		///< \brief the alpha component.
} tr5_colour_t;

/// \brief A vertex with x, y and z coordinates.
typedef vec3_t tr5_vertex_t;
typedef prtl::array < tr5_vertex_t > tr5_vertex_array_t;

/// \brief Definition for a triangle.
typedef struct {
	bitu16 vertices[3];	///< index into the appropriate list of vertices.
	bitu16 texture;	 /**< \brief object-texture index or colour index.
			   * If the triangle is textured, then this is an index into the object-texture list.
			   * If it's not textured, then the low 8 bit contain the index into the 256 colour palette
			   * and from TR2 on the high 8 bit contain the index into the 16 bit palette.
			   */
	bitu16 lighting;  /**< \brief transparency flag & strength of the hilight (TR4-TR5).
			    * bit0 if set, then alpha channel = intensity (see attribute in tr2_object_texture).<br>
			    * bit1-7 is the strength of the hilight.
			    */
} tr4_face3_t;
typedef prtl::array < tr4_face3_t > tr4_face3_array_t;

/// \brief Definition for a rectangle.
typedef struct {
	bitu16 vertices[4];	///< index into the appropriate list of vertices.
	bitu16 texture;	 /**< \brief object-texture index or colour index.
			   * If the rectangle is textured, then this is an index into the object-texture list.
			   * If it's not textured, then the low 8 bit contain the index into the 256 colour palette
			   * and from TR2 on the high 8 bit contain the index into the 16 bit palette.
			   */
	bitu16 lighting;  /**< \brief transparency flag & strength of the hilight (TR4-TR5).
			    *
			    * In TR4, objects can exhibit some kind of light reflection when seen from some particular angles.
			    * - bit0 if set, then alpha channel = intensity (see attribute in tr2_object_texture).
			    * - bit1-7 is the strength of the hilight.
			    */
} tr4_face4_t;
typedef prtl::array < tr4_face4_t > tr4_face4_array_t;

/** \brief 8-bit texture.
  *
  * Each pixel is an index into the colour palette.
  */
typedef struct {
	bitu8 pixels[256][256];
} tr_textile8_t;
typedef prtl::array < tr_textile8_t > tr_textile8_array_t;

/** \brief 16-bit texture.
  *
  * Each pixel is a colour with the following format.<br>
  * - 1-bit transparency (0 ::= transparent, 1 ::= opaque) (0x8000)
  * - 5-bit red channel (0x7c00)
  * - 5-bit green channel (0x03e0)
  * - 5-bit blue channel (0x001f)
  */
typedef struct {
	bitu16 pixels[256][256];
} tr2_textile16_t;
typedef prtl::array < tr2_textile16_t > tr2_textile16_array_t;

/** \brief 32-bit texture.
  *
  * Each pixel is an ABGR value.
  */
typedef struct {
	bitu32 pixels[256][256];
} tr4_textile32_t;
typedef prtl::array < tr4_textile32_t > tr4_textile32_array_t;

/** \brief Room portal.
  */
typedef struct {
	bitu16 adjoining_room;	///< \brief which room this portal leads to.
	tr5_vertex_t normal;		/**< \brief which way the portal faces.
					  * the normal points away from the adjacent room.
					  * to be seen through, it must point toward the viewpoint.
					  */
	tr5_vertex_t vertices[4];	/**< \brief the corners of this portal.
					  * the right-hand rule applies with respect to the normal.
					  * if the right-hand-rule is not followed, the portal will
					  * contain visual artifacts instead of a viewport to
					  * AdjoiningRoom.
					  */
} tr_room_portal_t;
typedef prtl::array < tr_room_portal_t > tr_room_portal_array_t;

/** \brief Room sector.
  */
typedef struct {
	bitu16 fd_index;	// Index into FloorData[]
	bitu16 box_index;	// Index into Boxes[]/Zones[] (-1 if none)
	bitu8 room_below;	// The number of the room below this one (-1 or 255 if none)
	bit8 floor;		// Absolute height of floor (multiply by 256 for world coordinates)
	bitu8 room_above;	// The number of the room above this one (-1 or 255 if none)
	bit8 ceiling;		// Absolute height of ceiling (multiply by 256 for world coordinates)
} tr_room_sector_t;
typedef prtl::array < tr_room_sector_t > tr_room_sector_array_t;

/** \brief Room light.
  */
typedef struct {
	tr5_vertex_t pos;	// world coords
	tr2_colour_t color;	// three bytes rgb values 
	bitu16 intensity1;	// Light intensity
	bitu16 intensity2;	// Almost always equal to Intensity1 [absent from TR1 data files]
	bitu32 fade1;		// Falloff value 1
	bitu32 fade2;		// Falloff value 2 [absent from TR1 data files]
	bitu8 light_type;	// same as D3D (i.e. 2 is for spotlight) 
	bitu8 unknown;		// always 0xff? 
	float r_inner;
	float r_outer;
	float length;
	float cutoff;
	tr5_vertex_t dir;	// direction
	tr5_vertex_t pos2;	// world coords
	tr5_vertex_t dir2;	// direction
} tr5_room_light_t;
typedef prtl::array < tr5_room_light_t > tr5_room_light_array_t;

/** \brief Room sprite.
  */
typedef struct {
	bit16 vertex;		// offset into vertex list
	bit16 texture;		// offset into sprite texture list
} tr_room_sprite_t;
typedef prtl::array < tr_room_sprite_t > tr_room_sprite_array_t;

/** \brief Room layer (TR5).
  */
typedef struct {
	bitu16 num_vertices;	// number of vertices in this layer (4 bytes)
	bitu16 unknown_l1;
	bitu16 unknown_l2;
	bitu16 num_rectangles;	// number of rectangles in this layer (2 bytes)
	bitu16 num_triangles;	// number of triangles in this layer (2 bytes)
	bitu16 unknown_l3;
	bitu16 unknown_l4;
	//  The following 6 floats (4 bytes each) define the bounding box for the layer
	float bounding_box_x1;
	float bounding_box_y1;
	float bounding_box_z1;
	float bounding_box_x2;
	float bounding_box_y2;
	float bounding_box_z2;
	bit16 unknown_l6a;
	bit16 unknown_l6b;
	bit16 unknown_l7a;
	bit16 unknown_l7b;
	bit16 unknown_l8a;
	bit16 unknown_l8b;
} tr5_room_layer_t;
typedef prtl::array < tr5_room_layer_t > tr5_room_layer_array_t;

/** \brief Room vertex.
  */
typedef struct {
	tr5_vertex_t vertex;	// where this vertex lies (relative to tr2_room_info::x/z)
	bit16 lighting1;
	bitu16 attributes;	// A set of flags for special rendering effects [absent from TR1 data files]
	// 0x8000 something to do with water surface
	// 0x4000 under water lighting modulation and
	// movement if viewed from above water surface
	// 0x2000 water/quicksand surface movement
	// 0x0010 "normal"
	bit16 lighting2;	// Almost always equal to Lighting1 [absent from TR1 data files]
	// TR5 -->
	tr5_vertex_t normal;
	tr5_colour_t colour;	// vertex color ARGB format (4 bytes)
} tr5_room_vertex_t;
typedef prtl::array < tr5_room_vertex_t > tr5_room_vertex_array_t;

/** \brief Room staticmesh.
  */
typedef struct {
	tr5_vertex_t pos;	// world coords
	float rotation;		// high two bits (0xC000) indicate steps of
	// 90 degrees (e.g. (Rotation >> 14) * 90)
	bit16 intensity1;	// Constant lighting; -1 means use mesh lighting
	bit16 intensity2;	// Like Intensity 1, and almost always the same value [absent from TR1 data files]
	bitu16 object_id;	// which StaticMesh item to draw
} tr2_room_staticmesh_t;
typedef prtl::array < tr2_room_staticmesh_t > tr2_room_staticmesh_array_t;

/** \brief Room.
  */
typedef struct {
	tr5_vertex_t offset;	///< \brief offset of room (world coordinates).
	float y_bottom;		///< \brief indicates lowest point in room.
	float y_top;		///< \brief indicates highest point in room.
	bitu32 num_layers;	// number of layers (pieces) this room (4 bytes)
	tr5_room_layer_array_t layers;	// [NumStaticMeshes]list of static meshes
	bitu32 num_vertices;	// number of vertices in the following list
	tr5_room_vertex_array_t vertices;	// [NumVertices] list of vertices (relative coordinates)
	bitu32 num_rectangles;	// number of textured rectangles
	tr4_face4_array_t rectangles;	// [NumRectangles] list of textured rectangles
	bitu32 num_triangles;	// number of textured triangles
	tr4_face3_array_t triangles;	// [NumTriangles] list of textured triangles
	bitu32 num_sprites;	// number of sprites
	tr_room_sprite_array_t sprites;	// [NumSprites] list of sprites
	bitu16 num_portals;	// number of visibility portals to other rooms
	tr_room_portal_array_t portals;	// [NumPortals] list of visibility portals
	bitu16 num_zsectors;	// "width" of sector list
	bitu16 num_xsectors;	// "height" of sector list
	tr_room_sector_array_t sector_list;	// [NumXsectors * NumZsectors] list of sectors
	// in this room
	bit16 intensity1;	// This and the next one only affect externally-lit objects
	bit16 intensity2;	// Almost always the same value as AmbientIntensity1 [absent from TR1 data files]
	bit16 light_mode;	// (present only in TR2: 0 is normal, 1 is flickering(?), 2 and 3 are uncertain)
	bitu16 num_lights;	// number of point lights in this room
	tr5_room_light_array_t lights;	// [NumLights] list of point lights
	bitu16 num_static_meshes;	// number of static meshes
	tr2_room_staticmesh_array_t static_meshes;	// [NumStaticMeshes]list of static meshes
	bit16 alternate_room;	// number of the room that this room can alternate
	// with (e.g. empty/filled with water is implemented as an empty room that alternates with a full room)
	bitu16 flags;
	// flag bits: 0x0001 - room is filled with water,
	// 0x0020 - Lara's ponytail gets blown
	// by the wind;
	// TR1 has only the water flag and the extra
	// unknown flag 0x0100.
	// TR3 most likely has flags for "is raining", "is snowing", "water is cold", and "is
	// filled by quicksand", among others.
	tr2_colour_t fog_colour;	// Present in TR3-TR4 only
	tr5_colour_t light_colour;	// Present in TR5 only

	// TR5 only -->
	bitu16 unknown_r1;
	bitu16 unknown_r2;
	bit16 unknown_r3;
	bitu32 unknown_r4;
	bitu32 unknown_r5;
	bitu32 unknown_r6;
	float room_x;
	float room_z;
	bitu16 unknown_r7a;
	bitu16 unknown_r7b;
	bitu32 unknown_r8;
	bitu32 unknown_r9;
	float room_y_bottom;
	float room_y_top;
} tr5_room_t;
typedef prtl::array < tr5_room_t > tr5_room_array_t;

/** \brief Mesh.
  */
typedef struct {
	tr5_vertex_t centre;	// This is usually close to the mesh's centroid, and appears to be the center of a sphere used for collision testing.
	bit32 collision_size;	// This appears to be the radius of that aforementioned collisional sphere.
	bit16 num_vertices;	// number of vertices in this mesh
	tr5_vertex_array_t vertices;	//[NumVertices]; // list of vertices (relative coordinates)
	bit16 num_normals;	// If positive, number of normals in this mesh.
	// If negative, number of vertex lighting elements (* (-1))
	bit16 num_lights;	// Engine internal for above
	tr5_vertex_array_t normals;	//[NumNormals]; // list of normals (if NumNormals is positive)
	bit16_array_t lights;	//[-NumNormals]; // list of light values (if NumNormals is negative)
	bit16 num_textured_rectangles;	// number of textured rectangles in this mesh
	tr4_face4_array_t textured_rectangles;	//[NumTexturedRectangles]; // list of textured rectangles
	bit16 num_textured_triangles;	// number of textured triangles in this mesh
	tr4_face3_array_t textured_triangles;	//[NumTexturedTriangles]; // list of textured triangles
	// the rest is not present in TR4
	bit16 num_coloured_rectangles;	// number of coloured rectangles in this mesh
	tr4_face4_array_t coloured_rectangles;	//[NumColouredRectangles]; // list of coloured rectangles
	bit16 num_coloured_triangles;	// number of coloured triangles in this mesh
	tr4_face3_array_t coloured_triangles;	//[NumColouredTriangles]; // list of coloured triangles
} tr4_mesh_t;
typedef prtl::array < tr4_mesh_t > tr4_mesh_array_t;

/** \brief Staticmesh.
  */
typedef struct {		// 32 bytes
	bitu32 object_id;	// Object Identifier (matched in Items[])
	bitu16 mesh;		// mesh (offset into MeshPointers[])
	tr5_vertex_t visibility_box[2];
	tr5_vertex_t collision_box[2];
	bitu16 flags;		// Meaning uncertain; it is usually 2, and is 3 for objects Lara can travel through,
	// like TR2's skeletons and underwater vegetation
} tr_staticmesh_t;
typedef prtl::array < tr_staticmesh_t > tr_staticmesh_array_t;

/** \brief MeshTree.
  *
  * MeshTree[] is actually groups of four bit32s. The first one is a
  * "flags" word;
  *    bit 1 (0x0002) indicates "put the parent mesh on the mesh stack";
  *    bit 0 (0x0001) indicates "take the top mesh off of the mesh stack and use as the parent mesh"
  * when set, otherwise "use the previous mesh are the parent mesh".
  * When both are present, the bit-0 operation is always done before the bit-1 operation; in effect, read the stack but do not change it.
  * The next three bit32s are X, Y, Z offsets of the mesh's origin from the parent mesh's origin.
  */
typedef struct {		// 4 bytes
	bitu32 flags;
	tr5_vertex_t offset;
} tr_meshtree_t;
typedef prtl::array < tr_meshtree_t > tr_meshtree_array_t;

/** \brief Frame.
  *
  * Frames indicates how composite meshes are positioned and rotated.  
  * They work in conjunction with Animations[] and Bone2[].
  *  
  * A given frame has the following format:
  *    short BB1x, BB1y, BB1z           // bounding box (low)
  *    short BB2x, BB2y, BB2z           // bounding box (high)
  *    short OffsetX, OffsetY, OffsetZ  // starting offset for this moveable
  *    (TR1 ONLY: short NumValues       // number of angle sets to follow)
  *    (TR2/3: NumValues is implicitly NumMeshes (from moveable))
  *   
  * What follows next is a list of angle sets.  In TR2/3, an angle set can
  * specify either one or three axes of rotation.  If either of the high two
  * bits (0xc000) of the first angle unsigned short are set, it's one axis:
  *  only one  unsigned short, 
  *  low 10 bits (0x03ff), 
  *  scale is 0x100 == 90 degrees;  
  * the high two  bits are interpreted as follows:  
  *  0x4000 == X only, 0x8000 == Y only,
  *  0xC000 == Z only.
  *   
  * If neither of the high bits are set, it's a three-axis rotation.  The next
  * 10 bits (0x3ff0) are the X rotation, the next 10 (including the following
  * unsigned short) (0x000f, 0xfc00) are the Y rotation, 
  * the next 10 (0x03ff) are the Z rotation, same scale as 
  * before (0x100 == 90 degrees).
  *
  * Rotations are performed in Y, X, Z order.
  * TR1 ONLY: All angle sets are two words and interpreted like the two-word
  * sets in TR2/3, EXCEPT that the word order is reversed.
  *
  */
typedef struct {
	tr5_vertex_t bbox_low;
	tr5_vertex_t bbox_high;
	tr5_vertex_t offset;
	tr5_vertex_array_t rotations;
	bit32 byte_offset;
} tr_frame_t;
typedef prtl::array < tr_frame_t > tr_frame_array_t;

/** \brief Moveable.
  */
typedef struct {		// 18 bytes
	bitu32 object_id;	// Item Identifier (matched in Items[])
	bitu16 num_meshes;	// number of meshes in this object
	bitu16 starting_mesh;	// stating mesh (offset into MeshPointers[])
	bitu32 mesh_tree_index;	// offset into MeshTree[]
	bitu32 frame_offset;	// byte offset into Frames[] (divide by 2 for Frames[i])
	bitu32 frame_index;
	bitu16 animation_index;	// offset into Animations[]
} tr_moveable_t;
typedef prtl::array < tr_moveable_t > tr_moveable_array_t;

/** \brief Item.
  */
typedef struct {		// 24 bytes [TR1: 22 bytes]
	bit16 object_id;	// Object Identifier (matched in Moveables[], or SpriteSequences[], as appropriate)
	bit16 room;		// which room contains this item
	tr5_vertex_t pos;	// world coords
	float rotation;		// ((0xc000 >> 14) * 90) degrees
	bit16 intensity1;	// (constant lighting; -1 means use mesh lighting)
	bit16 intensity2;	// Like Intensity1, and almost always with the same value. [absent from TR1 data files]
	bitu16 flags;		// 0x0100 indicates "initially invisible", 0x3e00 is Activation Mask
	// 0x3e00 indicates "open" or "activated";  these can be XORed with
	// related FloorData::FDlist fields (e.g. for switches)
} tr2_item_t;
typedef prtl::array < tr2_item_t > tr2_item_array_t;

/** \brief Sprite texture.
  */
typedef struct {		// 16 bytes
	bitu16 tile;
	bitu8 x;		// TR4 unknown
	bitu8 y;		// TR4 unknown
	bitu16 width;		// TR1/2/3 actually (Width * 256) + 255
	// TR4 = (real_width-1) * 256   
	bitu16 height;		// TR1/2/3 actually (Height * 256) + 255
	// TR4 = (real_height-1) * 256  
	bit16 left_side;
	bit16 top_side;
	bit16 right_side;
	bit16 bottom_side;
} tr_sprite_texture_t;
typedef prtl::array < tr_sprite_texture_t > tr_sprite_texture_array_t;

/** \brief Sprite sequence.
  */
typedef struct {		// 8 bytes
	bit32 object_id;	// Item identifier (matched in Items[])
	bit16 length;		// negative of "how many sprites are in this sequence"
	bit16 offset;		// where (in sprite texture list) this sequence starts
} tr_sprite_sequence_t;
typedef prtl::array < tr_sprite_sequence_t > tr_sprite_sequence_array_t;

/** \brief Animation.
  *
  * This describes each individual animation; these may be looped by specifying
  * the next animation to be itself. In TR2 and TR3, one must be careful when
  * parsing frames using the FrameSize value as the size of each frame, since
  * an animation's frame range may extend into the next animation's frame range,
  * and that may have a different FrameSize value.
  */
typedef struct {		// 32 bytes TR1/2/3 40 bytes TR4
	bitu32 frame_offset;	// byte offset into Frames[] (divide by 2 for Frames[i])
	bitu8 frame_rate;	// Engine ticks per frame
	bitu8 frame_size;	// number of bit16's in Frames[] used by this animation
	bitu16 state_id;

	bit16 unknown;
	bit16 speed;
	bit16 accel_lo;
	bit16 accel_hi;

	bit16 unknown2;		// new in TR4 -->
	bit16 speed2;		// not really sure what these mean.
	bit16 accel_lo2;
	bit16 accel_hi2;	// <-- TR4

	bitu16 frame_start;	// first frame in this animation
	bitu16 frame_end;	// last frame in this animation (numframes = (End - Start) + 1)
	bitu16 next_animation;
	bitu16 next_frame;

	bitu16 num_state_changes;
	bitu16 state_change_offset;	// offset into StateChanges[]
	bitu16 num_anim_commands;	// How many of them to use.
	bitu16 anim_command;	// offset into AnimCommand[]
} tr_animation_t;
typedef prtl::array < tr_animation_t > tr_animation_array_t;

/** \brief State Change.
  *
  * Each one contains the state to change to and which animation dispatches
  * to use; there may be more than one, with each separate one covering a different
  * range of frames.
  */
typedef struct {		// 6 bytes
	bitu16 state_id;
	bitu16 num_anim_dispatches;	// number of ranges (seems to always be 1..5)
	bitu16 anim_dispatch;	// Offset into AnimDispatches[]
} tr_state_change_t;
typedef prtl::array < tr_state_change_t > tr_state_change_array_t;

/** \brief Animation Dispatch.
  *
  * This specifies the next animation and frame to use; these are associated
  * with some range of frames. This makes possible such specificity as one
  * animation for left foot forward and another animation for right foot forward.
  */
typedef struct {		// 8 bytes
	bit16 low;		// Lowest frame that uses this range
	bit16 high;		// Highest frame (+1?) that uses this range
	bit16 next_animation;	// Animation to dispatch to
	bit16 next_frame;	// Frame offset to dispatch to
} tr_anim_dispatch_t;
typedef prtl::array < tr_anim_dispatch_t > tr_anim_dispatch_array_t;

/** \brief Animation Command.
  *
  * These are various commands associated with each animation; they are
  * called "Bone1" in some documentation. They are varying numbers of bit16's
  * packed into an array; the first of each set is the opcode, which determines
  * how operand bit16's follow it. Some of them refer to the whole animation
  * (jump and grab points, etc.), while others of them are associated with
  * specific frames (sound, bubbles, etc.).
  */
typedef struct {		// 2 bytes
	bit16 value;
} tr_anim_command_t;
typedef prtl::array < tr_anim_command_t > tr_anim_command_array_t;

/** \brief Box.
  */
typedef struct {		// 8 bytes [TR1: 20 bytes] In TR1, the first four are bit32's instead of bitu8's, and are not scaled.
	bitu32 zmin;		// sectors (* 1024 units)
	bitu32 zmax;
	bitu32 xmin;
	bitu32 xmax;
	bit16 true_floor;	// Y value (no scaling)
	bit16 overlap_index;	// index into Overlaps[]. The high bit is sometimes set; this
	// occurs in front of swinging doors and the like.
} tr_box_t;
typedef prtl::array < tr_box_t > tr_box_array_t;

/** \brief SoundSource.
  *
  * This structure contains the details of continuous-sound sources. Although
  * a SoundSource object has a position, it has no room membership; the sound
  * seems to propagate omnidirectionally for about 10 horizontal-grid sizes
  * without regard for the presence of walls.
  */
typedef struct {
	bit32 x;		// absolute X position of sound source (world coordinates)
	bit32 y;		// absolute Y position of sound source (world coordinates)
	bit32 z;		// absolute Z position of sound source (world coordinates)
	bitu16 sound_id;	// internal sound index
	bitu16 flags;		// 0x40, 0x80, or 0xc0
} tr_sound_source_t;
typedef prtl::array < tr_sound_source_t > tr_sound_source_array_t;

/** \brief SoundDetails.
 */
typedef struct {		// 8 bytes
	bit16 sample;		// (index into SampleIndices)
	bit16 volume;
	bit16 sound_range;	// sound range? (distance at which this sound can be heard?)
	bit16 flags;		// Bits 8-15: priority?, Bits 2-7: number of sound
	// samples in this group, Bits 0-1: channel number?
} tr_sound_details_t;
typedef prtl::array < tr_sound_details_t > tr_sound_detail_array_t;

/** \brief Object Texture Vertex.
  *
  * It specifies a vertex location in textile coordinates.
  * The Xpixel and Ypixel are the actual coordinates of the vertex's pixel.
  * The Xcoordinate and Ycoordinate values depend on where the other vertices
  * are in the object texture. And if the object texture is used to specify
  * a triangle, then the fourth vertex's values will all be zero.
  */
typedef struct {		// 4 bytes
	bit8 xcoordinate;	// 1 if Xpixel is the low value, -1 if Xpixel is the high value in the object texture
	bitu8 xpixel;
	bit8 ycoordinate;	// 1 if Ypixel is the low value, -1 if Ypixel is the high value in the object texture
	bitu8 ypixel;
} tr4_object_texture_vert_t;

/** \brief Object Texture.
  *
  * These, thee contents of ObjectTextures[], are used for specifying texture
  * mapping for the world geometry and for mesh objects.
  */
typedef struct {		// 38 bytes TR4 - 20 in TR1/2/3
	bitu16 transparency_flags;	// 0 means that a texture is all-opaque, and that transparency
	// information is ignored.
	// 1 means that transparency information is used. In 8-bit colour,
	// index 0 is the transparent colour, while in 16-bit colour, the
	// top bit (0x8000) is the alpha channel (1 = opaque, 0 = transparent).
	// 2 (only in TR3) means that the opacity (alpha) is equal to the intensity;
	// the brighter the colour, the more opaque it is. The intensity is probably calculated
	// as the maximum of the individual color values.
	bitu8 tile;		// index into textile list
	bitu8 tile_flags;
	bitu16 flags;		// TR4
	tr4_object_texture_vert_t vertices[4];	// the four corners of the texture
	bitu32 unknown1;	// TR4
	bitu32 unknown2;	// TR4
	bitu32 x_size;		// TR4
	bitu32 y_size;		// TR4
} tr4_object_texture_t;
typedef prtl::array < tr4_object_texture_t > tr4_object_texture_array_t;

/** \brief Animated Textures.
  */
typedef struct {
	bit16 num_texture_ids;	// Actually, this is the number of texture ID's - 1.
	bit16_array_t texture_ids;	//[NumTextureIDs + 1]; // offsets into ObjectTextures[], in animation order.
} tr_animated_textures_t;	//[NumAnimatedTextures];
typedef prtl::array < tr_animated_textures_t > tr_animated_texture_array_t;

/** \brief Camera.
  */
typedef struct {
	bit32 x;
	bit32 y;
	bit32 z;
	bit16 room;
	bitu16 unknown1;	// correlates to Boxes[]? Zones[]?
} tr_camera_t;
typedef prtl::array < tr_camera_t > tr_camera_array_t;

/** \brief Extra Camera.
  */
typedef struct {
	bit32 x1;
	bit32 y1;
	bit32 z1;
	bit32 x2;
	bit32 y2;
	bitu8 index1;
	bitu8 index2;
	bitu16 unknown[5];
	bit32 id;
} tr4_flyby_camera_t;
typedef prtl::array < tr4_flyby_camera_t > tr4_flyby_camera_array_t;

/** \brief AI Object.
  */
typedef struct {
	bitu16 object_id;	// the objectID from the AI object (AI_FOLLOW is 402)
	bitu16 room;
	bit32 x;
	bit32 y;
	bit32 z;
	bitu16 ocb;
	bitu16 flags;		// The trigger flags (button 1-5, first button has value 2)
	bit32 angle;
} tr4_ai_object_t;
typedef prtl::array < tr4_ai_object_t > tr4_ai_object_array_t;

/** \brief Cinematic Frame.
  */
typedef struct {
	bit16 roty;		// rotation about Y axis, +/- 32767 == +/- 180 degrees
	bit16 rotz;		// rotation about Z axis, +/- 32767 == +/- 180 degrees
	bit16 rotz2;		// seems to work a lot like rotZ;  I haven't yet been able to
	// differentiate them
	bit16 posz;		// camera position relative to something (target? Lara? room
	// origin?).  pos* are _not_ in world coordinates.
	bit16 posy;		// camera position relative to something (see posZ)
	bit16 posx;		// camera position relative to something (see posZ)
	bit16 unknown;		// changing this can cause a runtime error
	bit16 rotx;		// rotation about X axis, +/- 32767 == +/- 180 degrees
} tr_cinematic_frame_t;
typedef prtl::array < tr_cinematic_frame_t > tr_cinematic_frame_array_t;

/** \brief Lightmap.
  */
typedef struct {
	bitu8 map[32 * 256];
} tr_lightmap_t;

/** \brief Palette.
  */
typedef struct {
	tr2_colour_t colour[256];
} tr2_palette_t;

#endif // _TR_TYPES_H_
