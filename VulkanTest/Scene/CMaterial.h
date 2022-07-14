#pragma once
#include "../Graphics/CPipeline.h"

typedef struct { Byte bytes[256]; } __256t;

enum class MaterialProperitiesType
{
	Float,
	Vector2D,
	Vector3D,
	Vector4D,

	Int,
	IVector2D,
	IVector3D,
	IVector4D,

	Uint,
	UVector2D,
	UVector3D,
	UVector4D,
};

static Uint MaterialProperitiesTypeSize[] = 
{
	1, 2, 3, 4,
	1, 2, 3, 4,
	1, 2, 3, 4
};

struct MaterialProperitie
{
	std::string name;
	MaterialProperitiesType type;
};

class CShade
{
	private:
		CPipeline* pipeline;
		std::list<MaterialProperitie> material_properities;

	public:

		Void AddMatrerialProperitie(std::string name_, MaterialProperitiesType type_)
		{
			if (MaterialProperitiesTypeSize[(Uint)type_] == 1)
			{
				//material_properities.insert(frees[2])
			}
		}

};


class CMaterial
{
	private:
		Uint shade_id;
		__256t datablock;

	public:

		template<class Struct>
		Struct& Data() { return reinterpret_cast<Struct&>(datablock); }

		CMaterial(std::string shade_name_);

		Uint GetShadeId() { return this->shade_id; }
};