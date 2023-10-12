#pragma once

#include "GPU_Objects/CDX_Vector.h"//Object for shaders
#include "GPU_Objects/CDX_Buffer.h"//Object for shaders

#include <iostream>
#include <string>

#define DX_GPROGRAM_MAX_SRV			   8
#define DX_GPROGRAM_MAX_BUFFERS_ACCESS 8

//The next class is a standartic directx-graphics-program class...
class CDX_GProgram
{
	private:

		bool enable_vertex_shader;
		bool enable_pixel_shader;
		bool enable_geometry_shader;

		ID3D11VertexShader			*vertex_shader;
		ID3D11PixelShader			*pixel_shader;
		ID3D11GeometryShader		*geometry_shader;

		ID3D11InputLayout			*input_layout;

		//Binds...

		ID3D11ShaderResourceView	*binds_srv[DX_GPROGRAM_MAX_SRV];
		ID3D11UnorderedAccessView   *binds_buffers_access[DX_GPROGRAM_MAX_BUFFERS_ACCESS];

	public:

		bool file_exists(const std::wstring& name)
		{
			std::ifstream f(name.c_str());

			return f.good();
		}

		HRESULT compile_shader_from_file(std::wstring target_directory, D3D_SHADER_MACRO *defines, LPCSTR entry_point_keyword, LPCSTR shader_model, ID3DBlob **binary, ID3DBlob **compile_error_out)
		{
			//Chack if the file is exsist...

			if (!file_exists(target_directory))
				throw std::runtime_error("Shader file not found : " + std::string(target_directory.begin(), target_directory.end()));

			HRESULT result = S_OK;

			DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
			#ifdef _DEBUG
					shader_flags |= D3DCOMPILE_DEBUG;
			#endif

			result = D3DCompileFromFile(target_directory.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point_keyword, shader_model, shader_flags, 0, binary, compile_error_out);

			return result;
		}

		HRESULT compile_shader_from_memory(std::string text, std::string source_name, std::string entry_point, std::string shader_model, ID3DBlob **binary, ID3DBlob **compile_error_out)
		{
			//Chack if the file is exsist...

			if (!text.length())
				throw std::runtime_error("Shader text is empty");

			HRESULT result = S_OK;

			DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
			#ifdef _DEBUG
					shader_flags |= D3DCOMPILE_DEBUG;
			#endif

					result = D3DCompile(text.data(), text.length(), source_name.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point.data(), shader_model.data(), shader_flags, NULL, binary, compile_error_out);

			return result;
		}

		void write_complie_errors(std::wstring directory_out, ID3DBlob *error_message, std::string title)
		{
			if (error_message)
			{
				// If the shader failed to compile it should have writen something to the error message.
				char* compile_errors;
				unsigned long long bufferSize, i;
				std::ofstream fout;

				// Open a file to write the error message to.
				fout.open(directory_out);

				compile_errors = (char*)(error_message->GetBufferPointer());
				bufferSize = error_message->GetBufferSize();

				// Write out the error message.
				fout << title << directory_out.c_str() << "' :\n";

				for (i = 0; i < bufferSize; i++)
					fout << compile_errors[i];

				// Release the error message.
				error_message->Release();
				error_message = nullptr;

				// Close the file.
				fout.close();

				//open the file
				system(std::string(directory_out.begin(), directory_out.end()).c_str());

				throw std::runtime_error("OP_Program compile has error please check in the shader-error file");
			}
		}

		CDX_GProgram()
		{
			enable_vertex_shader	= false;
			enable_pixel_shader		= false;
			enable_geometry_shader	= false;
			vertex_shader			= nullptr;
			pixel_shader			= nullptr;
			geometry_shader			= nullptr;
			input_layout			= nullptr;
		}

		CDX_GProgram(std::wstring vs_directory, std::wstring ps_directory, std::wstring gs_directory, D3D_SHADER_MACRO *vs_defines, D3D_SHADER_MACRO* ps_defines, D3D_SHADER_MACRO* gs_defines, const std::vector<D3D11_INPUT_ELEMENT_DESC> *elements) : CDX_GProgram()
		{
			HRESULT result = S_OK;

			enable_vertex_shader		= (bool)(vs_directory.length());
			enable_pixel_shader			= (bool)(ps_directory.length());
			enable_geometry_shader		= (bool)(gs_directory.length());

			ID3DBlob *vs_error_message	= nullptr;
			ID3DBlob *ps_error_message	= nullptr;
			ID3DBlob *gs_error_message	= nullptr;

			ID3DBlob *vertex_shader_binary		= nullptr;
			ID3DBlob *pixel_shader_binary		= nullptr;
			ID3DBlob *geometry_shader_binary	= nullptr;

			//Compile shaders...

			if (enable_vertex_shader)
			{
				result = compile_shader_from_file(vs_directory, vs_defines, "main", "vs_5_0", &vertex_shader_binary, &vs_error_message);

				if (FAILED(result))
					write_complie_errors(L"vertex_error.txt", vs_error_message, "Vertex-shader : ");

				DX11_GLOBAL::Device->CreateVertexShader(vertex_shader_binary->GetBufferPointer(), vertex_shader_binary->GetBufferSize(), NULL, &vertex_shader);
			}

			if (enable_pixel_shader)
			{
				result = compile_shader_from_file(ps_directory, ps_defines, "main", "ps_5_0", &pixel_shader_binary, &ps_error_message);

				if (FAILED(result))
					write_complie_errors(L"pixel_error.txt", ps_error_message, "Pixel-shader : ");

				DX11_GLOBAL::Device->CreatePixelShader(pixel_shader_binary->GetBufferPointer(), pixel_shader_binary->GetBufferSize(), NULL, &pixel_shader);
			}

			if (enable_geometry_shader)
			{
				result = compile_shader_from_file(gs_directory, gs_defines, "main", "gs_5_0", &geometry_shader_binary, &gs_error_message);

				if (FAILED(result))
					write_complie_errors(L"geometry_error.txt", gs_error_message, "Geometric-shader : ");

				DX11_GLOBAL::Device->CreateGeometryShader(geometry_shader_binary->GetBufferPointer(), geometry_shader_binary->GetBufferSize(), NULL, &geometry_shader);
			}

			//Create input layout...

			if (enable_vertex_shader && elements != nullptr)
				DX11_GLOBAL::Device->CreateInputLayout(elements->data(), elements->size(), vertex_shader_binary->GetBufferPointer(), vertex_shader_binary->GetBufferSize(), &input_layout);
		}


		void use()
		{
			if (enable_vertex_shader)
				DX11_GLOBAL::DeviceContext->VSSetShader(vertex_shader, 0, 0);
			if (enable_pixel_shader)
				DX11_GLOBAL::DeviceContext->PSSetShader(pixel_shader, 0, 0);
			if (enable_geometry_shader)
				DX11_GLOBAL::DeviceContext->GSSetShader(geometry_shader, 0, 0);

			DX11_GLOBAL::DeviceContext->IASetInputLayout(input_layout);
		}

		//Set

		void set_vertex_shader(ID3D11VertexShader *vs) { vertex_shader = vs; if(vs) enable_vertex_shader = true; }
		void set_pixel_shader(ID3D11PixelShader *ps) { pixel_shader = ps; }
		void set_geometry_shader(ID3D11GeometryShader *gs) { geometry_shader = gs; }
		void set_input_layout(ID3D11InputLayout *input_layout) { this->input_layout = input_layout; }

		//Get

		ID3D11VertexShader		*get_vertex_shader() { return vertex_shader; }
		ID3D11PixelShader		*get_pixel_shader() { return pixel_shader; }
		ID3D11GeometryShader	*get_geometry_shader() { return geometry_shader; }
		ID3D11InputLayout		*get_input_layput() { return input_layout; }
};