#pragma once

#include "DX_Workspace.h"

template<class T> class CDX_Instances
{
	private:

		ID3D11Buffer	*gpu_buffer;
		std::vector<T>	 cpu_buffer;

		UINT			 max_instances_number;

	public:

		CDX_Instances(UINT32 max_instances_number)
		{
			this->max_instances_number = max_instances_number;

			HRESULT result = S_OK;
			typedef T InstanceStruct;

			//Create the instance buffer

			D3D11_BUFFER_DESC InsBufferDesc;
			InsBufferDesc.Usage					= D3D11_USAGE_DYNAMIC;
			InsBufferDesc.ByteWidth				= sizeof(InstanceStruct) * max_instances_number;
			InsBufferDesc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
			InsBufferDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
			InsBufferDesc.MiscFlags				= 0;
			InsBufferDesc.StructureByteStride	= sizeof(T);

			result = DX11_GLOBAL::Device->CreateBuffer(&InsBufferDesc, nullptr, &gpu_buffer);
			if (FAILED(result))
			{
				throw std::runtime_error("error - CDX_Instances constructor error : Instance buffer failed to created");
				return;
			}
		}

		inline void add_element(const T &element) { cpu_buffer.push_back(element); }

		void update()
		{
			HRESULT result;
			D3D11_MAPPED_SUBRESOURCE map_buffer;
			
			//map for write...

			result = DX11_GLOBAL::DeviceContext->Map(gpu_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map_buffer);
			if (FAILED(result))
			{
				throw std::runtime_error("error - CDX_Instances 'update'-functions : instanecs hade problem to update buffer");
				return;
			}

			T *MapPointer = (T*)map_buffer.pData;

			//copy data to buffer...

			memcpy(MapPointer, cpu_buffer.data(), sizeof(T) * cpu_buffer.size());

			DX11_GLOBAL::DeviceContext->Unmap(gpu_buffer, 0);
		}

		//render...
		
		void render(ID3D11Buffer *vertex_buffer, UINT32 vertex_num, UINT32 vertex_stride)
		{
			update();

			unsigned int offsets[2] = { 0,0 };
			unsigned int strides[2];
			
			strides[0] = vertex_stride;
			strides[1] = sizeof(T);

			ID3D11Buffer* bufferPointers[2];
			bufferPointers[0] = vertex_buffer;
			bufferPointers[1] = gpu_buffer;

			DX11_GLOBAL::DeviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
			DX11_GLOBAL::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			DX11_GLOBAL::DeviceContext->DrawInstanced(vertex_num, cpu_buffer.size(), 0, 0);
		}
		
		//void render(CMesh *mesh);

		//clear...

		void clear_elements()
		{
			cpu_buffer.clear();
			
			//GPU Releases...
		}


		~CDX_Instances()
		{
			//Release gpu memory...

			clear_elements();
			if(gpu_buffer) gpu_buffer->Release();
		}
};