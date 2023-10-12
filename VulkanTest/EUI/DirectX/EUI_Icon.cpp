#include "CDX_Resources.h"

CDX_Icon::CDX_Icon(std::string directory)
{
	load_succesed = false;

	std::ifstream icon_file;
	icon_file.open(directory.c_str(), std::ios::binary | std::ios::in);
			
	if (!icon_file.good())
		throw std::runtime_error(std::string("EUI_Icon constructor error - The file ") + (char)34 + directory + (char)34 + std::string(" not found or not supported"));

	UINT16 width	= 0U;
	UINT16 height	= 0U;

	icon_file.read((char*)&width, sizeof(UINT16));
	icon_file.read((char*)&height, sizeof(UINT16));

	if (width == 8 && height == 8)
	{
		icon_format = EUI_ICON8X8;
	}
	else if (width == 16 && height == 16)
	{
		icon_format = EUI_ICON16X16;
	}
	else if (width == 24 && height == 24)
	{
		icon_format = EUI_ICON24X24;
	}
	else if (width == 32 && height == 32)
	{
		icon_format = EUI_ICON32X32;
	}
	else
	{
		throw std::runtime_error("EUI_Icon constructor error - the image dimantions not allow for icon");
		icon_file.close();
		return;
	}

	UINT  sizeof_icon_image = (UINT)width * (UINT)height * 2;
	BYTE* icon_bytes = new BYTE[sizeof_icon_image];
	icon_file.read((char*)icon_bytes, sizeof_icon_image);

	if (!FAILED(CreateResource(icon_bytes)))
		load_succesed = true;
	int y = 5;
}
			
HRESULT CDX_Icon::CreateResource(BYTE* data)
{
	HRESULT result;

	UINT dimantion = 0U;
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

	//Icon's texture description...
	D3D11_TEXTURE2D_DESC texture_desc;
	ZeroMemory(&texture_desc, sizeof(D3D11_TEXTURE2D_DESC));
	texture_desc.Height				= dimantion;
	texture_desc.Width				= dimantion;
	texture_desc.MipLevels			= 1;
	texture_desc.ArraySize			= 1;
	texture_desc.SampleDesc.Count	= 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage				= D3D11_USAGE_DEFAULT;
	texture_desc.CPUAccessFlags		= 0;
	texture_desc.Format				= DXGI_FORMAT_R8G8_TYPELESS;
	texture_desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	texture_desc.MiscFlags			= 0;

	//Icon's subdata...
	D3D11_SUBRESOURCE_DATA sub_data;
	sub_data.pSysMem					= data;
	sub_data.SysMemPitch				= dimantion * 2;
	sub_data.SysMemSlicePitch			= dimantion * dimantion * 2;

	//Create the icon's texture...
	result = DX11_GLOBAL::Device->CreateTexture2D(&texture_desc, &sub_data, &icon_tex);
	if (FAILED(result))
	{
		throw std::runtime_error("EUI_Icon::CreateRecourse error - icon's texture failed to create");
		return result;
	}

	//SRV description...
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	ZeroMemory(&srv_desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srv_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;//it is a 2D texture
	srv_desc.Texture2D.MostDetailedMip	= 0;
	srv_desc.Texture2D.MipLevels		= texture_desc.MipLevels;
	srv_desc.Format						= DXGI_FORMAT_R8G8_UNORM;

	//Create the icon's srv...
	result = DX11_GLOBAL::Device->CreateShaderResourceView(icon_tex, &srv_desc, &resource_srv);
	if (FAILED(result))
	{
		throw std::runtime_error("EUI_Icon::CreateRecourse error - icon's srv failed to create");
		return result;
	}

	return result;
}