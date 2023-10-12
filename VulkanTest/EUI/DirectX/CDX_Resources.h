#pragma once

#include "DX_Workspace.h"
#include "FreeImage.h"
#include <fstream>

//Class base shader-resource
class CDX_ShaderResource
{
	protected:
		ID3D11ShaderResourceView	*resource_srv;
		BOOL						 load_succesed;

		virtual HRESULT CreateResource(BYTE* data) { return S_OK; }

	public:
		CDX_ShaderResource()
		{
			resource_srv	= nullptr;
			load_succesed	= false;
		}

		~CDX_ShaderResource()
		{
			if (resource_srv) resource_srv->Release();
			load_succesed = false;
		}

		ID3D11ShaderResourceView* Get_SRV() { return resource_srv; }
		BOOL IsSuccese() { return load_succesed; }

		void TackInVertexShader(UINT32 bound_index) { DX11_GLOBAL::DeviceContext->VSSetShaderResources(bound_index, 1, &resource_srv); }
		void TackInPixelShader(UINT32 bound_index) { DX11_GLOBAL::DeviceContext->PSSetShaderResources(bound_index, 1, &resource_srv); }
};

/****************************************Texture2D resource********************************************/

enum CDX_TEXTURE_FORMAT
{
	TF_NOTHING,
	TF_DEPTHMAP_8F,
	TF_DEPTHMAP_16F,
	TF_DEPTHMAP_24F,
	TF_DEPTHMAP_32F,
	TF_COLORMAP_R8,
	TF_COLORMAP_R8G8,
	TF_COLORMAP_R8G8B8,
	TF_COLORMAP_R8G8B8A8,
	TF_DEPTHSTENCIL_24D8S,
	TF_HALF_VECTOR,
	TF_DOUBLE_UINT16
};

enum CDX_TEXTURE_LOAD_TYPE
{
	CDX_LOAD_COLOR_TEXTURE,
	CDX_LOAD_DEPTHMAP
};

UINT8 CDX_GetBytesPerPixel(CDX_TEXTURE_FORMAT texture_format);
void  CDX_SaveTextureAsIcon(std::string image_dir, std::string icon_out_dir);

//Texture2D class
class CD3D11_Texture2D : public CDX_ShaderResource
{
	private:
		BOOL use_as_target;
		
	protected:
		ID3D11Texture2D*	texture;
		CDX_TEXTURE_FORMAT	texture_format;
		UINT16				tex_width;
		UINT16				tex_height;
		
		HRESULT CreateResource(BYTE* data);
		
	public:
		CD3D11_Texture2D() : CDX_ShaderResource()
		{
			use_as_target	= false;
			texture			= nullptr;
			texture_format	= TF_NOTHING;
			tex_width		= 0U;
			tex_height		= 0U;
		}

		CD3D11_Texture2D(ID3D11Texture2D* dx11_texture, DXGI_FORMAT srv_format);
		CD3D11_Texture2D(std::string filename, UINT16 loadtype);
		CD3D11_Texture2D(UINT16 wight, UINT16 height, CDX_TEXTURE_FORMAT format, const void* data);
		~CD3D11_Texture2D();

		//Get functions...

		UINT16			 Get_Wight() { return tex_width; }
		UINT16			 Get_Height() { return tex_height; }

		UINT32			 Get_BytePerPixel() { return CDX_GetBytesPerPixel(texture_format); }
		UINT32			 Get_TexturePitch() { return tex_width * Get_BytePerPixel(); }

		ID3D11Texture2D* Get_Buffer() { return texture; }
};


/****************************************Icon resource********************************************/

enum EUI_ICON_FORMAT
{
	EUI_ICON8X8,
	EUI_ICON16X16,
	EUI_ICON24X24,
	EUI_ICON32X32
};

//Icon class
class CDX_Icon : public CDX_ShaderResource
{
	private:
		EUI_ICON_FORMAT		 icon_format;
		ID3D11Texture2D		*icon_tex;

		HRESULT CreateResource(BYTE* data);

	public:

		CDX_Icon(std::string directory);

		UINT16 Get_Dimantion()
		{
			UINT16 dimantion = 0U;
			switch (icon_format)
			{
			case EUI_ICON8X8:
				dimantion = 8U;
				break;
			case EUI_ICON16X16:
				dimantion = 16U;
				break;
			case EUI_ICON24X24:
				dimantion = 24U;
				break;
			case EUI_ICON32X32:
				dimantion = 36U;
				break;
			}

			return dimantion;
		}

		EUI_ICON_FORMAT Get_Format() { return icon_format; }
};