#include "DX_Workspace.h"

HRESULT DX11_GLOBAL::InstallDevice()
{
	HRESULT result;

	UINT device_flags			= 0;
	D3D_DRIVER_TYPE driver_type = D3D_DRIVER_TYPE_NULL;

#ifdef _DEBUG
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT num_driver_types	= ARRAYSIZE(driver_types);
	UINT num_feature_levels	= ARRAYSIZE(feature_levels);

	for (UINT driver_index = 0; driver_index < num_driver_types; driver_index++)
	{
		driver_type = driver_types[driver_index];
		result = D3D11CreateDevice(	nullptr, driver_type, nullptr, device_flags, feature_levels, num_feature_levels,
									D3D11_SDK_VERSION, &DX11_GLOBAL::Device, nullptr, &DX11_GLOBAL::DeviceContext);

		if (SUCCEEDED(result))
		{
			break;
		}
		else if(result == E_INVALIDARG)
		{
			//DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			result = D3D11CreateDevice( nullptr, driver_type, nullptr, device_flags, &feature_levels[1], num_feature_levels - 1,
										D3D11_SDK_VERSION, &DX11_GLOBAL::Device, nullptr, &DX11_GLOBAL::DeviceContext);
		}
	}

	if (FAILED(result))
		return result;
	
	DX11_GLOBAL::Factory = nullptr;
	IDXGIDevice* dxgi_device = nullptr;
	result = DX11_GLOBAL::Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgi_device));
	if (SUCCEEDED(result))
	{
		IDXGIAdapter* adapter = nullptr;

		result = dxgi_device->GetAdapter(&adapter);
		if (SUCCEEDED(result))
		{
			result = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&DX11_GLOBAL::Factory));
			adapter->Release();
		}

		dxgi_device->Release();
	}

	result = DX11_GLOBAL::Device->QueryInterface(IID_PPV_ARGS(&Device1));

	return result;
}