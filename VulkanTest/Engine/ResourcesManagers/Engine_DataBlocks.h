#pragma once

#include "../Engine_GpuStructures.h"

namespace Engine
{
	struct CDataBlockBae
	{
		virtual GpuDataBlockStruct BuildGpuStruct() { return {}; }
		Pointer GetDataPointer() { return (Byte*)this + sizeof(Uint); }
	};
}