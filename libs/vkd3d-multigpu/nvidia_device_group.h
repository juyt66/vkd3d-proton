/*
 * Copyright 2024 NVIDIA Corporation
 * SPDX-License-Identifier: LGPL-2.1-or-later
   *
   * Multi-GPU support extension for VKD3D-Proton
   * Direct3D 12 Multi-GPU Device Group Management
 */

#ifndef __VKD3D_NVIDIA_DEVICE_GROUP_H
#define __VKD3D_NVIDIA_DEVICE_GROUP_H

#include <vulkan/vulkan.h>
#include "vkd3d.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct vkd3d_device_group vkd3d_device_group;
typedef struct vkd3d_gpu_affinity vkd3d_gpu_affinity;

/* Multi-GPU adapter information */
typedef struct
{
    uint32_t gpu_index;
    uint32_t device_id;
    uint64_t dedicated_vram;
    uint64_t shared_vram;
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    char device_name[256];
} vkd3d_gpu_info;

/* Device group configuration */
typedef struct
{
    uint32_t gpu_count;
    vkd3d_gpu_info *gpus;

    /* Affinity settings */
    uint32_t primary_gpu;
    uint8_t *gpu_affinity_mask;

    /* Load balancing */
    uint32_t load_balance_enabled;
    float gpu_load_threshold;

    /* Frame pacing */
    uint32_t frame_pacing_enabled;
    uint32_t frame_interval_ms;
} vkd3d_device_group_config;

/* Multi-GPU context */
typedef struct
{
    vkd3d_device_group_config config;
    VkInstance instance;
    VkPhysicalDevice *physical_devices;
    VkDevice *logical_devices;
    VkQueue *compute_queues;
    VkQueue *transfer_queues;

    /* Synchronization */
    VkFence *frame_fences;
    VkEvent *gpu_events;
    VkSemaphore *frame_semaphores;

    /* Memory management */
    VkDeviceMemory *device_memories;
    uint64_t *allocated_sizes;

    /* Statistics */
    uint64_t total_frames;
    float *gpu_utilization;
    uint64_t *gpu_bytes_transferred;
} vkd3d_multigpu_context;

/* Initialization and cleanup */
vkd3d_multigpu_context* vkd3d_multigpu_create(
    vkd3d_device_group_config *config);

void vkd3d_multigpu_destroy(
    vkd3d_multigpu_context *ctx);

/* GPU enumeration */
VkResult vkd3d_multigpu_enumerate_gpus(
    VkInstance instance,
    vkd3d_gpu_info **gpus,
    uint32_t *gpu_count);

void vkd3d_multigpu_free_gpu_info(
    vkd3d_gpu_info *gpus,
    uint32_t gpu_count);

/* Device management */
VkResult vkd3d_multigpu_create_device_group(
    vkd3d_multigpu_context *ctx);

VkResult vkd3d_multigpu_set_primary_gpu(
    vkd3d_multigpu_context *ctx,
    uint32_t gpu_index);

/* Memory affinity */
VkResult vkd3d_multigpu_allocate_memory(
    vkd3d_multigpu_context *ctx,
    VkMemoryAllocateInfo *alloc_info,
    uint32_t target_gpu,
    VkDeviceMemory *memory);

VkResult vkd3d_multigpu_bind_to_gpu(
    vkd3d_multigpu_context *ctx,
    VkDeviceMemory memory,
    uint8_t gpu_mask);

/* Work distribution */
VkResult vkd3d_multigpu_submit_work(
    vkd3d_multigpu_context *ctx,
    uint32_t target_gpu,
    VkSubmitInfo *submit_info,
    VkFence fence);

VkResult vkd3d_multigpu_wait_for_completion(
    vkd3d_multigpu_context *ctx,
    uint32_t gpu_mask,
    uint64_t timeout);

/* Load balancing */
VkResult vkd3d_multigpu_balance_load(
    vkd3d_multigpu_context *ctx,
    float *gpu_weights);

uint32_t vkd3d_multigpu_get_optimal_gpu(
    vkd3d_multigpu_context *ctx,
    uint64_t workload_size);

/* Peer access management */
VkResult vkd3d_multigpu_enable_peer_access(
    vkd3d_multigpu_context *ctx,
    uint32_t source_gpu,
    uint32_t dest_gpu);

VkResult vkd3d_multigpu_disable_peer_access(
    vkd3d_multigpu_context *ctx,
    uint32_t source_gpu,
    uint32_t dest_gpu);

/* Synchronization primitives */
VkResult vkd3d_multigpu_create_shared_fence(
    vkd3d_multigpu_context *ctx,
    VkFence *fence);

VkResult vkd3d_multigpu_create_shared_semaphore(
    vkd3d_multigpu_context *ctx,
    VkSemaphore *semaphore);

/* Statistics and profiling */
void vkd3d_multigpu_get_gpu_stats(
    vkd3d_multigpu_context *ctx,
    uint32_t gpu_index,
    float *utilization,
    uint64_t *bytes_transferred);

void vkd3d_multigpu_reset_stats(
    vkd3d_multigpu_context *ctx);

/* Configuration */
VkResult vkd3d_multigpu_set_frame_pacing(
    vkd3d_multigpu_context *ctx,
    uint32_t enabled,
    uint32_t frame_interval_ms);

VkResult vkd3d_multigpu_set_load_balance_threshold(
    vkd3d_multigpu_context *ctx,
    float threshold);

#ifdef __cplusplus
}
#endif

#endif /* __VKD3D_NVIDIA_DEVICE_GROUP_H */
