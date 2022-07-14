#include "SimpleComplie.h"

std::wstring GetDirectoryFolder(std::wstring directory_)
{
	Uint i;
	for (i = directory_.size() - 1; i >= 0; i--)
	{
		if (directory_.at(i) == '/' || directory_.at(i) == 47)
			break;
	}

	if (i < 0)
		return directory_;

	if (directory_.at(i) == '/' || directory_.at(i) == 47)//If directory use '//' or '\\'
		i--;

	if (i < 0)
		return directory_;

	else return std::wstring(directory_.begin(), directory_.begin() + i + 1);
}

std::wstring GetDirectoryNonFormatedFile(std::wstring directory_)
{
	Uint i;
	for (i = directory_.size() - 1; i >= 0; i--)
	{
		if (directory_.at(i) == '.')
			break;
	}

	if (i < 0)
		return directory_;

	else return std::wstring(directory_.begin(), directory_.begin() + i);
}

Void Engine::CompileGLSLShaderToByteCode(std::wstring directory_)
{
	//Compile the shader...

	std::ofstream bat_file(L"compile.bat");
	if (!bat_file.good())
	{
		throw std::runtime_error("Shader File Not found at directory" + std::string(directory_.begin(), directory_.end()) + '\n');
		return;
	}

	std::wstring commad_line = L"C:/VulkanSDK/1.2.198.1/Bin/glslc.exe " + directory_ + L" -o " + GetDirectoryNonFormatedFile(directory_) + L".spv";
	bat_file << std::string(commad_line.begin(), commad_line.end());
	bat_file.close();

	std::wstring path = std::filesystem::current_path().c_str() + std::wstring(L"\\bat_file.bat");
	int result = std::system("C:/projects/VulkanTest/VulkanTest/compile.bat");
	if (result != 0)
	{
		throw std::runtime_error("Compile Error! There is some Compilation isue with the shader at directory " + std::string(directory_.begin(), directory_.end()) + '\n');
		return;
	}
}

Void Engine::CompileHLSLShaderToByteCode(std::wstring directory_)
{
	//Compile the shader...

	std::ofstream bat_file(L"compile.bat");
	if (!bat_file.good())
	{
		throw std::runtime_error("Shader File Not found at directory" + std::string(directory_.begin(), directory_.end()) + '\n');
		return;
	}

	std::wstring commad_line = L"C:/VulkanSDK/1.2.198.0/Bin/dxc.exe " + directory_ + L" -o " + GetDirectoryNonFormatedFile(directory_) + L".spv";
	bat_file << std::string(commad_line.begin(), commad_line.end());
	bat_file.close();

	std::wstring path = std::filesystem::current_path().c_str() + std::wstring(L"\\bat_file.bat");
	int result = std::system("C:/projects/VulkanTest/VulkanTest/compile.bat");
	if (result != 0)
	{
		throw std::runtime_error("Compile Error! There is some Compilation isue with the shader at directory " + std::string(directory_.begin(), directory_.end()) + '\n');
		return;
	}
}


std::vector<Char> Engine::LoadLShaderBytecode(std::wstring directory_)
{
	//Now import the SPIR byte code...

	std::ifstream file(directory_, std::ios::ate | std::ios::binary);
	if (!file.good())
	{
		throw std::runtime_error("Something get worng the bit-code not found!");
		return std::vector<Char>();
	}

	Uint buffer_size = (Uint)file.tellg();
	std::vector<Char> buffer(buffer_size);

	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), buffer_size);

	file.close();

	return buffer;
}


VkPipelineShaderStageCreateInfo Engine::CompileGLSLShader(VkDevice device_, std::wstring directory_, VkShaderStageFlagBits stage_)
{
#ifndef SKIP_SHADER_BYTECODE_COMPILING
	CompileGLSLShaderToByteCode(directory_);
#endif

	std::vector<Char> bytecode_buffer = LoadLShaderBytecode(GetDirectoryNonFormatedFile(directory_) + L".spv");

	VkShaderModule shader_module;
	VkShaderModuleCreateInfo module_desc{};
	module_desc.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	module_desc.codeSize	= bytecode_buffer.size();
	module_desc.pCode		= reinterpret_cast<const Uint*>(bytecode_buffer.data());
	
	if (vkCreateShaderModule(device_, &module_desc, nullptr, &shader_module) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create shader module!");
		return VkPipelineShaderStageCreateInfo();
	}

	VkPipelineShaderStageCreateInfo shader_stage_desc{};
	shader_stage_desc.sType					= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_desc.module				= shader_module;
	shader_stage_desc.stage					= stage_;
	shader_stage_desc.pName					= "main";

	return shader_stage_desc;
}

VkPipelineShaderStageCreateInfo Engine::CompileHLSLShader(VkDevice device_, std::wstring directory_, VkShaderStageFlagBits stage_)
{
	#ifndef SKIP_SHADER_BYTECODE_COMPILING
	CompileHLSLShaderToByteCode(directory_);
#endif

	std::vector<Char> bytecode_buffer = LoadLShaderBytecode(GetDirectoryNonFormatedFile(directory_) + L".spv");

	VkShaderModule shader_module;
	VkShaderModuleCreateInfo module_desc{};
	module_desc.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	module_desc.codeSize	= bytecode_buffer.size();
	module_desc.pCode		= reinterpret_cast<const Uint*>(bytecode_buffer.data());
	
	if (vkCreateShaderModule(device_, &module_desc, nullptr, &shader_module) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create shader module!");
		return VkPipelineShaderStageCreateInfo();
	}

	VkPipelineShaderStageCreateInfo shader_stage_desc{};
	shader_stage_desc.sType					= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_desc.module				= shader_module;
	shader_stage_desc.stage					= stage_;
	shader_stage_desc.pName					= "main";

	return shader_stage_desc;
}