#pragma once
#include "CCamera.h"
#include "CArmature.h"
#include "CLight.h"
#include <fstream>
#include <filesystem>

class CScene
{
	private:
		std::map<std::string, CPbrMaterial*> materials;
		std::map<std::string, CMeshBase*>	 meshs;
		std::map<std::string, CAnimation*>	 animations;
		std::vector<CLight*>				 lights;

	public:
		CObject* root_object;

		const std::wstring directory;

	public:
		CScene(WString directory_, CGraphics* graphics_);

		std::wstring GetDirectory() { return directory; }
		std::wstring GetDirectoryFolder()
		{
			Uint loc;
			for (loc = directory.length() - 1 ; loc > 0; loc--)
			{
				if (directory.at(loc) == '/' || directory.at(loc) == 92)
					break;
			}
			
			return directory.substr(0, loc);
		}

		Void		ConvertPbrMaterials(const aiScene* ai_scene_, WString folder_directory_, std::ofstream* log_file_, CGpuUploadTask* upload_task_);
		Void		ConvertMeshs(const aiScene* ai_scene_, std::ofstream* log_file_, CGpuUploadTask* upload_task_);
		Void		ConvertAnimations(const aiScene* ai_scene_, std::ofstream* log_file_);
		CObject*	ConvertObjects(const aiNode* ai_object_, CObject* parent_, std::ofstream* log_file_, CGpuUploadTask* upload_task_);
		


		Void ClassifyBones(CObject* object_);
		Void ClassifyArmatures(CObject* entry_, VkDevice device_);

		CObject* GetRootObject() { return this->root_object; }

		CAnimation* GetAnimation(std::string explict_name_) 
		{ 
			if(animations.find(explict_name_) != animations.end())
				return animations[explict_name_];
			
			throw std::range_error("Animation not found");
			return nullptr;
		}

		//Call objects events...
		Void CallObjectsInitEvent(CGpuUploadTask* upload_task_);
		Void CallObjectsUpdateEvent();
		Void CallObjectsDrawEvent(CGpuDrawTask* draw_task_, Uint layer_);


		Void AddLight(CLight* light_) { lights.push_back(light_); }

		Uint GetLightsCount() { return lights.size(); }
		CLight* GetLight(Uint index_) { return lights.at(index_); }
		std::vector<CLight*> GetAllLights() { return lights; }
};


//Object traceing meshods...
CObject* SearchObjectForwardInFamely(std::string object_name_, CObject* entry_);
CObject* SearchObjectForward(std::string object_name_, CObject* entry_);