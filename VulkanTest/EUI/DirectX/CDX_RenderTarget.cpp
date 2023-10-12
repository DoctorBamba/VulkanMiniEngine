#include "CDX_RenderTarget.h"


//The next function build CDX_RenderTarget object for the window swapchain...
CDX_RenderTarget::CDX_RenderTarget(IDXGISwapChain *SwapChain) : CDX_RenderTarget()
{
	HRESULT result = S_OK;

	//Part1 - color
		
		ID3D11Texture2D* color_buffer = nullptr;

		DXGI_SWAP_CHAIN_DESC swapchain_desc;
		ZeroMemory(&swapchain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));
		SwapChain->GetDesc(&swapchain_desc);
		wight	= swapchain_desc.BufferDesc.Width;
		height	= swapchain_desc.BufferDesc.Height;

		if (wight <= 0 || height <= 0)
		{
			throw std::runtime_error("CDX_RenderTarget's constructor error - rendertarget dimantions not passible(them can't by zero or negtive numbers)");
			return;
		}

		//Save color buffer's pointer at texture2D...

		result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&color_buffer));
		if (FAILED(result))
		{
			throw std::runtime_error("CDX_RenderTarget's constructor error - swap chain buffer is not exsist or corrupt");
			return;
		}

		//Create render target from texture pointer...
		result = DX11_GLOBAL::Device->CreateRenderTargetView(color_buffer, nullptr, &color_target);
		if (FAILED(result))
		{
			throw std::runtime_error("CDX_RenderTarget's constructor error - color target failed to created");
			return;
		}

		color_success = true;

	//Part2 - depth, stencil
		ID3D11Texture2D* depth_stencil_buffer = nullptr;

		//The description of the depth-stencil-texture object...
		D3D11_TEXTURE2D_DESC depth_texture_desc;
		ZeroMemory(&depth_texture_desc, sizeof(depth_texture_desc));
		depth_texture_desc.Width				= wight;
		depth_texture_desc.Height				= height;
		depth_texture_desc.MipLevels			= 1;
		depth_texture_desc.ArraySize			= 1;
		depth_texture_desc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;//3 bytes for depth, one for stencil
		depth_texture_desc.SampleDesc.Count		= 1;
		depth_texture_desc.SampleDesc.Quality	= 0;
		depth_texture_desc.Usage				= D3D11_USAGE_DEFAULT;
		depth_texture_desc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
		depth_texture_desc.CPUAccessFlags		= 0;
		depth_texture_desc.MiscFlags			= 0;

		result = DX11_GLOBAL::Device->CreateTexture2D(&depth_texture_desc, nullptr, &depth_stencil_buffer);
		if (FAILED(result))
		{
			throw std::runtime_error("CDX_RenderTarget's constructor error - depth_stencil texture/buffer failed to created");
			return;
		}

		//The description of the depth-stencil-view object...
		D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
		ZeroMemory(&dsv_desc, sizeof(dsv_desc));
		dsv_desc.Format				= dsv_desc.Format;
		dsv_desc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Texture2D.MipSlice = 0;

		result = DX11_GLOBAL::Device->CreateDepthStencilView(depth_stencil_buffer, &dsv_desc, &depth_stencil_target);
		if (FAILED(result))
		{
			throw std::runtime_error("Create CDX_RenderTarget error - craete depth-stancil-view failed");
			return;
		}

		depth_success = true;
}

CDX_RenderTarget::CDX_RenderTarget(UINT16 wight, UINT16 height, CDX_TEXTURE_FORMAT color_format, CDX_TEXTURE_FORMAT depth_format) : CDX_RenderTarget()
{
	HRESULT result = S_OK;

	//Sep parameters

	this->wight		= wight;
	this->height	= height;
	
	if (wight == 0 || height == 0)
	{
		throw std::runtime_error("Create CDX_RenderTarget error - rendertarget dimantions not passible(them can't by zero)");
		return;
	}

	//Part1 - color...
		if (color_format != TF_NOTHING)
		{
			color_texture = new CD3D11_Texture2D(wight, height, color_format, nullptr);

			//The description of the renter-target-view object...
			D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
			ZeroMemory(&rtv_desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
			rtv_desc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
			rtv_desc.Texture2D.MipSlice = 0;

			switch (color_format)
			{
				case TF_HALF_VECTOR:
					rtv_desc.Format		= DXGI_FORMAT_R16G16B16A16_FLOAT;
					break;
				case TF_DOUBLE_UINT16:
					rtv_desc.Format		= DXGI_FORMAT_R32G32_UINT;
					break;
				case TF_COLORMAP_R8G8B8A8:
					rtv_desc.Format		= DXGI_FORMAT_R8G8B8A8_UNORM;
					break;
			}
		
			//Create the render target...
			result = DX11_GLOBAL::Device->CreateRenderTargetView(color_texture->Get_Buffer(), &rtv_desc, &color_target);
			if (FAILED(result))
			{
				throw std::runtime_error("CDX_RenderTarget's constructor error - failed to create color target");
				return;
			}

			color_success = true;
		}

	//Part2 - depth, stencil...
		if (depth_format)
		{
			depth_stencil_texture = new CD3D11_Texture2D(wight, height, depth_format, nullptr);

			//The description of the depth-stencil-view object...
			D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
			ZeroMemory(&dsv_desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			dsv_desc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
			dsv_desc.Texture2D.MipSlice	= 0;

			switch (depth_format)
			{
				case TF_DEPTHMAP_16F:
					dsv_desc.Format		= DXGI_FORMAT_R16_FLOAT;
					break;
				case TF_DEPTHMAP_32F:
					dsv_desc.Format		= DXGI_FORMAT_D32_FLOAT;
					break;
				case TF_DEPTHSTENCIL_24D8S:
					dsv_desc.Format		= DXGI_FORMAT_D32_FLOAT;
					break;
			}

			//Create the depth-stencil-view...
			result = DX11_GLOBAL::Device->CreateDepthStencilView(depth_stencil_texture->Get_Buffer(), &dsv_desc, &depth_stencil_target);
			if (FAILED(result))
			{
				throw std::runtime_error("CDX_RenderTarget error - failed to create depth target");
				return;
			}

			depth_success = true;
		}
}

void CDX_RenderTarget::Use(UINT16 faceside)
{
	if (faceside == FACE_CULL_NONE)
		DX11_GLOBAL::DeviceContext->RSSetState(DX11_GLOBAL::RASTERIZER_INERT);
	else if (faceside == FACE_CULL_BACK)
		DX11_GLOBAL::DeviceContext->RSSetState(DX11_GLOBAL::RASTERIZER_CULL_BACK);
	
	if (color_success && depth_success)
		DX11_GLOBAL::DeviceContext->OMSetRenderTargets(1, &color_target, depth_stencil_target);
	
	if (color_success && !depth_success)
		DX11_GLOBAL::DeviceContext->OMSetRenderTargets(1, &color_target, nullptr);
	
	if (!color_success && depth_success)
		DX11_GLOBAL::DeviceContext->OMSetRenderTargets(0, nullptr, depth_stencil_target);

}

void CDX_RenderTarget::Use(UINT16 faceside, UINT uav_number, ID3D11UnorderedAccessView* const* uav)
{
	if (faceside == FACE_CULL_NONE)
		DX11_GLOBAL::DeviceContext->RSSetState(DX11_GLOBAL::RASTERIZER_INERT);
	else if (faceside == FACE_CULL_BACK)
		DX11_GLOBAL::DeviceContext->RSSetState(DX11_GLOBAL::RASTERIZER_CULL_BACK);
	
	if (color_success && depth_success)
		DX11_GLOBAL::DeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &color_target, depth_stencil_target, 1, uav_number, uav, 0);

	if (color_success && !depth_success)
		DX11_GLOBAL::DeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &color_target, 0, 0, uav_number, uav, 0);

	if (!color_success && depth_success)
		DX11_GLOBAL::DeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0, 0, depth_stencil_target, 1, uav_number, uav, 0);

}

void CDX_RenderTarget::ClearBuffers(XMVECTORF32 clear_color)
{
	if (color_success)
		DX11_GLOBAL::DeviceContext->ClearRenderTargetView(color_target, clear_color);//Active color-clean program
	if (depth_success)
		DX11_GLOBAL::DeviceContext->ClearDepthStencilView(depth_stencil_target, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);//Active depth-stencil-clean program
}

//Delete buffers...

CDX_RenderTarget::~CDX_RenderTarget()
{
	//Reverse release objects...

	if (color_target)			color_target->Release();
	if (depth_stencil_target)	depth_stencil_target->Release();

	if (color_texture)			delete color_texture;
	if (depth_stencil_texture)	delete depth_stencil_texture;
}


//Global functions

void Set_DXViewport(FLOAT x, FLOAT y, FLOAT wight, FLOAT height)
{
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width    = wight;
	viewport.Height	  = height;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;

	DX11_GLOBAL::DeviceContext->RSSetViewports(1, &viewport);
}

void Get_DXViewport(FLOAT* x_out, FLOAT* y_out, FLOAT* width_out, FLOAT* height_out)
{
	UINT number = 1;
	D3D11_VIEWPORT viewport;

	DX11_GLOBAL::DeviceContext->RSGetViewports(&number, &viewport);

	*x_out = viewport.TopLeftX;
	*y_out = viewport.TopLeftY;
	*width_out = viewport.Width;
	*height_out = viewport.Height;
}