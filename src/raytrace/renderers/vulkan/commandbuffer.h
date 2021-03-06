/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <renderers/vulkan/vkcommon.h>
#include <renderers/vulkan/initializers.h>
#include <renderers/vulkan/pipeline/pipeline.h>
#include <renderers/vulkan/pipeline/raytracingpipeline.h>
#include <renderers/vulkan/resourcebarrier.h>

#include <QVector>

namespace Qt3DRaytrace {
namespace Vulkan {

class CommandBuffer : public Resource<VkCommandBuffer>
{
public:
    CommandBuffer(VkCommandBuffer commandBuffer=VK_NULL_HANDLE);

    Result begin(VkCommandBufferUsageFlags flags=0, const VkCommandBufferInheritanceInfo *inheritanceInfo=nullptr) const;
    Result end() const;
    Result reset(VkCommandBufferResetFlags flags=0) const;

    void resourceBarrier(int numBufferTransitions, const BufferTransition *bufferTransitions, int numImageTransitions, const ImageTransition *imageTransitions) const;
    void resourceBarrier(const QVector<BufferTransition> &bufferTransitions, const QVector<ImageTransition> &imageTransitions) const;
    void resourceBarrier(const QVector<BufferTransition> &bufferTransitions) const;
    void resourceBarrier(const QVector<ImageTransition> &imageTransitions) const;

    void resourceBarrier(const BufferTransition &bufferTransition) const
    {
        resourceBarrier(1, &bufferTransition, 0, nullptr);
    }
    void resourceBarrier(const ImageTransition &imageTransition) const
    {
        resourceBarrier(0, nullptr, 1, &imageTransition);
    }

    void pipelineBarrier(VkPipelineStageFlags srcStageMask, VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask) const
    {
        VkMemoryBarrier memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER };
        memoryBarrier.srcAccessMask = srcAccessMask;
        memoryBarrier.dstAccessMask = dstAccessMask;
        vkCmdPipelineBarrier(handle, srcStageMask, dstStageMask, 0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
    }

    void beginRenderPass(const RenderPassBeginInfo &beginInfo, VkSubpassContents contents) const
    {
        vkCmdBeginRenderPass(handle, beginInfo, contents);
    }
    void endRenderPass() const
    {
        vkCmdEndRenderPass(handle);
    }

    void clearColorImage(VkImage image, ImageState imageState, const ImageSubresourceRange &range={}) const
    {
        // We only support clearing to black.
        const VkClearColorValue clearValue = {};
        vkCmdClearColorImage(handle, image, ResourceBarrier::getImageLayoutFromState(imageState), &clearValue, 1, &range);
    }
    void bindPipeline(const Pipeline &pipeline) const
    {
        vkCmdBindPipeline(handle, pipeline.bindPoint, pipeline);
    }
    void bindDescriptorSets(const Pipeline &pipeline, uint32_t firstSet, const QVector<VkDescriptorSet> &descriptorSets) const
    {
        vkCmdBindDescriptorSets(handle, pipeline.bindPoint, pipeline.pipelineLayout, firstSet, uint32_t(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
    }
    void buildTopLevelAccelerationStructure(const VkAccelerationStructureInfoNV &info, VkAccelerationStructureNV dst, VkAccelerationStructureNV src, VkBuffer instanceData, VkBuffer scratch) const
    {
        const VkBool32 update = (src != VK_NULL_HANDLE) ? VK_TRUE : VK_FALSE;
        vkCmdBuildAccelerationStructureNV(handle, &info, instanceData, 0, update, dst, src, scratch, 0);
    }
    void buildBottomLevelAccelerationStructure(const VkAccelerationStructureInfoNV &info, VkAccelerationStructureNV dst, VkAccelerationStructureNV src, VkBuffer scratch) const
    {
        const VkBool32 update = (src != VK_NULL_HANDLE) ? VK_TRUE : VK_FALSE;
        vkCmdBuildAccelerationStructureNV(handle, &info, VK_NULL_HANDLE, 0, update, dst, src, scratch, 0);
    }
    void blitImage(VkImage srcImage, ImageState srcState, VkImage dstImage, ImageState dstState, const VkImageBlit &region, VkFilter filter) const
    {
        vkCmdBlitImage(handle, srcImage, ResourceBarrier::getImageLayoutFromState(srcState), dstImage, ResourceBarrier::getImageLayoutFromState(dstState), 1, &region, filter);
    }
    void copyBuffer(VkBuffer src, VkDeviceSize srcOffset, VkBuffer dest, VkDeviceSize dstOffset, VkDeviceSize size) const
    {
        VkBufferCopy bufferCopy;
        bufferCopy.srcOffset = srcOffset;
        bufferCopy.dstOffset = dstOffset;
        bufferCopy.size = size;
        vkCmdCopyBuffer(handle, src, dest, 1, &bufferCopy);
    }
    void copyImageToBuffer(VkImage srcImage, ImageState srcState, VkBuffer dstBuffer, const VkBufferImageCopy &region) const
    {
        vkCmdCopyImageToBuffer(handle, srcImage, ResourceBarrier::getImageLayoutFromState(srcState), dstBuffer, 1, &region);
    }
    void dispatch(uint32_t groupCountX, uint32_t groupCountY=1, uint32_t groupCountZ=1) const
    {
        vkCmdDispatch(handle, groupCountX, groupCountY, groupCountZ);
    }
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex=0, uint32_t firstInstance=0) const
    {
        vkCmdDraw(handle, vertexCount, instanceCount, firstVertex, firstInstance);
    }
    void executeCommands(const QVector<VkCommandBuffer> &commandBuffers) const
    {
        vkCmdExecuteCommands(handle, uint32_t(commandBuffers.size()), commandBuffers.data());
    }
    void pushConstants(const Pipeline &pipeline, int pushConstantRangeIndex, const void *values) const
    {
        Q_ASSERT(pushConstantRangeIndex >= 0 && pushConstantRangeIndex < pipeline.pushConstantRanges.size());
        const VkPushConstantRange &range = pipeline.pushConstantRanges[pushConstantRangeIndex];
        vkCmdPushConstants(handle, pipeline.pipelineLayout, range.stageFlags, range.offset, range.size, values);
    }
    void resetQueryPool(QueryPool queryPool, uint32_t firstQuery, uint32_t queryCount) const
    {
        vkCmdResetQueryPool(handle, queryPool, firstQuery, queryCount);
    }
    void traceRays(const RayTracingPipeline &rayTracingPipeline, uint32_t width, uint32_t height) const
    {
        traceRays(rayTracingPipeline, width, height, 0, 0);
    }
    void traceRays(const RayTracingPipeline &rayTracingPipeline, uint32_t width, uint32_t height, uint32_t missShaderBindingIndex, uint32_t hitShaderBindingIndex) const
    {
        Q_ASSERT(rayTracingPipeline.bindPoint == VK_PIPELINE_BIND_POINT_RAY_TRACING_NV);
        VkBuffer shaderBindingTableBuffer = rayTracingPipeline.shaderBindingTable;
        VkDeviceSize shaderBindingTableStride = rayTracingPipeline.shaderBindingTableStride;
        VkDeviceSize missShaderBindingOffset = (1 + missShaderBindingIndex) * shaderBindingTableStride;
        VkDeviceSize hitShaderBindingOffset = rayTracingPipeline.shaderBindingTableHitGroupOffset + hitShaderBindingIndex * shaderBindingTableStride;
        vkCmdTraceRaysNV(handle,
                         shaderBindingTableBuffer, 0,
                         shaderBindingTableBuffer, missShaderBindingOffset, shaderBindingTableStride,
                         shaderBindingTableBuffer, hitShaderBindingOffset, shaderBindingTableStride,
                         VK_NULL_HANDLE, 0, 0,
                         width, height, 1);
    }
    void setViewport(const QRect &rect, float minDepth=0.0f, float maxDepth=1.0f) const
    {
        VkViewport vkViewport;
        vkViewport.x = int32_t(rect.x());
        vkViewport.y = int32_t(rect.y());
        vkViewport.width = uint32_t(rect.width());
        vkViewport.height = uint32_t(rect.height());
        vkViewport.minDepth = minDepth;
        vkViewport.maxDepth = maxDepth;
        vkCmdSetViewport(handle, 0, 1, &vkViewport);
    }
    void setScissor(const QRect &rect) const
    {
        VkRect2D vkRect;
        vkRect.offset.x = int32_t(rect.x());
        vkRect.offset.y = int32_t(rect.y());
        vkRect.extent.width = uint32_t(rect.width());
        vkRect.extent.height = uint32_t(rect.height());
        vkCmdSetScissor(handle, 0, 1, &vkRect);
    }
    void setEvent(VkEvent event, VkPipelineStageFlags stageMask) const
    {
        vkCmdSetEvent(handle, event, stageMask);
    }
    void writeTimestamp(VkPipelineStageFlagBits pipelineStage, QueryPool queryPool, uint32_t query) const
    {
        vkCmdWriteTimestamp(handle, pipelineStage, queryPool, query);
    }
};

} // Vulkan
} // Qt3DRaytrace
