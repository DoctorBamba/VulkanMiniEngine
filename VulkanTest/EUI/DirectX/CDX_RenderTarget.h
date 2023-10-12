#pragma once

#include "CDX_GraphicsProgram.h"
#include "CDX_Resources.h"

#define FACE_CULL_NONE		1
#define FACE_CULL_BACK		2

//A Simple render-target object...
class CDX_RenderTarget
{
	protected:

		BOOL						color_success;
		BOOL						depth_success;

		UINT16						wight;
		UINT16						height;

		ID3D11DepthStencilState*	depth_stancil_state;

		CD3D11_Texture2D*				color_texture;
		CD3D11_Texture2D*				depth_stencil_texture;

		ID3D11RenderTargetView*		color_target;
		ID3D11DepthStencilView*		depth_stencil_target;


	public:

		CDX_RenderTarget()
		{
			color_success = false;
			depth_success = false;

			wight	= 0U;
			height	= 0U;

			depth_stancil_state		= nullptr;

			color_target			= nullptr;
			depth_stencil_target	= nullptr;

		}

		CDX_RenderTarget(IDXGISwapChain *SwapChain);
		CDX_RenderTarget(UINT16 wight, UINT16 height, CDX_TEXTURE_FORMAT color_format, CDX_TEXTURE_FORMAT depth_format);
		
		virtual ~CDX_RenderTarget();
		
		void Use(UINT16 cullface);
		void Use(UINT16 faceside, UINT uav_number, ID3D11UnorderedAccessView* const* uav_array);

		void ClearBuffers(XMVECTORF32 clear_color);

		//Get functions

		UINT16 Get_Width()	{ return wight; }
		UINT16 Get_Height()	{ return height; }

		ID3D11RenderTargetView* Get_ColorTarget()			{ return color_target; }
		ID3D11DepthStencilView* Get_DepthStencilTarget()	{ return depth_stencil_target; }

		CD3D11_Texture2D* Get_ColorTexture()			{return color_texture;}
		CD3D11_Texture2D* Get_DepthStencilTexture()	{ return depth_stencil_texture; }

		//Success...

		bool IsColorSuccesed()			{ return color_success; }
		bool IsDepthStencilSuccesed()	{ return depth_success; }
};
//Global functions

void Set_DXViewport(FLOAT x, FLOAT y, FLOAT wight, FLOAT height);
void Get_DXViewport(FLOAT* x_out, FLOAT* y_out, FLOAT* width_out, FLOAT* height_out);

//Expand CDX_GLOBAL namespace...

#ifndef DX_GLOABAL_NAMESPACE__RENDERTARGETS__
#define DX_GLOABAL_NAMESPACE__RENDERTARGETS__

namespace DX11_GLOBAL
{
	/*Programs(here beacuse need CDX_Program)*/

	//Defults...

	extern CDX_RenderTarget* MainTarget;
};

#endif