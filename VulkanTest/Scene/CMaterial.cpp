#include "../Engine/Engine_Renderer.h"

CMaterial::CMaterial(std::string shade_name_)
{
	shade_id  = Engine::renderer->GetShadeId(shade_name_);
	datablock = {};
}