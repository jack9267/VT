#ifndef __UTIL_H__
#define __UTIL_H__

int draw_string(int x, int y, float cr, float cg, float cb, float ca, char *fmt, ...);
void init_font(char *filename, const int texture_id);
void draw_box(tr5_vertex_t & c1, tr5_vertex_t & c2, bool closed);
void draw_marker(tr5_vertex_t & v, float size, bool closed);

void infinitePerspective(GLdouble fovy, GLdouble aspect, GLdouble znear);

/// \brief level file info.
typedef struct {
	char *filename;		///< \brief filename relative to the base path.
} level_info_t;

/// \brief Basic game info.
typedef struct {
	char *path;		///< \brief base path of the game.
	tr_version_e version;	///< \brief game engine version.
	level_info_t *levels;	///< \brief list of levels terminated with NULL.
} gamepath_info_t;

extern gamepath_info_t gamepath_info[];

#endif // __UTIL_H__
