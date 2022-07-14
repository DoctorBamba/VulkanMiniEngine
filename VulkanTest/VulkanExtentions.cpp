#include "VulkanExtentions.h"

VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers2EXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets,
    const VkDeviceSize*                         pSizes,
    const VkDeviceSize*                         pStrides)
{
    if (vkCmdBindVertexBuffers2EXT__ != nullptr)
        return vkCmdBindVertexBuffers2EXT__(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
    else
        throw std::runtime_error("The Vulkan Instance not support vkCmdBindVertexBuffers2EXT call!!!");
}