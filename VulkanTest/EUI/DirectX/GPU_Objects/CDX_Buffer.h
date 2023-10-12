#pragma once

#include "CDX_Object.h"

template<class T> class CDX_Buffer : public CDX_Object
{

	public:

		CDX_Buffer();

		void set(const T &value);
};

template<class T> CDX_Buffer<T>::CDX_Buffer() : CDX_Object(sizeof(T), 0)
{
	
}


template<class T> void CDX_Buffer<T>::set(const T &value)
{
	D3D11_MAPPED_SUBRESOURCE BufferMap;
	if (FAILED(DX11_GLOBAL::DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &BufferMap)))
	{
		throw "error - map fialed";
		return;
	}

	T *DataPointer = (T*)BufferMap.pData;
	memcpy(DataPointer, &value, sizeof(T));

	DX11_GLOBAL::DeviceContext->Unmap(Buffer, 0);
}