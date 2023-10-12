#include "CDX_Object.h"

int roundUp(int numToRound, int multiple)
{
	if (multiple == 0)
	{
		return numToRound;
	}

	int roundDown = ((int)(numToRound) / multiple) * multiple;
	int roundUp = roundDown + multiple;
	int roundCalc = roundUp;
	return (roundCalc);
}

CDX_Object::CDX_Object(UINT32 buffer_size, UINT32 buffer_stride)
{
	HRESULT result;

	//Create buffer description

	if (!buffer_size) { return; }

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = roundUp(buffer_size, 16);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = buffer_stride;

	result = DX11_GLOBAL::Device->CreateBuffer(&bufferDesc, NULL, &Buffer);

	if (FAILED(result))
	{
		throw "error - the buffer fialed to created";
		return;
	}
}

void CDX_Object::TackInVertexShader(UINT32 buffer_index)
{
	DX11_GLOBAL::DeviceContext->VSSetConstantBuffers(buffer_index, 1, &Buffer);
}

void CDX_Object::TackInPixelShader(UINT32 buffer_index)
{
	DX11_GLOBAL::DeviceContext->PSSetConstantBuffers(buffer_index, 1, &Buffer);
}