#ifndef _VT_LEVEL_H_
#define _VT_LEVEL_H_

#include "l_main.h"

class VT_Level : public TR_Level {
      public:
	void prepare_level();
	void dump_textures();
	tr_staticmesh_t *find_staticmesh_id(bitu32 object_id);
	tr2_item_t *find_item_id(bit32 object_id);
	tr_moveable_t *find_moveable_id(bitu32 object_id);

      protected:
	void convert_textile8_to_textile32(tr_textile8_t & tex, tr2_palette_t & pal, tr4_textile32_t & dst);
	void convert_textile16_to_textile32(tr2_textile16_t & tex, tr4_textile32_t & dst);
};

#endif // _VT_LEVEL_H_