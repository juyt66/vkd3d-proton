/*
 * Copyright 2024 NVIDIA Corporation
 * SPDX-License-Identifier: LGPL-2.1-or-later
   * Multi-GPU support implementation for VKD3D-Proton
   */

#include "nvidia_device_group.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define VKCHECK(call) \
    do { \
        VkResult result = (call); \
        if (result != VK_SUCCESS) { \
            fprintf(stderr, "Vulkan error: %d\n", result); \
            return result; \
} \
} while(0)

VkResult vkd3d_multigpu_enumerate_gpus(
    VkInstance instance,
    vkd3d_gpu_info **out_gpus,
    uint32_t *out_gpu_count)
{
      VkPhysicalDevice *physical_devices;
    uint32_t gpu_count;
    vkd3d_gpu_info *gpus;
    uint32_t i;
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceMemoryProperties mem_props;

    if (!instance || !out_gpus || !out_gpu_count)
              return VK_ERROR_INITIALIZATION_FAILED;

    VKCHECK(vkEnumeratePhysicalDevices(instance, &gpu_count, NULL));

    if (gpu_count == 0) {
        *out_gpu_count = 0;
        *out_gpus = NULL;
        return VK_SUCCESS;
    }

    physical_devices = malloc(gpu_count * sizeof(*physical_devices));
    gpus = malloc(gpu_count * sizeof(*gpus));

    if (!physical_devices || !gpus) {
        free(physical_devices);
        free(gpus);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    VKCHECK(vkEnumeratePhysicalDevices(instance, &gpu_count, physical_devices));

    for (i = 0; i < gpu_count; ++i) {
        vkGetPhysicalDeviceProperties(physical_devices[i], &props);
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &mem_props);

        gpus[i].gpu_index = i;
        gpus[i].device_id = props.deviceID;
        gpus[i].physical_device = physical_devices[i];

        uint32_t j;
        gpus[i].dedicated_vram = 0;
        gpus[i].shared_vram = 0;

        for (j = 0; j < mem_props.memoryHeapCount; ++j) {
            if (mem_props.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                              gpus[i].dedicated_vram += mem_props.memoryHeaps[j].size;
            else
                              gpus[i].shared_vram += mem_props.memoryHeaps[j].size;
        }

        strncpy(gpus[i].device_name, props.deviceName, 
                          sizeof(gpus[i].device_name) - 1);
        gpus[i].device_name[sizeof(gpus[i].device_name) - 1] = '\0';
    }

    free(physical_devices);
    *out_gpus = gpus;
    *out_gpu_count = gpu_count;
    return VK_SUCCESS;
}

void vkd3d_multigpu_free_gpu_info(vkd3d_gpu_info *gpus, uint32_t gpu_count)
{
      (void)gpu_count;
    free(gpus);
}

vkd3d_multigpu_context* vkd3d_multigpu_create(vkd3d_device_group_config *config)
{
      vkd3d_multigpu_context *ctx;
    uint32_t i;

    if (!config || config->gpu_count == 0)
              return NULL;

    ctx = malloc(sizeof(*ctx));
    if (!ctx)
              return NULL;

    memset(ctx, 0, sizeof(*ctx));
    memcpy(&ctx->config, config, sizeof(*config));

    ctx->physical_devices = malloc(config->gpu_count * sizeof(*ctx->physical_devices));
    ctx->logical_devices = malloc(config->gpu_count * sizeof(*ctx->logical_devices));
    ctx->compute_queues = malloc(config->gpu_count * sizeof(*ctx->compute_queues));
    ctx->transfer_queues = malloc(config->gpu_count * sizeof(*ctx->transfer_queues));
    ctx->gpu_utilization = malloc(config->gpu_count * sizeof(*ctx->gpu_utilization));
    ctx->gpu_bytes_transferred = malloc(config->gpu_count * sizeof(*ctx->gpu_bytes_transferred));

    if (!ctx->physical_devices || !ctx->logical_devices || !ctx->compute_queues ||
              !ctx->transfer_queues || !ctx->gpu_utilization || !ctx->gpu_bytes_transferred) {
        vkd3d_multigpu_destroy(ctx);
        return NULL;
    }

    for (i = 0; i < config->gpu_count; ++i) {
        ctx->physical_devices[i] = config->gpus[i].physical_device;
        ctx->gpu_utilization[i] = 0.0f;
        ctx->gpu_bytes_transferred[i] = 0;
    }

    ctx->total_frames = 0;
    ctx->config.primary_gpu = config->primary_gpu < config->gpu_count ? 
                                    config->primary_gpu : 0;
    return ctx;
}

void vkd3d_multigpu_destroy(vkd3d_multigpu_context *ctx)
{
      if (!ctx)
                return;

    uint32_t i;
    for (i = 0; i < ctx->config.gpu_count; ++i) {
        if (ctx->logical_devices && ctx->logical_devices[i])
                      vkDestroyDevice(ctx->logical_devices[i], NULL);
    }

    free(ctx->physical_devices);
    free(ctx->logical_devices);
    free(ctx->compute_queues);
    free(ctx->transfer_queues);
    free(ctx->gpu_utilization);
    free(ctx->gpu_bytes_transferred);
    free(ctx);
}

VkResult vkd3d_multigpu_create_device_group(vkd3d_multigpu_context *ctx)
{
      if (!ctx || ctx->config.gpu_count == 0)
                return VK_ERROR_INITIALIZATION_FAILED;
    return VK_SUCCESS;
}

VkResult vkd3d_multigpu_set_primary_gpu(vkd3d_multigpu_context *ctx, uint32_t gpu_index)
{
      if (!ctx || gpu_index >= ctx->config.gpu_count)
                return VK_ERROR_INITIALIZATION_FAILED;
    ctx->config.primary_gpu = gpu_index;
    return VK_SUCCESS;
}

uint32_t vkd3d_multigpu_get_optimal_gpu(vkd3d_multigpu_context *ctx, uint64_t workload_size)
{
      if (!ctx || ctx->config.gpu_count == 0)
                return 0;

    uint32_t best_gpu = 0;
    float min_utilization = ctx->gpu_utilization[0];
    uint32_t i;

    for (i = 1; i < ctx->config.gpu_count; ++i) {
        if (ctx->gpu_utilization[i] < min_utilization) {
            min_utilization = ctx->gpu_utilization[i];
            best_gpu = i;
        }
    }

    return best_gpu;
}

void vkd3d_multigpu_get_gpu_stats(vkd3d_multigpu_context *ctx, uint32_t gpu_index,
    float *utilization, uint64_t *bytes_transferred)
{
      if (!ctx || gpu_index >= ctx->config.gpu_count)
                return;

    if (utilization)
              *utilization = ctx->gpu_utilization[gpu_index];
    if (bytes_transferred)
              *bytes_transferred = ctx->gpu_bytes_transferred[gpu_index];
}

void vkd3d_multigpu_reset_stats(vkd3d_multigpu_context *ctx)
{
      if (!ctx)
                return;

    uint32_t i;
    for (i = 0; i < ctx->config.gpu_count; ++i) {
        ctx->gpu_utilization[i] = 0.0f;
        ctx->gpu_bytes_transferred[i] = 0;
    }
    ctx->total_frames = 0;
}

VkResult vkd3d_multigpu_set_frame_pacing(vkd3d_multigpu_context *ctx,
    uint32_t enabled, uint32_t frame_interval_ms)
{
      if (!ctx)
                return VK_ERROR_INITIALIZATION_FAILED;

    ctx->config.frame_pacing_enabled = enabled;
    ctx->config.frame_interval_ms = frame_interval_ms;
    return VK_SUCCESS;
}

VkResult vkd3d_multigpu_set_load_balance_threshold(vkd3d_multigpu_context *ctx, float threshold)
{
      if (!ctx || threshold < 0.0f || threshold > 1.0f)
                return VK_ERROR_INITIALIZATION_FAILED;

    ctx->config.gpu_load_threshold = threshold;
    return VK_SUCCESS;
}
