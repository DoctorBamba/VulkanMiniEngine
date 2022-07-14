#pragma once
#include "Workspace.h"

extern __declspec(selectany) PFN_vkCmdBindVertexBuffers2EXT vkCmdSetPrimitiveTopologyEXT__;
extern __declspec(selectany) PFN_vkCmdBindVertexBuffers2EXT vkCmdBindVertexBuffers2EXT__;

static void LoadExtentionsFuncPointers(VkDevice device)
{
    vkCmdSetPrimitiveTopologyEXT__  = reinterpret_cast<PFN_vkCmdBindVertexBuffers2EXT>(vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveTopologyEXT"));
    vkCmdBindVertexBuffers2EXT__    = reinterpret_cast<PFN_vkCmdBindVertexBuffers2EXT>(vkGetDeviceProcAddr(device, "vkCmdBindVertexBuffers2EXT"));
}
