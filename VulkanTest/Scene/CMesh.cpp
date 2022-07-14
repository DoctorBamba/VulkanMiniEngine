#include "CMesh.h"


Uint ElementsPerFace(MeshTopologyType topology_type_)
{
	switch (topology_type_) 
	{
	case VK_PRIMITIVE_TOPOLOGY_MAX_ENUM:
		return 0;
		break;
	case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
		return 1;
		break;
	case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		return 2;
		break;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		return 3;
		break;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		return 4;//For quads
		break;
	default:
		break;
	}

	return 0;
}

MeshTopologyType ConvertTopologyType(Uint ai_type_code_)
{
	switch (ai_type_code_)
	{
	case 0x1:
		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		break;
	case 0x2:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;
	case 0x4:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
	case 0x8:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		break;
	default:
		return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
		break;
	}
}