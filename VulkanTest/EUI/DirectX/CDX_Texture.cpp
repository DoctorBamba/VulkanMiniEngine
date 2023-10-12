#include "CDX_Resources.h"

BOOL file_exists(const std::string &directory)
{
	std::ifstream file(directory.c_str());
	return file.good();
}

UINT8 CDX_GetBytesPerPixel(CDX_TEXTURE_FORMAT texture_format)
{
	switch (texture_format)
	{
	case TF_NOTHING:
		return 0U;
		break;
	case TF_DEPTHMAP_8F:
		return 1U;
		break;
	case TF_DEPTHMAP_16F:
		return 2U;
		break;
	case TF_DEPTHMAP_24F:
		return 3U;
		break;
	case TF_DEPTHMAP_32F:
		return 4U;
		break;
	case TF_COLORMAP_R8:
		return 1U;
		break;
	case TF_COLORMAP_R8G8:
		return 2U;
		break;
	case TF_COLORMAP_R8G8B8:
		return 3U;
		break;
	case TF_COLORMAP_R8G8B8A8:
		return 4U;
		break;
	case TF_DEPTHSTENCIL_24D8S:
		return 4U;
		break;
	case TF_HALF_VECTOR:
		return 4U;
		break;
	case TF_DOUBLE_UINT16:
		return 4U;
		break;
	}
}


CD3D11_Texture2D::CD3D11_Texture2D(std::string filename, UINT16 loadtype) : CD3D11_Texture2D()
{
	if (file_exists(filename))
	{
		FREE_IMAGE_FORMAT FileFormat = FreeImage_GetFileType(filename.c_str(), 0);
		if (FileFormat == FIF_UNKNOWN)
		{
			throw std::runtime_error("CDX_Texture2D error - The texture loader cant load this file format or the file not exist(directory - " + filename + ")");
			return;
		}

		FIBITMAP *image = FreeImage_Load(FileFormat, filename.c_str());
		if (image == NULL)
		{
			throw std::runtime_error("CDX_Texture2D error - The texture loader failed to load the image");
			return;
		}

		FIBITMAP* temp = image;
		image = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(temp);

		//Set texture parameters...

		tex_width	= FreeImage_GetWidth(image);
		tex_height	= FreeImage_GetHeight(image);

		UINT32 pitch = FreeImage_GetPitch(image);
		UINT8  byte_per_pixel = pitch / tex_width;

		//Copy texture data...
		BYTE *temp_bytes = FreeImage_GetBits(image);
		BYTE *texture_data = new BYTE[tex_width * tex_height * 4];

		for (uint32_t i = 0; i < tex_width * tex_height; i++)
		{
			texture_data[i * 4 + 0] = temp_bytes[i * 4 + 2];
			texture_data[i * 4 + 1] = temp_bytes[i * 4 + 1];
			texture_data[i * 4 + 2] = temp_bytes[i * 4 + 0];
			texture_data[i * 4 + 3] = temp_bytes[i * 4 + 3];
		}

		//Determine the texture's format...
		if (loadtype == CDX_LOAD_DEPTHMAP)
		{
			switch (byte_per_pixel)
			{
			case 3:
				texture_format = TF_DEPTHMAP_16F;
				break;
			case 4:
				texture_format = TF_DEPTHMAP_32F;
				break;
			default:
				throw std::runtime_error("CDX_Texture2D error - The texture format unsupported");
				break;
			}
		}
		else if (loadtype == CDX_LOAD_COLOR_TEXTURE)
		{
			switch (byte_per_pixel)
			{
			case 1:
				texture_format = TF_COLORMAP_R8;
				break;
			case 2:
				texture_format = TF_COLORMAP_R8G8;
				break;
			case 3:
				texture_format = TF_COLORMAP_R8G8B8;
				break;
			case 4:
				texture_format = TF_COLORMAP_R8G8B8A8;
				break;
			default:
				throw std::runtime_error("CDX_Texture2D error - The texture format unsupported");
				break;
			}
		}
		else
		{
			throw std::runtime_error("CDX_Texture2D error - The texture format unsupported");
		}

		if (FAILED(CreateResource((BYTE*)texture_data)))
		{
			throw std::runtime_error("CDX_Texture2D error - DirectX failed to create the texture resource");
			return;
		}
		else
		{
			load_succesed = false;
		}

		load_succesed = true;
	}
	else
	{
		throw std::runtime_error("CDX_Texture2D error - File directory : '" + filename + "' not found.");
		load_succesed = false;//File is not founded;
	}
}

CD3D11_Texture2D::CD3D11_Texture2D(ID3D11Texture2D* dx11_texture, DXGI_FORMAT srv_format)
{
	load_succesed = false;

	if (dx11_texture == nullptr)
	{
		throw std::runtime_error("CD3D11_Texture2D Constructor Error - dx11_texture is NULL");
		return;
	}

	D3D11_TEXTURE2D_DESC texture_desc;
	dx11_texture->GetDesc(&texture_desc);

	this->texture			= dx11_texture;
	this->tex_width			= texture_desc.Width;
	this->tex_height		= texture_desc.Height;

	//Shader resource view description...
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	ZeroMemory(&srv_desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srv_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip	= 0;
	srv_desc.Texture2D.MipLevels		= texture_desc.MipLevels;
	srv_desc.Format						= srv_format;

	//Create the shader resoruce view...
	if (FAILED(DX11_GLOBAL::Device->CreateShaderResourceView(dx11_texture, &srv_desc, &resource_srv)))
	{
		throw std::runtime_error("CDX_Texture2D Constructor Error - SRV failed to create");
		return;
	}

	load_succesed = true;
}


CD3D11_Texture2D::CD3D11_Texture2D(UINT16 wight, UINT16 height, CDX_TEXTURE_FORMAT format, const void *data) : CD3D11_Texture2D()
{
	tex_width		= wight;
	tex_height		= height;
	texture_format	= format;

	if (FAILED(CreateResource((BYTE*)data)))
	{
		throw std::runtime_error("CDX_Texture2D error - DirectX failed to create the texture resource");
		load_succesed = false;
		return;
	}
	else
	{
		load_succesed = true;
	}
}

HRESULT CD3D11_Texture2D::CreateResource(BYTE *data_pointer)
{
	HRESULT result = S_OK;

	//The texture description...
	D3D11_TEXTURE2D_DESC texture_desc;
	ZeroMemory(&texture_desc, sizeof(D3D11_TEXTURE2D_DESC));
	texture_desc.Width					= tex_width;
	texture_desc.Height					= tex_height;
	texture_desc.MipLevels				= 1;
	texture_desc.ArraySize				= 1;
	texture_desc.SampleDesc.Count		= 1;
	texture_desc.SampleDesc.Quality		= 0;
	texture_desc.Usage					= D3D11_USAGE_DEFAULT;
	texture_desc.CPUAccessFlags			= 0;

	D3D11_SUBRESOURCE_DATA sub_data;
	sub_data.pSysMem					= data_pointer;
	sub_data.SysMemPitch				= Get_TexturePitch();
	sub_data.SysMemSlicePitch			= Get_TexturePitch() * tex_height;

	//Shader resource view description...
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	ZeroMemory(&srv_desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srv_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip	= 0;
	srv_desc.Texture2D.MipLevels		= texture_desc.MipLevels;

	//Build descriptions from format

	if(texture_format == TF_NOTHING)
	{
		throw std::runtime_error("CDX_Texture2D::CreateResource error - texture format is not set(texture_format = TF_NOTHING)");
		return 1;
	}

	switch (texture_format)
	{
	case TF_DEPTHMAP_16F:
		texture_desc.Format			= DXGI_FORMAT_R16_TYPELESS;
		texture_desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		texture_desc.MiscFlags		= 0;
		srv_desc.Format				= DXGI_FORMAT_R16_FLOAT;
		break;
	case TF_DEPTHMAP_32F:
		texture_desc.Format			= DXGI_FORMAT_R32_TYPELESS;
		texture_desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		texture_desc.MiscFlags		= 0;
		srv_desc.Format				= DXGI_FORMAT_R32_FLOAT;
		break;

	case TF_COLORMAP_R8:
		texture_desc.Format			= DXGI_FORMAT_R8_TYPELESS;
		texture_desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texture_desc.MiscFlags		= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		srv_desc.Format				= DXGI_FORMAT_R8_UNORM;
		break;

	case TF_COLORMAP_R8G8:
		texture_desc.Format			= DXGI_FORMAT_R8G8_TYPELESS;
		texture_desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texture_desc.MiscFlags		= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		srv_desc.Format				= DXGI_FORMAT_R8G8_UNORM;
		break;

	case TF_COLORMAP_R8G8B8A8:
		texture_desc.Format			= DXGI_FORMAT_R8G8B8A8_TYPELESS;
		texture_desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texture_desc.MiscFlags		= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		srv_desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
		break;

	case TF_HALF_VECTOR:
		texture_desc.Format			= DXGI_FORMAT_R16G16B16A16_FLOAT;
		texture_desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texture_desc.MiscFlags		= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		srv_desc.Format				= DXGI_FORMAT_R16G16B16A16_FLOAT;
		break;

	case TF_DOUBLE_UINT16:
		texture_desc.Format			= DXGI_FORMAT_R32G32_UINT;
		texture_desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texture_desc.MiscFlags		= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		srv_desc.Format				= DXGI_FORMAT_R32G32_UINT;
		break;

	default:
		throw std::runtime_error("CDX_Texture2D::CreateResource error - format is unsupported");
		break;
	}


	//Create the texture object...
	if (data_pointer != nullptr)
		result = DX11_GLOBAL::Device->CreateTexture2D(&texture_desc, &sub_data, &texture);
	else
		result = DX11_GLOBAL::Device->CreateTexture2D(&texture_desc, 0, &texture);
	if (FAILED(result))
	{
		throw std::runtime_error("CDX_Texture2D::CreateResource error - texture failed to create");
		return result;
	}

	//Create the shader resoruce view...
	result = DX11_GLOBAL::Device->CreateShaderResourceView(texture, &srv_desc, &resource_srv);
	if (FAILED(result))
	{
		throw std::runtime_error("CDX_Texture2D::CreateResource error - srv failed to create");
		return result;
	}

	return result;//S_OK
}

CD3D11_Texture2D::~CD3D11_Texture2D()
{
	if (texture) texture->Release();
}

//Glbal functions...
void CDX_SaveTextureAsIcon(std::string image_dir, std::string icon_out_dir)
{
	FREE_IMAGE_FORMAT file_format;
	file_format = FreeImage_GetFileType(image_dir.c_str(), 0);

	if (file_format == FIF_UNKNOWN)
	{
		throw std::runtime_error(std::string("CDX_SaveTextureAsIcon error - The file not found or the image's format is not support(directory : ") + (char)34 + image_dir + (char)34 + std::string(")"));
		return;
	}

	FIBITMAP* image = FreeImage_Load(file_format, image_dir.c_str());

	if (image == NULL)
	{
		throw std::runtime_error("DX_SaveTextureAsIcon error - The image failed to load");
		return;
	}

	FIBITMAP* temp = image;
	image = FreeImage_ConvertTo32Bits(image);
	FreeImage_Unload(temp);

	//Set texture parameters

	UINT16 image_width = FreeImage_GetWidth(image);
	UINT16 image_height = FreeImage_GetHeight(image);
	UINT16 image_pitch = FreeImage_GetPitch(image);

	BYTE* image_bytes = FreeImage_GetBits(image);

	//Write icon file...

	std::ofstream file_out;
	file_out.open(icon_out_dir.c_str(), std::ios::binary | std::ios::out);

	file_out.write((char*)&image_width,   sizeof(UINT16));
	file_out.write((char*)&image_height,  sizeof(UINT16));

	for (uint32_t i = 0; i < image_width * image_height; i++)
	{
		const UINT16 pixel_unit = image_bytes[i * 4 + 2] + (image_bytes[i * 4 + 3] << 8);//save the red value and alpha value
		const char* pixel_bytes = (char*)&pixel_unit;

		file_out.write(pixel_bytes, 2);
	}

	file_out.close();
}
