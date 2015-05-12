#include <math.h>
#include <malloc.h>
 #include "SDL.h"
#include "dyngl.h"
#include "l_main.h"
#include "glmath.h"
#include "util.h"

#define RCSID "$Id: util.cpp,v 1.3 2002/09/20 15:59:02 crow Exp $"

const int CHAR_WIDTH = 12;
const int CHAR_HEIGHT = 16;

static int font_texture_id = 0;

int draw_string(int x, int y, float cr, float cg, float cb, float ca, char *fmt, ...)
{
	va_list v;
	int result;
	int r = 0, c = 0;
	char buffer[2048];
	char *s = buffer;

	va_start(v, fmt);
	result = vsprintf(buffer, fmt, v);
	va_end(v);
	qglBindTexture(GL_TEXTURE_2D, font_texture_id);
	qglColor4f(cr, cg, cb, ca);
	qglBegin(GL_QUADS);
	while (*s) {
		float ts, tu;

		if (*s == '\n') {
			s++;
			c++;
			r = 0;
			continue;
		}
		ts = (float)((*s - 1) % 16) * (CHAR_WIDTH / 256.0f);
		tu = (float)((*s - 1) / 16) * (CHAR_HEIGHT / 256.0f);
		qglTexCoord2f(ts, tu);
		qglVertex2f((float)((x + 0) + (r * CHAR_WIDTH)), (float)((y + 0) + (c * CHAR_HEIGHT)));
		qglTexCoord2f(ts + (CHAR_WIDTH / 256.0f), tu);
		qglVertex2f((float)((x + CHAR_WIDTH) + (r * CHAR_WIDTH)), (float)((y + 0) + (c * CHAR_HEIGHT)));
		qglTexCoord2f(ts + (CHAR_WIDTH / 256.0f), tu + (CHAR_HEIGHT / 256.0f));
		qglVertex2f((float)((x + CHAR_WIDTH) + (r * CHAR_WIDTH)), (float)((y + CHAR_HEIGHT) + (c * CHAR_HEIGHT)));
		qglTexCoord2f(ts, tu + (CHAR_HEIGHT / 256.0f));
		qglVertex2f((float)((x + 0) + (r * CHAR_WIDTH)), (float)((y + CHAR_HEIGHT) + (c * CHAR_HEIGHT)));
		s++;
		r++;
	}
	qglEnd();

	return result;
}

void init_font(char *filename, const int texture_id)
{
	SDL_Surface *font;
	Uint32 *pixels;
	int x, y;

	font = SDL_LoadBMP(filename);
	if (!font)
		return;

	pixels = (Uint32 *) malloc(256 * 256 * 4);
	if (!pixels) {
		SDL_FreeSurface(font);
		return;
	}

	memset(pixels, 0, 256 * 256 * 4);

	for (y = 0; y < font->h; y++)
		for (x = 0; x < font->w; x++)
			if (((Uint8 *) font->pixels)[y * font->pitch + x])
				pixels[y * 256 + x] = 0xffffffff;

	SDL_FreeSurface(font);

	font_texture_id = texture_id;
	qglBindTexture(GL_TEXTURE_2D, font_texture_id);
	qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	free(pixels);
}

void draw_box(tr5_vertex_t & c1, tr5_vertex_t & c2, bool closed)
{
	GLenum mode;

	if (closed)
		mode = GL_QUADS;
	else
		mode = GL_LINE_LOOP;
	qglBegin(mode);
	qglVertex3f(c1.x, c1.y, c1.z);
	qglVertex3f(c2.x, c1.y, c1.z);
	qglVertex3f(c2.x, c2.y, c1.z);
	qglVertex3f(c1.x, c2.y, c1.z);
	qglEnd();

	qglBegin(mode);
	qglVertex3f(c1.x, c1.y, c2.z);
	qglVertex3f(c1.x, c2.y, c2.z);
	qglVertex3f(c2.x, c2.y, c2.z);
	qglVertex3f(c2.x, c1.y, c2.z);
	qglEnd();

	qglBegin(mode);
	qglVertex3f(c1.x, c1.y, c1.z);
	qglVertex3f(c1.x, c2.y, c1.z);
	qglVertex3f(c1.x, c2.y, c2.z);
	qglVertex3f(c1.x, c1.y, c2.z);
	qglEnd();

	qglBegin(mode);
	qglVertex3f(c2.x, c1.y, c1.z);
	qglVertex3f(c2.x, c1.y, c2.z);
	qglVertex3f(c2.x, c2.y, c2.z);
	qglVertex3f(c2.x, c2.y, c1.z);
	qglEnd();

	qglBegin(mode);
	qglVertex3f(c1.x, c1.y, c1.z);
	qglVertex3f(c1.x, c1.y, c2.z);
	qglVertex3f(c2.x, c1.y, c2.z);
	qglVertex3f(c2.x, c1.y, c1.z);
	qglEnd();

	qglBegin(mode);
	qglVertex3f(c1.x, c2.y, c1.z);
	qglVertex3f(c2.x, c2.y, c1.z);
	qglVertex3f(c2.x, c2.y, c2.z);
	qglVertex3f(c1.x, c2.y, c2.z);
	qglEnd();
}

void draw_marker(tr5_vertex_t & v, float size, bool closed)
{
	tr5_vertex_t c1;
	tr5_vertex_t c2;

	c1.x = v.x - size;
	c1.y = v.y - size;
	c1.z = v.z - size;

	c2.x = v.x + size;
	c2.y = v.y + size;
	c2.z = v.z + size;

	if (closed) {
		draw_box(c1, c2, true);
	} else {
		qglBegin(GL_LINES);
		qglVertex3f(c2.x, c2.y, c2.z);
		qglVertex3f(c1.x, c1.y, c1.z);

		qglVertex3f(c2.x, c2.y, c1.z);
		qglVertex3f(c1.x, c1.y, c2.z);

		qglVertex3f(c1.x, c2.y, c2.z);
		qglVertex3f(c2.x, c1.y, c1.z);

		qglVertex3f(c2.x, c1.y, c2.z);
		qglVertex3f(c1.x, c2.y, c1.z);
		qglEnd();
	}
}

static level_info_t tr1_levels[] = {
	{"DATA/TITLE.PHD"},
	{"DATA/CUT1.PHD"},
	{"DATA/CUT2.PHD"},
	{"DATA/CUT3.PHD"},
	{"DATA/CUT4.PHD"},
	// 5
	{"DATA/GYM.PHD"},
	{"DATA/LEVEL1.PHD"},
	{"DATA/LEVEL10A.PHD"},
	{"DATA/LEVEL10B.PHD"},
	{"DATA/LEVEL10C.PHD"},
	// 10
	{"DATA/LEVEL2.PHD"},
	{"DATA/LEVEL3A.PHD"},
	{"DATA/LEVEL3B.PHD"},
	{"DATA/LEVEL4.PHD"},
	{"DATA/LEVEL5.PHD"},
	// 15
	{"DATA/LEVEL6.PHD"},
	{"DATA/LEVEL7A.PHD"},
	{"DATA/LEVEL7B.PHD"},
	{"DATA/LEVEL8A.PHD"},
	{"DATA/LEVEL8B.PHD"},
	// 20
	{"DATA/LEVEL8C.PHD"},
	{NULL}
};

static level_info_t tr1_demo1_levels[] = {
	{"DATA/TITLE.PHD"},
	{"DATA/LEVEL2.PHD"},
	{NULL}
};

static level_info_t tr1_demo2_levels[] = {
	{"DATA/TITLE.PHD"},
	{"DATA/LEVEL2.PHD"},
	{NULL}
};

static level_info_t tr1_gold_levels[] = {
	{"DATA/CAT.TUB"},
	{"DATA/EGYPT.TUB"},
	{"DATA/END.TUB"},
	{"DATA/END2.TUB"},
	{NULL}
};

static level_info_t tr2_levels[] = {
	{"DATA/TITLE.TR2"},
	{"DATA/ASSAULT.TR2"},
	{"DATA/BOAT.TR2"},
	{"DATA/CATACOMB.TR2"},
	{"DATA/CUT1.TR2"},
	{"DATA/CUT2.TR2"},
	{"DATA/CUT3.TR2"},
	{"DATA/CUT4.TR2"},
	{"DATA/DECK.TR2"},
	{"DATA/EMPRTOMB.TR2"},
	{"DATA/FLOATING.TR2"},
	{"DATA/HOUSE.TR2"},
	{"DATA/ICECAVE.TR2"},
	{"DATA/KEEL.TR2"},
	{"DATA/LIVING.TR2"},
	{"DATA/MONASTRY.TR2"},
	{"DATA/OPERA.TR2"},
	{"DATA/PLATFORM.TR2"},
	{"DATA/RIG.TR2"},
	{"DATA/SKIDOO.TR2"},
	{"DATA/UNWATER.TR2"},
	{"DATA/VENICE.TR2"},
	{"DATA/WALL.TR2"},
	{"DATA/XIAN.TR2"},
	{NULL}
};

static level_info_t tr2_demo_levels[] = {
	{"DATA/TITLE.TR2"},
	{"DATA/BOAT.TR2"},
	{NULL}
};

static level_info_t tr2_gold_levels[] = {
	{"DATA/TITLE.TR2"},
	{"DATA/LEVEL1.TR2"},
	{"DATA/LEVEL2.TR2"},
	{"DATA/LEVEL3.TR2"},
	{"DATA/LEVEL4.TR2"},
	{"DATA/LEVEL5.TR2"},
	{NULL}
};

static level_info_t tr3_levels[] = {
	{"DATA/TITLE.TR2"},
	{"DATA/ANTARC.TR2"},
	{"DATA/AREA51.TR2"},
	{"DATA/CHAMBER.TR2"},
	{"DATA/CITY.TR2"},
	{"DATA/COMPOUND.TR2"},
	{"DATA/CRASH.TR2"},
	{"DATA/HOUSE.TR2"},
	{"DATA/JUNGLE.TR2"},
	{"DATA/MINES.TR2"},
	{"DATA/NEVADA.TR2"},
	{"DATA/OFFICE.TR2"},
	{"DATA/QUADCHAS.TR2"},
	{"DATA/RAPIDS.TR2"},
	{"DATA/ROOFS.TR2"},
	{"DATA/SEWER.TR2"},
	{"DATA/SHORE.TR2"},
	{"DATA/STPAUL.TR2"},
	{"DATA/TEMPLE.TR2"},
	{"DATA/TONYBOSS.TR2"},
	{"DATA/TOWER.TR2"},
	{"DATA/TRIBOSS.TR2"},
	{"DATA/VICT.TR2"},
	{NULL}
};

static level_info_t tr3_demo1_levels[] = {
	{"DATA/TITLE.TR2"},
	{"DATA/JUNGLE.TR2"},
	{NULL}
};

static level_info_t tr3_demo2_levels[] = {
	{"DATA/TITLE.TR2"},
	{"DATA/SHORE.TR2"},
	{NULL}
};

static level_info_t tr4_levels[] = {
	{"DATA/TITLE.TR4"},
	{"DATA/ALEXHUB.TR4"},
	{"DATA/ALEXHUB2.TR4"},
	{"DATA/ANG_RACE.TR4"},
	{"DATA/ANGKOR1.TR4"},
	{"DATA/BIKEBIT.TR4"},
	{"DATA/CITNEW.TR4"},
	{"DATA/CORTYARD.TR4"},
	{"DATA/CSPLIT1.TR4"},
	{"DATA/CSPLIT2.TR4"},
	{"DATA/HALL.TR4"},
	{"DATA/HIGHSTRT.TR4"},
	{"DATA/JEEPCHAS.TR4"},
	{"DATA/JEEPCHS2.TR4"},
	{"DATA/JOBY1A.TR4"},
	{"DATA/JOBY1B.TR4"},
	{"DATA/JOBY2.TR4"},
	{"DATA/JOBY3A.TR4"},
	{"DATA/JOBY3B.TR4"},
	{"DATA/JOBY4A.TR4"},
	{"DATA/JOBY4B.TR4"},
	{"DATA/JOBY4C.TR4"},
	{"DATA/JOBY5A.TR4"},
	{"DATA/JOBY5B.TR4"},
	{"DATA/JOBY5C.TR4"},
	{"DATA/KARNAK1.TR4"},
	{"DATA/LAKE.TR4"},
	{"DATA/LIBEND.TR4"},
	{"DATA/LIBRARY.TR4"},
	{"DATA/LOWSTRT.TR4"},
	{"DATA/NUTRENCH.TR4"},
	{"DATA/PALACES.TR4"},
	{"DATA/PALACES2.TR4"},
	{"DATA/SEMER.TR4"},
	{"DATA/SEMER2.TR4"},
	{"DATA/SETTOMB1.TR4"},
	{"DATA/SETTOMB2.TR4"},
	{"DATA/TRAIN.TR4"},
	{NULL}
};

static level_info_t tr4_demo_levels[] = {
	{"DATA/TITLE.TR4"},
	{"DATA/LIBDEM.TR4"},
	{NULL}
};

static level_info_t tr4_times_levels[] = {
	{"DATA/TITLE.TR4"},
	{"DATA/OFFICE.TR4"},
	{"DATA/TIMES.TR4"},
	{NULL}
};

static level_info_t tr5_levels[] = {
	{"DATA/TITLE.TRC"},
	{"DATA/ANDREA1.TRC"},
	{"DATA/ANDREA2.TRC"},
	{"DATA/ANDREA3.TRC"},
	{"DATA/ANDY1.TRC"},
	{"DATA/ANDY2.TRC"},
	{"DATA/ANDY3.TRC"},
	{"DATA/JOBY2.TRC"},
	{"DATA/JOBY3.TRC"},
	{"DATA/JOBY4.TRC"},
	{"DATA/JOBY5.TRC"},
	{"DATA/RICH1.TRC"},
	{"DATA/RICH2.TRC"},
	{"DATA/RICH3.TRC"},
	{"DATA/RICHCUT2.TRC"},
	{"DATA/DEL.TRC"},
	{NULL}
};

static level_info_t tr5_demo_levels[] = {
	{"DATA/TITLE.TRC"},
	{"DATA/DEMO.TRC"},
	{NULL}
};

gamepath_info_t gamepath_info[] = {
	// 0
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider I/",
	 TR_I,
	 tr1_levels},
	// 1
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider I (Demo) Vilcabama 1/",
	 TR_I_DEMO,
	 tr1_demo1_levels},
	// 2
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider I (Demo) Vilcabama 2/",
	 TR_I,
	 tr1_demo2_levels},
	// 3
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider I - Unfinished Business/",
	 TR_I_UB,
	 tr1_gold_levels},
	// 4
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider II/",
	 TR_II,
	 tr2_levels},
	// 5
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider II (Demo)/",
	 TR_II,
	 tr2_demo_levels},
	// 6
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider II Gold (Full Net)/",
	 TR_II,
	 tr2_gold_levels},
	// 7
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider III/",
	 TR_III,
	 tr3_levels},
	// 8
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider III (Demo) India/",
	 TR_III,
	 tr3_demo1_levels},
	// 9
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider III (Demo) South Pacific/",
	 TR_III,
	 tr3_demo2_levels},
	// 10
	{
	 "Tomb Raider IV - The Last Revelation/",
	 TR_IV,
	 tr4_levels},
	// 11
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider IV - The Last Revelation (Demo)/",
	 TR_IV_DEMO,
	 tr4_demo_levels},
	// 12
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider IV - The Times/",
	 TR_IV,
	 tr4_times_levels},
	// 13
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider V - Chronicles/",
	 TR_V,
	 tr5_levels},
	// 14
	{
	 "C:/Spiele/Tomb Raider/Tomb Raider V - Chronicles (Demo)/",
	 TR_V,
	 tr5_demo_levels},
	// 15
	{
	 NULL}
};

void infinitePerspective(GLdouble fovy, GLdouble aspect, GLdouble znear)
{
	GLdouble left, right, bottom, top;
	GLdouble m[16];

	top = znear * tan(fovy * M_PI / 360.0);
	bottom = -top;
	left = bottom * aspect;
	right = top * aspect;

	//qglFrustum(left, right, bottom, top, znear, zfar);

	m[ 0] = (2 * znear) / (right - left);
	m[ 4] = 0;
	m[ 8] = (right + left) / (right - left);
	m[12] = 0;

	m[ 1] = 0;
	m[ 5] = (2 * znear) / (top - bottom);
	m[ 9] = (top + bottom) / (top - bottom);
	m[13] = 0;

	m[ 2] = 0;
	m[ 6] = 0;
	//m[10] = - (zfar + znear) / (zfar - znear);
	//m[14] = - (2 * zfar * znear) / (zfar - znear);
	m[10] = -1;
	m[14] = -2 * znear;

	m[ 3] = 0;
	m[ 7] = 0;
	m[11] = -1;
	m[15] = 0;

	qglMultMatrixd(m);
}
