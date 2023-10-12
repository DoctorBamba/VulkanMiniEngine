#pragma once
#include "UiGraphics.h"
#include "../Graphics/CTexture2DArray.h"
#include "../Graphics/CGpuTask.h"

#define EUI_MAX_FONTINSTANCES_IN_LAYOUT 4096

extern FT_Library FT_FontLibrary;

#define EUI_OBJECT_MAX_CIRCLES		2
#define EUI_OBJECT_MAX_TRIANGLES	2


/*this is the quad struct for the font's instance struct*/
typedef struct 
{
	Vector3D		ut[2];
	Float			width;
	Float			height;
	Uint			texlayer;
	Vector4D		color;
	Uint			marked;
	Vector4D		mark_color;
}UiFontInstance;

/*this is the Charter struct for a charters map's element*/
struct UiCharacter
{
	Int						texlayer;		//Index of the character layer in the 3D Font texture
	Vector2D__<Uint>		size;			// Size of glyph
	Vector2D__<Int>			bearing;		// Offset from baseline to left/top of glyph
	Vector2D__<Uint>		advance;		// Offset to advance to next glyph


	UiCharacter(const Int texture_layer_, const Vector2D__<Uint>& size_, Vector2D__<Int> bearing_, Vector2D__<Uint> advence_)
	{
		texlayer		= texture_layer_;
		size			= size_;
		bearing			= bearing_;
		advance			= advence_;
	}

	UiCharacter() : UiCharacter(-1, Vector2D__<Uint>(), Vector2D__<Int>(), Vector2D__<Uint>()) {}
};

struct UiCharBitmap
{
	Byte*	data;
	Uint16	width;
	Uint16	height;
};

#define EUI_SPACE_CHARACTURE_WIDTH_FACTOR 0.5f

#define eui_place_fontsize(size) (if (fontsize_map.find(size) == fontsize_map.end()) \
			fontsize_map.insert(std::pair<UINT16, CUI_Font_Size*>(size, new CUI_Font_Size(size, face))))


#define eui_place_fontsort_map(depth,size) (if (pipeline_sort_map->find(depth) == pipeline_sort_map->end())  \
				pipeline_sort_map->insert(std::pair<FLOAT, std::map<UINT16, QUADSPACK*>>(depth, std::map<UINT16, QUADSPACK*>)); \
			std::map<UINT16, QUADSPACK*> *sizeMap = &pipeline_sort_map->at(depth);\
			if (sizeMap->find(size) == sizeMap->end())\
				sizeMap->insert(std::pair<UINT16, QUADSPACK*>(size, new QUADSPACK(1024))));


class UiFontLevel
{
	private:
		const FT_Face				face;		/* handle to face object */
		const Uint					size;
		std::map<Byte, UiCharacter>	characters; /* characters map */

		//buffers...

		std::vector<Byte*>			bitmaps_pointers;
		Byte*						bitmaps_data;
		Uint16						bitmaps_width;
		Uint16						bitmaps_height;

		CTexture2DArray*			textures;
		
	public:
		UiFontLevel(FT_Face face_, Uint16 size_, CGpuUploadTask* upload_task_);

		UiCharacter CreateCharacter(FT_Face face_, char c_);
		FT_Error SetSize(Uint16 size);

		Void Instantiate(std::string str_, std::list<UiFontInstance>* chanel_, Vector4D color_ = Vector4D(0.0f, 0.0f, 0.0f, 0.0f), Bool marked_ = false, Vector4D marked_color_ = Vector4D(0.3f, 0.5f, 0.8f, 0.0f));

		UiCharacter		GetCharacter(Byte c) { return (characters.find(c) != characters.end()) ? characters.at(c) : UiCharacter(); }
		Int				GetAdvance(const std::string string);
		Int				GetIndexByAdvance(const std::string string_, const Int advance_);
		Int				GetRowHeight() { return bitmaps_height; }
};

/*this is the main font class*/


class UiFont
{
	private:

		FT_Face							face;
		std::map<Uint, UiFontLevel*>	levels;
		
	public:


		UiFont(std::wstring directory_);


		//Bacis functions...

		//void place_level(const Uint size) { if (levels.find(size) == levels.end()) levels.insert(std::pair<UINT16, CUI_FontLevel*>(size, new CUI_FontLevel(size, face))); }
		//UiFontLevel* find_levels(UINT16 size) { return (levels.find(size) != levels.end()) ? levels.at(size) : nullptr; }

		//Get function...

		//INT	get_advance(const std::string string, UINT16 size, const UINT start_index, const UINT end_index) { place_level(size); return levels.at(size)->get_advance(string, start_index, end_index); };
		//INT	get_floor_advance(const std::string string, UINT16 size, const INT advance_in, INT* inraw_index_out) { place_level(size); return levels.at(size)->get_floor_advance(string, advance_in, inraw_index_out); };
};
