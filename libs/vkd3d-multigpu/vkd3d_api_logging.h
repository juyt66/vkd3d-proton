/**
 * Copyright 2024 NVIDIA Corporation
 * SPDX-License-Identifier: LGPL-2.1-or-later
   * VKD3D API Function Logging System
 */

#ifndef VKD3D_API_LOGGING_H
#define VKD3D_API_LOGGING_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef enum {
    VKD3D_LOG_LEVEL_NONE = 0,
    VKD3D_LOG_LEVEL_ERROR = 1,
    VKD3D_LOG_LEVEL_WARNING = 2,
    VKD3D_LOG_LEVEL_INFO = 3,
    VKD3D_LOG_LEVEL_DEBUG = 4,
    VKD3D_LOG_LEVEL_TRACE = 5,
} vkd3d_log_level;

typedef struct {
    FILE *log_file;
    vkd3d_log_level log_level;
    int enable_timestamps;
    int enable_gpu_stats;
    int enable_api_tracing;
} vkd3d_logger_config;

typedef struct {
    uint32_t gpu_index;
    float utilization;
    uint64_t bytes_transferred;
    uint64_t shader_compilations;
    uint64_t resource_allocations;
    uint64_t command_submissions;
    uint64_t frame_count;
} vkd3d_gpu_api_stats;

/* Logger initialization */
void vkd3d_logger_init(const char *log_file_path, vkd3d_log_level level);
void vkd3d_logger_shutdown(void);

/* Logging functions */
void vkd3d_log_error(const char *format, ...);
void vkd3d_log_warning(const char *format, ...);
void vkd3d_log_info(const char *format, ...);
void vkd3d_log_debug(const char *format, ...);
void vkd3d_log_trace(const char *format, ...);

/* API event logging */
void vkd3d_log_api_call(const char *api_name, uint32_t gpu_index, const char *details);
void vkd3d_log_device_creation(uint32_t gpu_index, uint32_t gpu_count);
void vkd3d_log_resource_allocation(uint32_t gpu_index, uint64_t size, const char *resource_type);
void vkd3d_log_command_submission(uint32_t gpu_index, uint32_t command_count);
void vkd3d_log_shader_compilation(uint32_t gpu_index, const char *shader_hash);
void vkd3d_log_frame_complete(uint32_t gpu_index, uint64_t frame_time_ms);

/* GPU Statistics */
void vkd3d_log_gpu_stats(const vkd3d_gpu_api_stats *stats);
void vkd3d_log_multigpu_load_balance(uint32_t gpu_count, const float *utilization);
void vkd3d_log_peer_access(uint32_t src_gpu, uint32_t dst_gpu, uint64_t bytes);

#endif /* VKD3D_API_LOGGING_H */
