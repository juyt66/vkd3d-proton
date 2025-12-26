# VKD3D-Proton Multi-GPU NVIDIA Extension

## Overview

This extension adds native multi-GPU support to VKD3D-Proton, enabling Direct3D 12 applications to efficiently utilize multiple NVIDIA GPUs through Vulkan's device group functionality.

## Features

- **Multi-GPU Device Groups**: Seamless multi-GPU coordination
- - **Load Balancing**: Automatic work distribution across GPUs
  - - **GPU Memory Affinity**: Fine-grained VRAM allocation control
    - - **Peer Access**: Direct GPU-to-GPU communication
      - - **Frame Pacing**: Synchronized frame rendering across devices
        - - **Performance Monitoring**: Built-in GPU statistics and profiling
          - - **NVIDIA CUDA Integration**: Optional CUDA compute support
           
            - ## Supported GPUs
           
            - - RTX 40-series (Ada)
              - - RTX 30-series (Ampere)
                - - RTX 20-series (Turing)
                  - - And newer NVIDIA discrete GPUs
                   
                    - ## Components
                   
                    - ### Header Files
                    - - `nvidia_device_group.h` - Core multi-GPU device management API
                      - - `multigpu.h` - Public D3D12 multi-GPU interface
                       
                        - ### Implementation
                        - - `nvidia_device_group.c` - Vulkan-based device group implementation
                          - - `meson.build` - Build configuration
                           
                            - ## Building
                           
                            - ```bash
                              meson --prefix /path/to/install -Denable_multigpu=true builddir
                              ninja -C builddir install
                              ```

                              ## Environment Variables

                              ```bash
                              # Enable multi-GPU support
                              VKD3D_MULTIGPU=1

                              # Set GPU affinity (comma-separated indices)
                              VKD3D_GPU_AFFINITY=0,1,2

                              # Enable load balancing
                              VKD3D_LOAD_BALANCE=1

                              # Load balancing threshold (0.0-1.0)
                              VKD3D_LOAD_BALANCE_THRESHOLD=0.8

                              # Enable frame pacing
                              VKD3D_FRAME_PACING=16
                              ```

                              ## Usage Example

                              ```c
                              #include <d3d12.h>
                              #include <vkd3d_multigpu.h>

                              uint32_t gpu_indices[] = {0, 1, 2};
                              ID3D12Device *device_group;

                              // Create multi-GPU device group
                              vkd3d_create_d3d12_device_group(&device_group, 3, gpu_indices);

                              // Get GPU count
                              uint32_t gpu_count;
                              vkd3d_get_gpu_count(device_group, &gpu_count);

                              // Enable multi-GPU features
                              vkd3d_enable_multi_gpu_support(
                                  device_group,
                                  VKD3D_MULTIGPU_LOAD_BALANCE |
                                  VKD3D_MULTIGPU_PEER_ACCESS);
                              ```

                              ## Key Functions

                              ### GPU Enumeration
                              - `vkd3d_multigpu_enumerate_gpus()` - List available GPUs
                              - - `vkd3d_multigpu_free_gpu_info()` - Free GPU information
                               
                                - ### Device Management
                                - - `vkd3d_multigpu_create()` - Create multi-GPU context
                                  - - `vkd3d_multigpu_destroy()` - Clean up context
                                    - - `vkd3d_multigpu_set_primary_gpu()` - Configure primary GPU
                                     
                                      - ### Work Distribution
                                      - - `vkd3d_multigpu_submit_work()` - Submit work to GPU
                                        - - `vkd3d_multigpu_wait_for_completion()` - Synchronize completion
                                         
                                          - ### Performance
                                          - - `vkd3d_multigpu_get_optimal_gpu()` - Select best GPU for workload
                                            - - `vkd3d_multigpu_get_gpu_stats()` - Query GPU statistics
                                              - - `vkd3d_multigpu_reset_stats()` - Reset performance counters
                                               
                                                - ## License
                                               
                                                - LGPL-2.1-or-later (compatible with VKD3D-Proton)
