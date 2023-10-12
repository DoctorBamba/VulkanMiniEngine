#include "UiFonts.h"

using namespace UI;

UiCharacter UiFontLevel::CreateCharacter(FT_Face face_, char c_)
{
	if (c_ == 32)//Spacial surface for 'space'
		return UiCharacter(0, Vector2D__<Uint>(size * EUI_SPACE_CHARACTURE_WIDTH_FACTOR, face_->glyph->bitmap.rows), Vector2D__<Int>(0, 0), Vector2D__<Uint>(0, 0));

	FT_GlyphSlot  slot = face_->glyph;

	/* load glyph image into the slot (erase previous one) */

	FT_Error error = FT_Load_Char(face_, c_, FT_LOAD_RENDER);

	if (error)
		return UiCharacter();  /* ignore errors */

	if (!face_->glyph->bitmap.buffer)
		return UiCharacter();  /* ignore errors */

	Uint16 width  = slot->bitmap.width;
	Uint16 height = slot->bitmap.rows;

	bitmaps_width  = max(bitmaps_width, width);
	bitmaps_height = max(bitmaps_height, height);

	Byte* bitmap_data = new Byte[width * height]; /*open new buffer for this specific bitmap(becuse FT_Load_Char erase previous bitmap)*/
	memset(bitmap_data, 0, width * height);
	memcpy(bitmap_data, slot->bitmap.buffer, width * height);//copy data

	bitmaps_pointers.push_back(bitmap_data);

	return UiCharacter( bitmaps_pointers.size() - 1,
						Vector2D__<Uint>(face_->glyph->bitmap.width, face_->glyph->bitmap.rows),
						Vector2D__<Int>(face_->glyph->bitmap_left, face_->glyph->bitmap_top),
						Vector2D__<Uint>((Uint)slot->advance.x, (Uint)slot->advance.y) );
}

FT_Error UiFontLevel::SetSize(Uint16 size_)
{
	return FT_Set_Pixel_Sizes(	face,   /* handle to face object */
								0,      /* pixel_width           */
								size);   /* pixel_height          */
}

UiFontLevel::UiFontLevel(FT_Face face_, Uint16 size_, CGpuUploadTask* upload_task_) : face(face_), size(size_)
{
	if (SetSize(size))
	{
		throw std::runtime_error("UiFontLevel Constructor Error -> There is problem with the font size");
		return;
	}

	//Create characters...
	std::vector<Byte*> bitmaps;

	for (Byte c = 0; c < 128; c++)
	{
		UiCharacter character = CreateCharacter(face_, c);
		if (character.texlayer != -1)//That mean character creation sucessed
			characters.insert(std::pair<Byte, UiCharacter>(c, character));
	}


	//Create a Texture2DArray from bitmaps...
	textures = new CTexture2DArray(	Graphics->p_Device->MemorySpaces.local_space,
									bitmaps_width,
									bitmaps_height, 
									bitmaps_pointers.size(),
									VK_FORMAT_R8G8B8A8_UNORM,
									false,
									bitmaps.data(), 
									upload_task_,
									"Font_Texture2DArray");


	//free bitmaps data from CPU...
	for (UINT i = 0; i < bitmaps_pointers.size(); i++)
		free(bitmaps_pointers.at(i));
}

Void UiFontLevel::Instantiate(std::string str_, std::list<UiFontInstance>* chanel_, Vector4D color_, Bool marked_, Vector4D marked_color_)
{
	if (chanel_ == nullptr)
		return;

	for (Uint i = 0; i < str_.size(); i++)
	{
		UiCharacter character = GetCharacter(str_.at(i));

		UiFontInstance instance;
		instance.texlayer	= character.texlayer;
		instance.width		= character.size[0];
		instance.height		= character.size[1];
		instance.color		= color_;
		instance.marked		= (Uint)marked_;
		instance.mark_color = marked_color_;

	}
}


Int UiFontLevel::GetAdvance(const std::string string_)
{
	Int advance = 0;

	if (string_.length() == 0)
		return 0;

	for (Uint i = 0; i < string_.length(); i++)
		advance += characters.at(string_.at(i)).size[0];

	return advance + characters.at(string_.at(string_.size() - 1)).bearing[0];
}

Int UiFontLevel::GetIndexByAdvance(const std::string string_, const Int advance_)
{
	Int advance = 0;
	
	if (advance_ <= 0 || string_.length() == 0)
		return -1;


	UiCharacter last_character = UiCharacter();
	for (Uint i = 0; i < string_.size(); i ++)
	{
		UiCharacter current_character = GetCharacter(string_.at(i));

		if (advance - (last_character.size[0] / 2) + last_character.bearing[0] <= advance && advance <= advance + (current_character.size[0] / 2) + current_character.bearing[0])
			return i;
		
		advance += current_character.size[0];
		last_character = current_character;
	}

	return string_.size();
}


UiFont::UiFont(std::wstring directory_)
{
	FT_Error error;

	std::string str_dir = std::string(directory_.begin(), directory_.end());

	//Create the face...

	if (!DirectoryExist(directory_.c_str()))
	{
		throw std::runtime_error(std::string("Load Font Error -> the file : ") + (char)34 + str_dir + (char)34 +  std::string(" is not exsist"));
		return;
	}
	
	error = FT_New_Face(FT_FontLibrary, str_dir.c_str(), 0, &face);

	if (error == FT_Err_Unknown_File_Format)
	{
		throw std::runtime_error("Load Font Error -> the format is unsupport");
		return;
	}
	else if (error)
	{
		throw std::runtime_error("Load Font Error -> the file failed to load");
		return;
	}
}