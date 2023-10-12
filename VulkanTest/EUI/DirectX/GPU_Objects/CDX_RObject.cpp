#include "CDX_RObject.h"

struct SLightBlock
{
	XMFLOAT3 Position;
	XMFLOAT3 Direction;
	FLOAT Power;
	FLOAT Distance;

	XMVECTOR color;
	XMVECTOR ambient;
	XMVECTOR diffuse;
	XMVECTOR specular;
	XMVECTOR emssive;
	XMVECTOR shininess;
};

CDX_SRVObject::CDX_SRVObject(UINT32 max_num_of_element, UINT32 stride, const void *data_pointer, UINT32 flags)
{
	BufferFlags = flags;
	BufferStride = stride;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	//Set flags

	if (!(flags & RO_CPU_CAN_WRITE_BUFFER) && !(flags & RO_GPU_CAN_WRITE_BUFFER))
	{
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else if ((flags & RO_CPU_CAN_WRITE_BUFFER) && !(flags & RO_GPU_CAN_WRITE_BUFFER))
	{

		if (flags & RO_USAGE_DYNAMIC_BUFFER)
		{
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		}
		else
		{
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		}

		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else if (!(flags & RO_CPU_CAN_WRITE_BUFFER) && (flags & RO_GPU_CAN_WRITE_BUFFER))
	{
		if (!(flags & RO_USAGE_DYNAMIC_BUFFER))
		{
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		}
		else
		{
			throw "GPU can't write dynamic buffer";
			return;
		}
	}
	else
	{
		throw "GPU & CPU cant write at same time";
		return;
	}

	bufferDesc.ByteWidth = max_num_of_element * stride;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	bufferDesc.StructureByteStride = stride;


	D3D11_SUBRESOURCE_DATA bufferInitData;
	ZeroMemory((&bufferInitData), sizeof(bufferInitData));

	bufferInitData.pSysMem = data_pointer;
	bufferInitData.SysMemPitch = 0;
	bufferInitData.SysMemSlicePitch = 0;

	if (FAILED(DX11_GLOBAL::Device->CreateBuffer(&bufferDesc, (data_pointer) ? (&bufferInitData) : NULL, &Buffer)))
	{
		throw "Create buffer error";
		return;
	}

	//Create the resource...

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = max_num_of_element;

	if (FAILED(DX11_GLOBAL::Device->CreateShaderResourceView(Buffer, &srvDesc, &ShaderResouceView)))
	{
		throw "Create resource error";
		return;
	}
}

CDX_UAccessObject::CDX_UAccessObject(UINT32 max_num_of_element, UINT32 stride, const void *data_pointer, UINT32 flags)
{
	BufferFlags = flags;
	BufferStride = stride;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	//Set flags

	if (!(flags & RO_CPU_CAN_WRITE_BUFFER) && !(flags & RO_GPU_CAN_WRITE_BUFFER))
	{
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else if((flags & RO_CPU_CAN_WRITE_BUFFER) && !(flags & RO_GPU_CAN_WRITE_BUFFER))
	{
		
		if (flags & RO_USAGE_DYNAMIC_BUFFER)
		{
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		}
		else
		{
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		}

		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else if(!(flags & RO_CPU_CAN_WRITE_BUFFER) && (flags & RO_GPU_CAN_WRITE_BUFFER))
	{
		if (!(flags & RO_USAGE_DYNAMIC_BUFFER))
		{
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		}
		else
		{
			throw "GPU can't write dynamic buffer";
			return;
		}
	}
	else
	{
		throw "GPU & CPU cant write at same time";
		return;
	}

	bufferDesc.ByteWidth = max_num_of_element * stride;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	if (flags & RO_STRUCTURE_BUFFER)
	{
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = stride;
	}
	else
	{
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = stride;
	}
	

	D3D11_SUBRESOURCE_DATA bufferInitData;
	ZeroMemory((&bufferInitData), sizeof(bufferInitData));

	bufferInitData.pSysMem = data_pointer;

	if (FAILED(DX11_GLOBAL::Device->CreateBuffer(&bufferDesc, (data_pointer) ? (&bufferInitData) : NULL, &Buffer)))
	{
		throw "Create buffer error";
		return;
	}

	//Create the resource...

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	
	if (flags & RO_STRUCTURE_BUFFER)
	{
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	}
	else
	{
		uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	}

	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = max_num_of_element;

	if(FAILED(DX11_GLOBAL::Device->CreateUnorderedAccessView(Buffer, &uavDesc, &ShaderUnorderedAccessView)))
	{
		throw "Create resource error";
		return;
	}
}