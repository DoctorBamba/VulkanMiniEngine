#include "CDX_RenderTarget.h"

//Define DX_GLOBAL namespace...

namespace DX11_GLOBAL
{
	/*Engine basic DX-object...*/

	XMUINT2 ScreenDimantions;

	IDXGIFactory1*			Factory;
	ID3D11Device*			Device;                    
	ID3D11Device1*			Device1;
	ID3D11DeviceContext*	DeviceContext;          
	ID3D11DeviceContext1*	DeviceContext1;

	IDXGISwapChain*			SwapChain;
	IDXGISwapChain1*		SwapChain1;
	
	CDX_RenderTarget*		MainTarget;

	//A global useful DX-Objects...

	ID3D11RasterizerState*		RASTERIZER_INERT;
	ID3D11RasterizerState*		RASTERIZER_CULL_BACK;

	ID3D11BlendState*			BLENDSTATE_INERT;
	ID3D11BlendState*			BLENDSTATE_REGULAR_ALPHA;
	ID3D11BlendState*			BLENDSTATE_ADDBLENDING_COLOR;

	ID3D11DepthStencilState*	DEPTH_TEXT_REGULAR;
	ID3D11DepthStencilState*	DEPTH_TEXT_INERT;

	ID3D11SamplerState*			SAMPLAR_STATE_NEAREST;
	ID3D11SamplerState*			SAMPLAR_STATE_LINEAR_FILLTERING;

	ID3D11Buffer*				VERTEX_BUFFER_FRAMEQUAD;
};

HRESULT DX11_GLOBAL::InstallUsefulRasterizers()
{
	HRESULT result = S_OK;

	D3D11_RASTERIZER_DESC rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_NONE;
	rasterizerState.FrontCounterClockwise = false;
	
	DX11_GLOBAL::Device->CreateRasterizerState(&rasterizerState, &DX11_GLOBAL::RASTERIZER_INERT);
	if (FAILED(result))
	{
		throw std::runtime_error("InstallUsefulRasterizers error - DX_GLOBAL::RASTERIZER_INERT faild to create");
		return result;
	}

	rasterizerState.CullMode = D3D11_CULL_NONE;

	DX11_GLOBAL::Device->CreateRasterizerState(&rasterizerState, &DX11_GLOBAL::RASTERIZER_CULL_BACK);
	if (FAILED(result))
	{
		throw std::runtime_error("InstallUsefulRasterizers error - DX_GLOBAL::RASTERIZER_CULL_BACK faild to create");
		return result;
	}

	return result;
}

HRESULT DX11_GLOBAL::InstallUsefulBlendStates()
{
	//The regular alpha blend state...

	HRESULT result = S_OK;
	D3D11_BLEND_DESC desc;

	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
	desc.RenderTarget[0].BlendEnable			= true;
	desc.RenderTarget[0].SrcBlend				= D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

	result = DX11_GLOBAL::Device->CreateBlendState(&desc, &DX11_GLOBAL::BLENDSTATE_REGULAR_ALPHA);
	if (FAILED(result))
	{
		throw std::runtime_error("InstallUsefulBlendStates error - DX_GLOBAL::BLENDSTATE_REGULAR_ALPHA faild to create");
		return result;
	}

	//The addblending color blend state...

	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
	desc.RenderTarget[0].BlendEnable			= true;
	desc.RenderTarget[0].SrcBlend				= D3D11_BLEND_SRC_COLOR;
	desc.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_COLOR;
	desc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_MAX;
	desc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

	result = DX11_GLOBAL::Device->CreateBlendState(&desc, &DX11_GLOBAL::BLENDSTATE_ADDBLENDING_COLOR);
	if (FAILED(result))
	{
		throw std::runtime_error("InstallUsefulBlendStates error - DX_GLOBAL::BLENDSTATE_ADDBLENDING_COLOR faild to create");
		return result;
	}

	desc.RenderTarget[0].BlendEnable = false;
	result = DX11_GLOBAL::Device->CreateBlendState(&desc, &DX11_GLOBAL::BLENDSTATE_INERT);
	if (FAILED(result))
	{
		throw std::runtime_error("InstallUsefulBlendStates error - DX_GLOBAL::BLENDSTATE_INERT faild to create");
		return result;
	}

	DX11_GLOBAL::DeviceContext->OMSetBlendState(DX11_GLOBAL::BLENDSTATE_REGULAR_ALPHA, 0, 0xffffffff);//Set to defult

	return result;
}

HRESULT DX11_GLOBAL::InstallUsefulDepthStates()
{
	HRESULT result = S_OK;
	D3D11_DEPTH_STENCIL_DESC desc;
	
	//The regular depth test state...

	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	desc.DepthEnable						= true;
	desc.DepthWriteMask						= D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc							= D3D11_COMPARISON_LESS;

	// Stencil test parameters
	desc.StencilEnable						= true;
	desc.StencilReadMask					= 0xFF;
	desc.StencilWriteMask					= 0xFF;

	// Stencil operations if pixel is front-facing
	desc.FrontFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp		= D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc				= D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	desc.BackFace.StencilFailOp				= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp		= D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp				= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc				= D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	result = DX11_GLOBAL::Device->CreateDepthStencilState(&desc, &DX11_GLOBAL::DEPTH_TEXT_REGULAR);
	if (FAILED(result))
	{
		throw std::runtime_error("InstallUsefulDepthStates error - DX_GLOBAL::DEPTH_TEXT_REGULAR faild to create");
		return result;
	}

	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	desc.DepthEnable						= false;
	desc.DepthWriteMask						= D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc							= D3D11_COMPARISON_ALWAYS;

	// Stencil test parameters
	desc.StencilEnable						= false;
	desc.StencilReadMask					= 0xFF;
	desc.StencilWriteMask					= 0xFF;

	// Stencil operations if pixel is front-facing
	desc.FrontFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp		= D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc				= D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	desc.BackFace.StencilFailOp				= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp		= D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp				= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc				= D3D11_COMPARISON_ALWAYS;

	result = DX11_GLOBAL::Device->CreateDepthStencilState(&desc, &DX11_GLOBAL::DEPTH_TEXT_INERT);
	if (FAILED(result))
	{
		throw std::runtime_error("InstallUsefulDepthStates error - DX_GLOBAL::DEPTH_TEXT_INERT faild to create");
		return result;
	}
	return result;
}

HRESULT DX11_GLOBAL::InstallUsefulSamplerStates()
{
	HRESULT result = S_OK;
	D3D11_SAMPLER_DESC desc;
	
	//Create nearest-sampler state
	ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));

	desc.Filter			= D3D11_FILTER_MIN_MAG_MIP_POINT;
	desc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD			= 0;
	desc.MaxLOD			= D3D11_FLOAT32_MAX;

	result = DX11_GLOBAL::Device->CreateSamplerState(&desc, &DX11_GLOBAL::SAMPLAR_STATE_NEAREST);
	if (FAILED(result))
	{
		throw std::runtime_error("InstallUsefulSamplerStates error - DX_GLOBAL::SAMPLAR_STATE_NEAREST faild to create");
		return result;
	}

	//Create nearest-sampler state
	ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));

	desc.Filter				= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc		= D3D11_COMPARISON_NEVER;
	desc.MinLOD				= 0;
	desc.MaxLOD				= D3D11_FLOAT32_MAX;

	result = DX11_GLOBAL::Device->CreateSamplerState(&desc, &DX11_GLOBAL::SAMPLAR_STATE_LINEAR_FILLTERING);
	if (FAILED(result))
	{
		throw std::runtime_error("InstallUsefulSamplerStates error - DX_GLOBAL::SAMPLAR_STATE_LINEAR_FILLTERING faild to create");
		return result;
	}

	return result;
}

HRESULT DX11_GLOBAL::InstallUsefulMeshBuffers()
{
	HRESULT result = S_OK;

	//Create a quad buffer

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
	
	vertexBufferDesc.Usage					= D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth				= sizeof(PE_Vector3D) * 6;
	vertexBufferDesc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags			= 0;
	vertexBufferDesc.MiscFlags				= 0;
	vertexBufferDesc.StructureByteStride	= sizeof(PE_Vector3D);

	// Give the subresource structure a pointer to the vertex data.

	const FLOAT VertexsData[] = { 1.0f, 1.0f, 0.0f,		-1.0, -1.0f, 0.0f,		 1.0f, -1.0f, 0.0f,
								  1.0f, 1.0f, 0.0f,		-1.0, 1.0f, 0.0f,		-1.0f, -1.0f, 0.0f };

	vertexData.pSysMem = &VertexsData;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = DX11_GLOBAL::Device->CreateBuffer(&vertexBufferDesc, &vertexData, &DX11_GLOBAL::VERTEX_BUFFER_FRAMEQUAD);      // create the buffer        

	if (FAILED(result))
	{
		throw std::runtime_error("InstallUsefulMeshBuffers error - feiled to create DX_VERTEX_BUFFER_BOX buffer");
		return result;
	}

	return result;
}

HRESULT DX11_GLOBAL::InstallObjects()
{
	HRESULT result = S_OK;
	result = InstallUsefulRasterizers();
	result = InstallUsefulBlendStates();
	result = InstallUsefulDepthStates();
	result = InstallUsefulSamplerStates();
	result = InstallUsefulMeshBuffers();
	result = InstallUsefulMeshBuffers();
	return result;
}

//Draw functions...

void DX11_GLOBAL::DrawFrameQuad()
{
	UINT stride = sizeof(FLOAT) * 2;
	UINT offset = 0;

	DX11_GLOBAL::DeviceContext->IASetVertexBuffers(0, 1, &DX11_GLOBAL::VERTEX_BUFFER_FRAMEQUAD, &stride, &offset);

	DX11_GLOBAL::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //Set geometric to triangels
	DX11_GLOBAL::DeviceContext->Draw(6, 0);
}