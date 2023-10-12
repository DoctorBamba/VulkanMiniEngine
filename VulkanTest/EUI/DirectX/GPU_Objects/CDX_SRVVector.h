#pragma once

#include "CDX_RObject.h"

template<class T> class CDX_SRVVector : public CDX_SRVObject
{
	protected:

		UINT32 element_num;

	public:

		CDX_SRVVector(UINT32 max_num_of_elements, UINT32 flags) : CDX_SRVObject(max_num_of_elements, sizeof(T), nullptr, flags) {}
		CDX_SRVVector(const std::vector<T> &install_data, UINT32 flags) : CDX_SRVObject(install_data.size(), sizeof(T), install_data.data(), flags) {}

		void TackInPixelShader(UINT index);
	public:

		HRESULT push_back(const T &new_element);
		HRESULT at_W(UINT32 index, T data);
		const T &at_R(UINT32 index);
		HRESULT set(std::vector<T> *Data);
		HRESULT copyBytes(UINT32 offset, Pointer data_pointer, SIZE_T size_in_element);
};

template<class T> HRESULT CDX_SRVVector<T>::push_back(const T &new_element)
{
	HRESULT result = 0;

	if ((BufferFlags & RO_CPU_CAN_WRITE_BUFFER) && !(BufferFlags & RO_USAGE_DYNAMIC_BUFFER))
	{
		element_num++;

		D3D11_MAPPED_SUBRESOURCE Map;

		result = DX11_GLOBAL::DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE, 0, &Map);
		if (FAILED(result))
		{
			return result;
		}

		T *DataPointer = (T*)Map.pData;
		memcpy(&DataPointer[element_num - 1], &new_element, sizeof(T));

		DX11_GLOBAL::DeviceContext->Unmap(Buffer, 0);
	}
	else
	{
		throw "error - Buffer flags not appropriate for push_back";
		return -1;
	}

	return result;
}

template<class T> HRESULT CDX_SRVVector<T>::set(std::vector<T> *data)
{
	HRESULT result = S_OK;


	if (!(data->size() > 0))
	{

		return result;
	}

	if (BufferFlags & RO_CPU_CAN_WRITE_BUFFER)
	{
		D3D11_MAPPED_SUBRESOURCE Map;

		D3D11_MAP mapFlags;

		if (BufferFlags & RO_USAGE_DYNAMIC_BUFFER)
		{
			mapFlags = D3D11_MAP_WRITE_DISCARD;
		}
		else
		{
			mapFlags = D3D11_MAP_WRITE;
		}

		result = DX11_GLOBAL::DeviceContext->Map(Buffer, 0, mapFlags, 0, &Map);
		if (FAILED(result))
		{
			throw "error - map failed";
			return result;
		}

		T *DataPointer = (T*)Map.pData;
		memcpy(&DataPointer[0], data->data(), sizeof(T) * data->size());



		DX11_GLOBAL::DeviceContext->Unmap(Buffer, 0);
	}
	else
	{
		throw "error - Buffer flags not appropriate for cpu update";
		return -1;
	}

	return result;
}

template<class T> HRESULT CDX_SRVVector<T>::copyBytes(UINT32 offset, Pointer data_pointer, SIZE_T size_in_element)
{
	HRESULT result = S_OK;

	if (!data_pointer)
	{
		return 0;
	}

	if (BufferFlags & RO_CPU_CAN_WRITE_BUFFER)
	{
		D3D11_MAPPED_SUBRESOURCE Map;

		D3D11_MAP mapFlags;

		if (BufferFlags & RO_USAGE_DYNAMIC_BUFFER)
		{
			mapFlags = D3D11_MAP_WRITE_DISCARD;

			if (offset > 0)
			{
				throw "error - offset cant by 0 in write discard mode";
				return result;
			}
		}
		else
		{
			mapFlags = D3D11_MAP_WRITE;
		}

		result = DX11_GLOBAL::DeviceContext->Map(Buffer, 0, mapFlags, 0, &Map);

		if (FAILED(result))
		{
			throw "error - map failed";
			return result;
		}

		T *DataPointer = (T*)Map.pData;

		memcpy(DataPointer, data_pointer, sizeof(T) * size_in_element);

		DX11_GLOBAL::DeviceContext->Unmap(Buffer, 0);
	}
	else
	{
		throw "error - Buffer flags not appropriate for cpu update";
		return -1;
	}

	return result;
}

template<class T> HRESULT CDX_SRVVector<T>::at_W(UINT32 index, T data)
{
	HRESULT result = 0;

	if ((BufferFlags & RO_CPU_CAN_WRITE_BUFFER) && !(BufferFlags & RO_USAGE_DYNAMIC_BUFFER))
	{
		D3D11_MAPPED_SUBRESOURCE Map;

		result = DX11_GLOBAL::DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE, 0, &Map);
		if (FAILED(result))
		{
			return result;
		}

		T *DataPointer = (T*)Map.pData;
		memcpy(&DataPointer[index], &data, sizeof(T));

		DX11_GLOBAL::DeviceContext->Unmap(Buffer, 0);
	}
	else
	{
		throw "error - Buffer flags not appropriate for at_W function";
		return -1;
	}

	return result;
}

template<class T> const T &CDX_SRVVector<T>::at_R(UINT32 index)
{
	T out;

	if (BufferFlags & CPU_CAN_READ_RESOURCE)
	{
		D3D11_MAPPED_SUBRESOURCE Map;

		HRESULT result = DX11_GLOBAL::DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE, 0, &Map);
		if (FAILED(result))
		{
			return result;
		}

		T *DataPointer = (T*)Map.pData;

		memcpy(&out, &DataPointer[element_num - 1], sizeof(T));

		DX11_GLOBAL::DeviceContext->Unmap(Buffer, 0);
	}

	return (const T&)&out;
}

template<class T> void CDX_SRVVector<T>::TackInPixelShader(UINT index)
{
	DX11_GLOBAL::DeviceContext->PSSetShaderResources(index, 1, &ShaderResouceView);
}