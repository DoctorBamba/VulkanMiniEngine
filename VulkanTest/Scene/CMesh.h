#pragma once

#include "assimp/scene.h"
#include "assimp/mesh.h"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "assimp/pbrmaterial.h"

#pragma comment(lib, "assimp-vc142-mtd.lib")

#include "../Graphics/Buffers/CVertexBuffer.h"

#include "CMaterial.h"

#include "../Engine/Engine_Graphics.h"
#include "../Engine/ResourcesManagers/Engine_ResourcesManager.h"

#define MAXINIUM_VERTEX_BONES_CONTECTS 4

typedef VkPrimitiveTopology MeshTopologyType;


typedef struct
{
	std::string	bone_name;
	Matrix4D offset;

}BoneData;

class CBaseVertex
{
	public:
		Vector3D position;
		Vector2D texcoord;
		Vector3D normal;
		Vector3D tangent;
		Vector3D bitangent;
};

class CSkinVertex : public CBaseVertex
{
	public://Had an addition bones connection data
		Uint  bone_ids[MAXINIUM_VERTEX_BONES_CONTECTS];
		Float widths[MAXINIUM_VERTEX_BONES_CONTECTS];
};

class CMeshBase
{
	protected :
		typedef unsigned int Index;

		std::string				name;
		MeshTopologyType		topology_type;
		Uint					vertex_stride;
		
		Uint					vertices_number;
		Uint					faces_number;
		Uint					bones_number;

		CMaterial*				material;

		const aiMesh* ai_pointer;

		VkDevice* p_DeviceContext;

	public:

		MeshTopologyType GetTopologyType() const { return topology_type; }
		virtual Void UploadBuffers(CGpuUploadTask* upload_commands_) {}
		virtual Void Render(CGpuTask* task_) {}


		
		Void SetMaterial(CMaterial* material_datablock_) { this->material = material_datablock_; }
		CMaterial* GetMaterial() { return this->material; }

		std::string			GetName() { return name; }
		Uint				GetFaceNumber() { return faces_number; }
		
		virtual Uint		GetBonesNumber() { return 0; }
		virtual std::string	GetBoneName(Uint index_) { return ""; }
		virtual Matrix4D	GetBoneOffsetTransform(Uint index_) { return Matrix4D(1.0f); }
};


template<class Vertex> class CMesh : public CMeshBase
{
	static_assert(std::is_base_of<CBaseVertex, Vertex>::value, "Template must by a vertex type");

	//friend MakeTriangleList;

	public :
		std::vector<Vertex>		vertices;
		std::vector<Index>		indeices;
		std::vector<BoneData>	bones;
		
		//GPU...

		CVertexBuffer<Vertex>*		gpu_vertex_buffer;
		CIndexBuffer*				gpu_index_buffer;
		
	public : 
		CMesh(const aiMesh* ai_mesh_)
		{
			if (ai_mesh_ == nullptr)
			{
				throw std::runtime_error("CMesh :: Constructor Error -> the input ai_mesh object is nullptr");
				return;
			}

			this->name				= ai_mesh_->mName.C_Str();
			this->vertices_number	= ai_mesh_->mNumVertices;
			this->faces_number		= ai_mesh_->mNumFaces;
			this->bones_number		= ai_mesh_->mNumBones;
			this->vertex_stride		= sizeof(Vertex);

			this->ai_pointer = ai_mesh_;

			//Create CPU buffers...
			if (vertices_number > 0)
				Convert(ai_mesh_);
		}

		CMesh(String name_, MeshTopologyType topology_type_, Vertex* vertices_, Uint vertices_number_, Uint* indeices_, Uint indeices_number_, CGpuUploadTask* upload_task_)
		{
			name			= name_;
			topology_type	= topology_type_;
			vertices_number = vertices_number_;
			faces_number	= indeices_number_ / ElementsPerFace(topology_type_);
			bones_number	= 0;

			if (0 < vertices_number_)
			{
				vertices.resize(vertices_number_);
				memcpy(vertices.data(), vertices_, sizeof(Vertex) * vertices_number_);
			}

			if (0 < indeices_number_)
			{
				indeices.resize(indeices_number_);
				memcpy(indeices.data(), indeices_, sizeof(Uint) * indeices_number_);
			}

			UploadBuffers(upload_task_);
		}

		
		Void Convert(const aiMesh* ai_mesh_)
		{
			topology_type = ConvertTopologyType(ai_mesh_->mPrimitiveTypes);
			
			if (!ai_mesh_->HasFaces())
				return;

			Uint elenents_per_face = ElementsPerFace(topology_type);

			vertices.resize(ai_mesh_->mNumVertices);
			indeices.resize(ai_mesh_->mNumFaces * elenents_per_face);

			if (ai_mesh_->HasFaces())
			{
				for (Uint i = 0; i < ai_mesh_->mNumFaces;  i ++)
				{
					for(Uint j = 0 ; j < elenents_per_face ; j ++)
						indeices.at(i * elenents_per_face + j) = ai_mesh_->mFaces[i].mIndices[j];
				}
			}

			if (ai_mesh_->HasPositions())
			{
				for (Uint i = 0; i < ai_mesh_->mNumVertices ; i ++)
					vertices.at(i).position = Vector3D(ai_mesh_->mVertices[i].x, ai_mesh_->mVertices[i].y, ai_mesh_->mVertices[i].z);
			}

			if (ai_mesh_->HasTextureCoords(0))
			{
				for (Uint i = 0; i < ai_mesh_->mNumVertices ; i ++)
					vertices.at(i).texcoord= Vector2D(ai_mesh_->mTextureCoords[0][i].x, ai_mesh_->mTextureCoords[0][i].y);
			}

			if (ai_mesh_->HasNormals())
			{
				for (Uint i = 0; i < ai_mesh_->mNumVertices; i++)
					vertices.at(i).normal = Vector3D(ai_mesh_->mNormals[i].x, ai_mesh_->mNormals[i].y, ai_mesh_->mNormals[i].z);
			}

			if (ai_mesh_->HasTangentsAndBitangents())
			{
				for (Uint i = 0; i < ai_mesh_->mNumVertices; i++)
				{
					vertices.at(i).tangent   = Vector3D(ai_mesh_->mTangents[i].x, ai_mesh_->mTangents[i].y, ai_mesh_->mTangents[i].z);
					vertices.at(i).bitangent = Vector3D(ai_mesh_->mBitangents[i].x, ai_mesh_->mBitangents[i].y, ai_mesh_->mBitangents[i].z);
				}
			}

			if (!std::is_base_of<CSkinVertex, Vertex>::value)
				return;

			if (ai_mesh_->HasBones())
			{
				for (Uint i = 0; i < ai_mesh_->mNumBones; i ++)
				{
					BoneData new_contect;
					new_contect.bone_name = ai_mesh_->mBones[i]->mName.C_Str();
					new_contect.offset = *(Matrix4D*)(&ai_mesh_->mBones[i]->mOffsetMatrix);

					bones.push_back(new_contect);

					const aiBone* ai_bone = ai_mesh_->mBones[i];
					for (Uint j = 0; j < ai_bone->mNumWeights; j ++)
					{
						Uint vertex_id = ai_bone->mWeights[j].mVertexId;
						CSkinVertex* vertex = (CSkinVertex*)(&vertices.at(vertex_id));

						for (Uint k = 0; k < MAXINIUM_VERTEX_BONES_CONTECTS; k ++)
						{
							if (vertex->widths[k] == 0)//Search for first empty index k
							{
								vertex->bone_ids[k] = i;
								vertex->widths[k] = ai_bone->mWeights[j].mWeight;
								break;
							}
						}
					}
				}
			
				//Normalize widths...
				for (Uint i = 0; i < ai_mesh_->mNumVertices ; i ++)
				{
					CSkinVertex* vertex = (CSkinVertex*)(&vertices.at(i));

					Float widths_sum = 0.0f;
					for (Uint k = 0; k < MAXINIUM_VERTEX_BONES_CONTECTS; k ++)
						widths_sum += vertex->widths[k];

					for (Uint k = 0; k < MAXINIUM_VERTEX_BONES_CONTECTS; k++)
						vertex->widths[k] /= widths_sum;
				}
			}
		}

		Void UploadBuffers(CGpuUploadTask* upload_task_)
		{
			if (upload_task_ == nullptr)
			{
				throw std::runtime_error("Mesh :: UploadBuffers Error -> The upload task is nullptr");
				return;
			}

			if (!upload_task_->InRecording())
			{
				throw std::runtime_error("Mesh :: UploadBuffers Error -> Upload task not in recording mode!");
				return;
			}
			
			VulkanDevice* device = upload_task_->GetDevice();

			if (vertices_number > 0)
				gpu_vertex_buffer = new CVertexBuffer<Vertex>(device, vertices_number, upload_task_, vertices.data());

			if (faces_number > 0)
				gpu_index_buffer = new CIndexBuffer(device, faces_number * ElementsPerFace(topology_type), upload_task_, indeices.data());
		}


		Void Render(CGpuTask* task_)
		{
			if (0 < faces_number)
			{
				gpu_vertex_buffer->Bind(task_->GetCommandBuffer(), 0, sizeof(Vertex));
				gpu_index_buffer->Bind(task_->GetCommandBuffer());
				vkCmdDrawIndexed(task_->GetCommandBuffer(), faces_number * ElementsPerFace(topology_type), 1, 0, 0, 0);
			}
			else
			{
				gpu_vertex_buffer->Bind(task_->GetCommandBuffer(), 0, sizeof(Vertex));
				vkCmdDraw(task_->GetCommandBuffer(), vertices_number, 1, 0, 0);
				int y = 5;
			}
		}

		//template<class Instance> PE_Void DrawInstances(CVertexBuffer<Instance>* instance_buffer_, PE_Uint instance_count_, ID3D12GraphicsCommandList* command_list_);

		Uint		GetBonesNumber() { return bones.size(); }
		std::string	GetBoneName(Uint index_) { return bones.at(index_).bone_name; }
		Matrix4D	GetBoneOffsetTransform(Uint index_) { return bones.at(index_).offset; }
};

/*
template<class Vertex>
template<class Instance>
PE_Void inline CMesh<Vertex>::DrawInstances(CVertexBuffer<Instance>* instance_buffer_, PE_Uint instance_count_, ID3D12GraphicsCommandList* command_list_)
{
	command_list_->IASetPrimitiveTopology(topology_type); // set the primitive topology

	D3D12_VERTEX_BUFFER_VIEW vertex_view[2] = { gpu_vertex_buffer->Get_VertexBufferView(), instance_buffer_->Get_VertexBufferView() };
	command_list_->IASetVertexBuffers(0, 2, vertex_view); // set the vertex buffer (using the vertex buffer view)
	
	
	if (0 < faces_number)
	{
		D3D12_INDEX_BUFFER_VIEW index_view = gpu_index_buffer->Get_IndexBufferView();
		command_list_->IASetIndexBuffer(&index_view);
		command_list_->DrawIndexedInstanced(faces_number * ElementsPerFace(topology_type), instance_count_, 0, 0, 0);
	}
	else
	{
		command_list_->DrawInstanced(vertices_number, 1, 0, 0);
		command_list_->DrawInstanced(vertices_number, instance_count_, 0, 0);
	}
	command_list_->DrawIndexedInstanced(faces_number * ElementsPerFace(topology_type), instance_count_, 0, 0, 0); // draw 2 triangles (draw 1 instance of 2 triangles)
}
*/

Uint ElementsPerFace(MeshTopologyType topology_type_);
MeshTopologyType ConvertTopologyType(Uint ai_type_code_);

