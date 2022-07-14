#include "Engine_BasicMeshs.h"

namespace Engine
{
	namespace Meshs
	{
		CMesh<CBaseVertex>* Quad2D;
		CMesh<CBaseVertex>* Cube;
		CMesh<CBaseVertex>* Sphere;
		CMesh<CBaseVertex>* Cylinder;
		CMesh<CBaseVertex>* Cone;
		CMesh<CBaseVertex>* Torus;
		CMesh<CBaseVertex>* PlanetControls;
	}
}

Void Engine::Meshs::LoadBasicMeshs(CGpuUploadTask* upload_task_)
{
	//initialized//

	Void* buffer;
	Uint buffer_size;
	ReadDirectory(L"Engine/BasicMeshs.fbx", &buffer, &buffer_size);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory(buffer, buffer_size, aiProcess_CalcTangentSpace |
																			aiProcess_JoinIdenticalVertices |
																			aiProcess_SortByPType | aiProcess_GenNormals);
	if (!scene)
	{
		throw std::runtime_error("Failed load basic meshs");
		return;
	}

	Quad2D			= new CMesh<CBaseVertex>(scene->mMeshes[0]);
	Cube			= new CMesh<CBaseVertex>(scene->mMeshes[1]);
	Sphere			= new CMesh<CBaseVertex>(scene->mMeshes[2]);
	Cylinder		= new CMesh<CBaseVertex>(scene->mMeshes[3]);
	Cone			= new CMesh<CBaseVertex>(scene->mMeshes[4]);
	Torus			= new CMesh<CBaseVertex>(scene->mMeshes[5]);
	PlanetControls	= new CMesh<CBaseVertex>(scene->mMeshes[6]);

	//Upload meshs tp gpu...
	Quad2D->UploadBuffers(upload_task_);
	Cube->UploadBuffers(upload_task_);
	Sphere->UploadBuffers(upload_task_);
	Cylinder->UploadBuffers(upload_task_);
	Cone->UploadBuffers(upload_task_);
	Torus->UploadBuffers(upload_task_);
	PlanetControls->UploadBuffers(upload_task_);

	importer.FreeScene();
}

CMesh<CBaseVertex>* Engine::Meshs::NewControledCube(CGpuUploadTask* upload_task_, Uint subdevision_ = 1)
{
	const Uint vn = subdevision_ + 1;
	const Float delta = 1.0f / subdevision_;

	std::vector<CBaseVertex> vertices(vn * vn * 6);
	std::vector<Uint>	 indices(subdevision_ * subdevision_ * 4 * 6);

	CBaseVertex* source_quad;
	CBaseVertex* target_vert;
	Uint*	 target_indices;

	Vector3D delta_position_u, delta_position_v;
	Vector2D delta_texcoord_u, delta_texcoord_v;

	for (Uint q = 0; q < 6; q++)
	{
		Uint target_offset = vn * vn * q;

		source_quad		= PlanetControls->vertices.data() + 4 * q;
		target_vert		= vertices.data() + target_offset;
		target_indices	= indices.data()  + subdevision_ * subdevision_ * 4 * q;

		delta_position_u = (source_quad[1].position - source_quad[0].position) * delta;
		delta_position_v = (source_quad[3].position - source_quad[0].position) * delta;
		delta_texcoord_u = (source_quad[1].texcoord - source_quad[0].texcoord) * delta;
		delta_texcoord_v = (source_quad[3].texcoord - source_quad[0].texcoord) * delta;

		Uint u, v;
		for (Uint i = 0; i < vn * vn; i++)
		{
			u = i % vn;
			v = i / vn;

			target_vert[i] = { source_quad[0].position + delta_position_u * u + delta_position_v * v,
							   source_quad[0].texcoord + delta_texcoord_u * u + delta_texcoord_v * v,
							   source_quad[0].normal, source_quad[0].tangent, source_quad[0].bitangent };

		}

		for (Uint i = 0; i < subdevision_ * subdevision_; i++)
		{
			u = i % subdevision_;
			v = i / subdevision_;

			target_indices[i * 4 + 0] = target_offset + vn * v + u;
			target_indices[i * 4 + 1] = target_offset + vn * v + u + 1;
			target_indices[i * 4 + 2] = target_offset + vn * v + u + 1 + vn;
			target_indices[i * 4 + 3] = target_offset + vn * v + u + vn;
		}
	}

	return new CMesh<CBaseVertex>("Controled_Cube", VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, vertices.data(), vertices.size(), indices.data(), indices.size(), upload_task_);
}