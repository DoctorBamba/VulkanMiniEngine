#pragma once
#include "../Workspace.h"
#include <fstream>
#include <filesystem>

namespace Engine
{
	Void CompileGLSLShaderToByteCode(std::wstring directory_);
	Void CompileHLSLShaderToByteCode(std::wstring directory_);
	std::vector<Char> LoadLShaderBytecode(std::wstring directory_);
	VkPipelineShaderStageCreateInfo CompileGLSLShader(VkDevice device_, std::wstring directory_, VkShaderStageFlagBits stage_);
	VkPipelineShaderStageCreateInfo CompileHLSLShader(VkDevice device_, std::wstring directory_, VkShaderStageFlagBits stage_);
}