#pragma once

#include "DX_Workspace.h"//Resource header

//Buffer state

#define CO_CPU_BUFFER_WRITE 0x0001
#define CO_GPU_BUFFER_WRITE 0x0002
#define CO_USAGE_DYNAMIC_BUFFER 0x0004

/*gpu object class*/

 class CDX_Object
{
	public:

		CDX_Object(UINT32 state, UINT32 buffer_size);

		void TackInVertexShader(UINT32 buffer_index);
		void TackInPixelShader(UINT32 buffer_index);

	protected:

		ID3D11Buffer *Buffer;
		UINT32 BufferSize;

		UINT16 BufferState;
		D3D11_MAP MapState;
};