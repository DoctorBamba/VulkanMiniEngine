#pragma once

#include "CDX_SRVVector.h"
#include "CDX_UAccessVector.h"

//Pixels format

typedef struct 
{
	UINT32 Format;
	UINT8 SizeInByte;
}SP_FORMAT;

static SP_FORMAT SPF_R8 = { 1, 1 };
static SP_FORMAT SPF_R8G8 = { 2, 2 };
static SP_FORMAT SPF_R8G8B8 = { 3, 3 };
static SP_FORMAT SPF_R8G8B8A8 = { 4, 4 };
static SP_FORMAT SPF_R32F = { 5, 4 };
static SP_FORMAT SPF_R32G32F = { 6, 8 };
static SP_FORMAT SPF_R32G32B32F = { 7, 12 };
static SP_FORMAT SPF_R32G32B32A32F = { 8, 16 };

struct SGPU_Samplar
{
	UINT32 wight;
	UINT32 height;
	UINT32 format;
	UINT32 mem_offset;
};

class CDX_RTexture2DStream
{
	private:
		
		CDX_UAccessVector<FLOAT> *TexturesResource;
		CDX_UAccessVector<SGPU_Samplar> *SamplersResource;
		
		UINT32 TexturesNum;
		UINT32 Memory_Offset;

	public :
		
		ID3D11UnorderedAccessView *GetTexturesAccess() { return TexturesResource->ShaderUnorderedAccessView; }
		ID3D11UnorderedAccessView *GetReadersAccess() { return SamplersResource->ShaderUnorderedAccessView; };

		CDX_RTexture2DStream(UINT32 resurce_limit, UINT32 max_textures);

		HRESULT AddTexture(UINT16 wight, UINT16 height, SP_FORMAT format, Pointer Data);
		
};