/*
 * VKD3D-Proton Multi-GPU Extension Header
 * Public API for D3D12 multi-GPU device group support
 */

#ifndef __VKD3D_MULTIGPU_H
#define __VKD3D_MULTIGPU_H

#include "vkd3d.h"
#include "nvidia_device_group.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Multi-GPU configuration flags */
#define VKD3D_MULTIGPU_LOAD_BALANCE      (1 << 0)
#define VKD3D_MULTIGPU_PEER_ACCESS       (1 << 1)
#define VKD3D_MULTIGPU_FRAME_PACING      (1 << 2)
#define VKD3D_MULTIGPU_AFFINITY          (1 << 3)
#define VKD3D_MULTIGPU_STATS             (1 << 4)

/* D3D12 device group creation with multi-GPU support */
HRESULT vkd3d_create_d3d12_device_group(
    ID3D12Device **device_group,
    uint32_t gpu_count,
    const uint32_t *gpu_indices);

HRESULT vkd3d_enable_multi_gpu_support(
    ID3D12Device *device,
    uint32_t flags);

HRESULT vkd3d_get_gpu_count(
    ID3D12Device *device,
    uint32_t *count);

HRESULT vkd3d_get_gpu_info(
    ID3D12Device *device,
    uint32_t gpu_index,
    vkd3d_gpu_info *info);

HRESULT vkd3d_set_gpu_affinity(
    ID3D12Device *device,
    uint32_t gpu_index,
    uint8_t affinity_mask);

HRESULT vkd3d_get_gpu_utilization(
    ID3D12Device *device,
    uint32_t gpu_index,
    float *utilization);

#ifdef __cplusplus
}
#endif

#endif /* __VKD3D_MULTIGPU_H */
