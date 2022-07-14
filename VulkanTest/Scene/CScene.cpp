#include "CScene.h"


std::vector<CMeshBase*>	temp_meshs_list;			//Temporary mesh list for recognize mesh by index
std::vector<CMaterial*>	temp_material_packet;	//Temporary material list for recognize material by index

CScene::CScene(WString directory_, CGraphics* graphics_) : directory(directory_)
{
	//initialized//
	Void* buffer;
	Uint buffer_size;
	ReadDirectory(directory_, &buffer, &buffer_size);

	Assimp::Importer importer;
	const aiScene* ai_scene = importer.ReadFileFromMemory(buffer, buffer_size,	aiProcess_CalcTangentSpace |
																				aiProcess_Triangulate |
																				aiProcess_JoinIdenticalVertices |
																				aiProcess_SortByPType | aiProcess_GenNormals |
																				aiProcess_LimitBoneWeights);
	if (!ai_scene)
	{
		throw std::runtime_error("CScene Error - Failed load scene from file");
		return;
	}

	//Create GPU uploading program for upload this scene...
	CGpuUploadTask* upload_task = new CGpuUploadTask(graphics_->p_Device, graphics_->p_CommandPool);
	upload_task->Reset();

	//Open log-report-file...
	std::ofstream out_report_file;
	out_report_file.open(std::wstring(directory_) + L"_load_report.3ds");
	
	//Convertion process...
	ConvertPbrMaterials(ai_scene, GetDirectoryFolder().c_str(), &out_report_file, upload_task);
	ConvertMeshs(ai_scene, &out_report_file, upload_task);
	ConvertAnimations(ai_scene, &out_report_file);
	this->root_object = ConvertObjects(ai_scene->mRootNode, nullptr, &out_report_file, upload_task);

	upload_task->Execute(graphics_->p_GraphicsQueue);//Now executing the scene uploading task

	//Classify game objects...
	ClassifyBones(root_object);
	ClassifyArmatures(root_object, graphics_->GetVkDevice());

	//Close loading process...
	temp_meshs_list.clear();
	temp_material_packet.clear();

	out_report_file.close();

	importer.FreeScene();//Free assimp scene's data struct after all relevant data copied

	upload_task->WaitAntilComplite();//Cpu continiue to run saperatly antil this point
	delete upload_task;

	int y = 5;
}


CTexture2D* CreateTextureFromMaterial(const aiMaterial* ai_material_, String key_, Uint type_, Uint index_, String type_name_, WString folder_directory_, CGpuUploadTask* upload_task_, std::ofstream* log_file_)
{
	aiString ai_path;

	if (!ai_material_->Get(key_, type_, index_, ai_path) == AI_SUCCESS)
	{
		*log_file_ << std::string("CreateTextureFromMaterial Error -> Failed to read details of ") + type_name_ + " texture.\n";
		return nullptr;
	}
	else
	{
		std::string  path  = ai_path.C_Str();
		std::wstring wpath = folder_directory_ + std::wstring(L"/") + std::wstring(path.begin(), path.end());
		return new CTexture2D(Engine::graphics->m_DeviceMemory.local_space, wpath, upload_task_);
	}

}

//Blender Material Statements...
#define AI_MATKEY_CONVERTED_PBR_BASECOLOR			"$raw.DiffuseColor|file", aiTextureType_UNKNOWN, 0
#define AI_MATKEY_CONVERTED_PBR_METALICE			"$raw.ReflectionFactor|file", aiTextureType_UNKNOWN, 0
#define AI_MATKEY_CONVERTED_PBR_ROUGNESS			"$raw.ShininessExponent|file", aiTextureType_UNKNOWN, 0
#define AI_MATKEY_CONVERTED_PBR_SPECULAR			"$raw.SpecularFactor|file", aiTextureType_UNKNOWN, 0
#define AI_MATKEY_CONVERTED_PBR_TRANSPERANCY_FACTOR	"$raw.TransparencyFactor|file", aiTextureType_UNKNOWN, 0
#define AI_MATKEY_CONVERTED_PBR_NORMALS				"$raw.NormalMap|file", aiTextureType_UNKNOWN, 0
#define AI_MATKEY_CONVERTED_PBR_EMISSION			"$raw.EmissiveColor|file", aiTextureType_UNKNOWN, 0

Void CScene::ConvertPbrMaterials(const aiScene* ai_scene_, WString folder_directory_, std::ofstream* log_file_, CGpuUploadTask* upload_task_)
{
	for (Uint i = 0; i < ai_scene_->mNumMaterials; i ++)
	{
		const aiMaterial* ai_material = ai_scene_->mMaterials[i];
		
		aiString material_name;
		ai_material->Get(AI_MATKEY_NAME, material_name);

		CTexture2D* base_color_texture  = CreateTextureFromMaterial(ai_material, AI_MATKEY_CONVERTED_PBR_BASECOLOR, "base_color", folder_directory_, upload_task_, log_file_);
		CTexture2D* metalic_texture		= CreateTextureFromMaterial(ai_material, AI_MATKEY_CONVERTED_PBR_METALICE, "metalic", folder_directory_, upload_task_, log_file_);
		CTexture2D* roughness_texture	= CreateTextureFromMaterial(ai_material, AI_MATKEY_CONVERTED_PBR_ROUGNESS, "roughness", folder_directory_, upload_task_, log_file_);
		CTexture2D* specular_texture	= CreateTextureFromMaterial(ai_material, AI_MATKEY_CONVERTED_PBR_SPECULAR, "emissive", folder_directory_, upload_task_, log_file_);
		CTexture2D* transmision_texture = CreateTextureFromMaterial(ai_material, AI_MATKEY_CONVERTED_PBR_TRANSPERANCY_FACTOR, "transperency", folder_directory_, upload_task_, log_file_);
		CTexture2D* emission_texture	= CreateTextureFromMaterial(ai_material, AI_MATKEY_CONVERTED_PBR_EMISSION, "emmision", folder_directory_, upload_task_, log_file_);
		CTexture2D* normals_texture		= CreateTextureFromMaterial(ai_material, AI_MATKEY_CONVERTED_PBR_NORMALS, "normals", folder_directory_, upload_task_, log_file_);
		
		
		Engine::resource_manager->AddTexturesPacket({	base_color_texture,
														metalic_texture,
														roughness_texture,
														specular_texture,
														transmision_texture,
														emission_texture,
														normals_texture		});

		aiColor4D	base_color(1.0, 1.0, 1.0, 1.0);
		Float		metalic			= 0.0f;
		Float		specular		= 0.0f;
		Float		roughness		= 0.5;
		Float		transparency	= 0.0f;
		aiColor4D	emmisive(1.0, 1.0, 1.0, 1.0);

		aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_DIFFUSE,		&base_color);
		aiGetMaterialFloat(ai_material, AI_MATKEY_REFLECTIVITY,			&metalic);
		aiGetMaterialFloat(ai_material, AI_MATKEY_SHININESS_STRENGTH,	&specular);  specular *= 2.0f;
		aiGetMaterialFloat(ai_material, AI_MATKEY_SHININESS,			&roughness); roughness = 1.0f - sqrt(roughness) * 0.1f;
		aiGetMaterialFloat(ai_material, AI_MATKEY_TRANSPARENCYFACTOR,	&transparency);
		aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_EMISSIVE,		&emmisive);

		//pow(2, 1.0 / shininess)
		aiGetMaterialColor(ai_material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR,	&base_color);
		aiGetMaterialFloat(ai_material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR,	&metalic);
		aiGetMaterialFloat(ai_material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR,	&roughness);
		
		//Create a new material...
		CMaterial* material = new CMaterial("BasicPbr");
		Engine::GpuPbrMaterialStruct& material_data = material->Data<Engine::GpuPbrMaterialStruct>();

		material_data.base_color				= Vector4D(base_color[0], base_color[1], base_color[2], base_color[3]);
		material_data.metalic					= metalic;
		material_data.specular					= specular;
		material_data.roughness					= roughness;
		material_data.transmision				= transparency;
		material_data.emmision_power			= emmisive[0] + emmisive[1] + emmisive[2];
		material_data.emmision					= Vector3D(emmisive[0], emmisive[1], emmisive[2]) * (1.0f / material_data.emmision_power);

		material_data.base_color_texture_loc	= Engine::resource_manager->GetTextureLocation(base_color_texture);
		material_data.metalic_texture_loc		= Engine::resource_manager->GetTextureLocation(metalic_texture);
		material_data.roughness_texture_loc		= Engine::resource_manager->GetTextureLocation(roughness_texture);
		material_data.specular_texture_loc		= Engine::resource_manager->GetTextureLocation(specular_texture);
		material_data.transmision_texture_loc	= Engine::resource_manager->GetTextureLocation(transmision_texture);
		material_data.emission_texture_loc		= Engine::resource_manager->GetTextureLocation(emission_texture);
		material_data.normals_texture_loc		= Engine::resource_manager->GetTextureLocation(normals_texture);
		
		temp_material_packet.push_back(material);
	}

	Engine::resource_manager->UploadMaterialsPacket(temp_material_packet, upload_task_);
}

Void CScene::ConvertMeshs(const aiScene* ai_scene_, std::ofstream* log_file_, CGpuUploadTask* upload_task_)
{
	for (Uint i = 0; i < ai_scene_->mNumMeshes; i++)
	{
		const aiMesh* ai_mesh = ai_scene_->mMeshes[i];
		CMeshBase* mesh;
		std::string mesh_name = ai_mesh->mName.C_Str();

		mesh = new CMesh<CSkinVertex>(ai_mesh);
		
		mesh->SetMaterial(temp_material_packet.at(ai_mesh->mMaterialIndex));

		temp_meshs_list.push_back(mesh);
		if (meshs.find(std::string(mesh_name)) == meshs.end())//If there not exist mesh with a same name
			meshs.insert(std::pair<std::string, CMeshBase*>(mesh_name, mesh));
		else
			*log_file_ << "Load Mesh (Error) - Duplicate Meshs name";

		mesh->UploadBuffers(upload_task_);//Upload mesh to GPU
	}
}

Void CScene::ConvertAnimations(const aiScene* ai_scene_, std::ofstream* log_file_)
{
	for (Uint i = 0; i < ai_scene_->mNumAnimations; i ++)
		animations.insert(std::pair<std::string, CAnimation*>(std::string(ai_scene_->mAnimations[i]->mName.data), new CAnimation(ai_scene_->mAnimations[i])));
}

CObject* CScene::ConvertObjects(const aiNode* ai_object_, CObject* parent_, std::ofstream* log_file_, CGpuUploadTask* upload_task_)
{
	CObject* new_object = new CObject(ai_object_->mName.C_Str());
	if (parent_ != nullptr)
		parent_->AddChild(new_object);

	new_object->SetOffsetTransform(Matrix4D((Float*)&ai_object_->mTransformation));
	
	for (Uint i = 0; i < ai_object_->mNumMeshes; i++)
		new_object->AddMesh(temp_meshs_list.at(ai_object_->mMeshes[i]));

	for (Uint i = 0; i < ai_object_->mNumChildren; i++)
		ConvertObjects(ai_object_->mChildren[i], new_object, log_file_, upload_task_);

	return new_object;
}


//Call Objects events...

Void CallObjectsInitEventRecursive(CObject* object_, CGpuUploadTask* upload_task_)
{
	object_->Init(upload_task_);

	for (Uint i = 0; i < object_->GetChildrenNumber(); i++)
		CallObjectsInitEventRecursive(object_->GetChild(i), upload_task_);
}

Void CScene::CallObjectsInitEvent(CGpuUploadTask* upload_task_)
{
	CallObjectsInitEventRecursive(root_object, upload_task_);
}

Void CallObjectsUpdateEventRecursive(CObject* object_)
{
	object_->Update();

	for (Uint i = 0; i < object_->GetChildrenNumber(); i++)
		CallObjectsUpdateEventRecursive(object_->GetChild(i));
}

Void CScene::CallObjectsUpdateEvent()
{
	CallObjectsUpdateEventRecursive(root_object);
}

Void CallObjectsDrawEventRecursive(CObject* object_, CGpuDrawTask* draw_task_, Uint layer_)
{
	object_->Render(draw_task_, layer_);

	for (Uint i = 0; i < object_->GetChildrenNumber(); i++)
		CallObjectsDrawEventRecursive(object_->GetChild(i), draw_task_, layer_);
}

Void CScene::CallObjectsDrawEvent(CGpuDrawTask* draw_task_, Uint layer_)
{
	CallObjectsDrawEventRecursive(root_object, draw_task_, layer_);
}