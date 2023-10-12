#pragma once

#include "DX_Workspace.h"

#define RO_CPU_CAN_WRITE_BUFFER 0x000001
#define RO_GPU_CAN_WRITE_BUFFER 0x000002
#define RO_USAGE_DYNAMIC_BUFFER 0x000004
#define RO_STRUCTURE_BUFFER		0x000010

#define CPU_CAN_WRITE_RESOURCE 0X000001 
#define CPU_CAN_READ_RESOURCE 0X000002
#define CPU_CAN_READ_WRITE_RESOURCE 0X000003

typedef void *Pointer;

class CDX_SRVObject
{
	protected:
		ID3D11Buffer *Buffer;
		UINT32 BufferFlags;
		UINT32 BufferSize;
		UINT32 BufferStride;
		Pointer dataPointer;

	public:
		ID3D11ShaderResourceView *ShaderResouceView;

	public:

		CDX_SRVObject(UINT32 max_num_of_element, UINT32 stride, const void *data_pointer, UINT32 flags);
};

class CDX_UAccessObject
{
	protected:
		ID3D11Buffer *Buffer;
		UINT32 BufferFlags;
		UINT32 BufferSize;
		UINT32 BufferStride;
		Pointer dataPointer;

	public:
		ID3D11UnorderedAccessView *ShaderUnorderedAccessView;

	public:

		CDX_UAccessObject(UINT32 max_num_of_element, UINT32 stride, const void *data_pointer, UINT32 flags);
};