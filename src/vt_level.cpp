#include "SDL.h"
#include "SDL_endian.h"
#include "vt_level.h"

#define RCSID "$Id: vt_level.cpp,v 1.1 2002/09/20 15:59:02 crow Exp $"

void VT_Level::prepare_level()
{
	bitu32 i;

	if ((game_version >= TR_II) && (game_version <= TR_V)) {
		if (!read_32bit_textiles) {
			if (textile32.empty())
				textile32.resize(num_textiles);
			for (i = 0; i < (num_textiles - num_misc_textiles); i++)
				convert_textile16_to_textile32(textile16[i], textile32[i]);
		}
	} else {
		textile32.resize(num_textiles);
		for (i = 0; i < num_textiles; i++)
			convert_textile8_to_textile32(textile8[i], palette, textile32[i]);
	}
}

tr_staticmesh_t *VT_Level::find_staticmesh_id(bitu32 object_id)
{
	bitu32 i;

	for (i = 0; i < static_meshes.size(); i++)
		if (static_meshes[i].object_id == object_id)
			return &static_meshes[i];

	return NULL;
}

tr2_item_t *VT_Level::find_item_id(bit32 object_id)
{
	bitu32 i;

	for (i = 0; i < items.size(); i++)
		if (items[i].object_id == object_id)
			return &items[i];

	return NULL;
}

tr_moveable_t *VT_Level::find_moveable_id(bitu32 object_id)
{
	bitu32 i;

	for (i = 0; i < moveables.size(); i++)
		if (moveables[i].object_id == object_id)
			return &moveables[i];

	return NULL;
}

void VT_Level::convert_textile8_to_textile32(tr_textile8_t & tex, tr2_palette_t & pal, tr4_textile32_t & dst)
{
	int x, y;

	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			int col = tex.pixels[y][x];

			if (col > 0)
				dst.pixels[y][x] = ((int)pal.colour[col].r) | ((int)pal.colour[col].g << 8) | ((int)pal.colour[col].b << 16) | (0xff << 24);
			else
				dst.pixels[y][x] = 0x00000000;
		}
	}
}

void VT_Level::convert_textile16_to_textile32(tr2_textile16_t & tex, tr4_textile32_t & dst)
{
	int x, y;

	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			int col = tex.pixels[y][x];

			if (col & 0x8000)
				dst.pixels[y][x] = ((col & 0x00007c00) >> 7) | (((col & 0x000003e0) >> 2) << 8) | (((col & 0x0000001f) << 3) << 16) | 0xff000000;
			else
				dst.pixels[y][x] = 0x00000000;
		}
	}
}

static void WriteTGAfile(const char *filename, const bitu8 *data, const int width, const int height)
{
	unsigned char c;
	unsigned short s;
	int x, y;
	FILE *st;

	st = fopen(filename, "wb");
	if (st == NULL)
		return;

	// write the header
	// id_length
	c = 0;
	fwrite(&c, sizeof(c), 1, st);
	// colormap_type
	c = 0;
	fwrite(&c, sizeof(c), 1, st);
	// image_type
	c = 2;
	fwrite(&c, sizeof(c), 1, st);
	// colormap_index
	s = 0;
	fwrite(&s, sizeof(s), 1, st);
	// colormap_length
	s = 0;
	fwrite(&s, sizeof(s), 1, st);
	// colormap_size
	c = 0;
	fwrite(&c, sizeof(c), 1, st);
	// x_origin
	s = 0;
	fwrite(&s, sizeof(s), 1, st);
	// y_origin
	s = 0;
	fwrite(&s, sizeof(s), 1, st);
	// width
	s = SDL_SwapLE16(width);
	fwrite(&s, sizeof(s), 1, st);
	// height
	s = SDL_SwapLE16(height);
	fwrite(&s, sizeof(s), 1, st);
	// bits_per_pixel
	c = 32;
	fwrite(&c, sizeof(c), 1, st);
	// attributes
	c = 0;
	fwrite(&c, sizeof(c), 1, st);

	for (y = height - 1; y >= 0; y--)
		for (x = 0; x < width; x++) {
			fwrite(&data[(y * width + x) * 4 + 2], sizeof(bitu8), 1, st);
			fwrite(&data[(y * width + x) * 4 + 1], sizeof(bitu8), 1, st);
			fwrite(&data[(y * width + x) * 4 + 0], sizeof(bitu8), 1, st);
			fwrite(&data[(y * width + x) * 4 + 3], sizeof(bitu8), 1, st);
		}

	fclose(st);
}

void VT_Level::dump_textures()
{
	bitu32 i;

	for (i = 0; i < num_textiles; i++) {
		char buffer[1024];

		sprintf(buffer, "c:/home/dev/cvs/vt2/%03i_32.tga", i);
		WriteTGAfile(buffer, (bitu8 *)&textile32[i].pixels, 256, 256);
	}
}

