/**
 * Copyright 2024 NVIDIA Corporation
 * SPDX-License-Identifier: LGPL-2.1-or-later
   * VKD3D Multi-GPU Monitoring & Profiling System
 * 
 * Comprehensive monitoring infrastructure for VKD3D-Proton multi-GPU support
   * Includes: API logging, performance profiling, memory tracking,
   *           command visualization, and real-time dashboard
 */

#ifndef VKD3D_MONITORING_H
#define VKD3D_MONITORING_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

/* ===== Logging System ===== */
typedef enum {
    VKD3D_LOG_LEVEL_NONE = 0,
    VKD3D_LOG_LEVEL_ERROR = 1,
    VKD3D_LOG_LEVEL_WARNING = 2,
    VKD3D_LOG_LEVEL_INFO = 3,
    VKD3D_LOG_LEVEL_DEBUG = 4,
    VKD3D_LOG_LEVEL_TRACE = 5,
} vkd3d_log_level;

typedef struct {
    uint32_t gpu_index;
    float utilization;
    uint64_t bytes_transferred;
    uint64_t shader_compilations;
    uint64_t resource_allocations;
    uint64_t command_submissions;
    uint64_t frame_count;
} vkd3d_gpu_api_stats;

void vkd3d_logger_init(const char *log_file_path, vkd3d_log_level level);
void vkd3d_logger_shutdown(void);
void vkd3d_log_error(const char *format, ...);
void vkd3d_log_warning(const char *format, ...);
void vkd3d_log_info(const char *format, ...);
void vkd3d_log_debug(const char *format, ...);
void vkd3d_log_trace(const char *format, ...);
void vkd3d_log_api_call(const char *api_name, uint32_t gpu_index, const char *details);
void vkd3d_log_device_creation(uint32_t gpu_index, uint32_t gpu_count);
void vkd3d_log_resource_allocation(uint32_t gpu_index, uint64_t size, const char *resource_type);
void vkd3d_log_command_submission(uint32_t gpu_index, uint32_t command_count);
void vkd3d_log_gpu_stats(const vkd3d_gpu_api_stats *stats);

/* ===== Profiler System ===== */
typedef struct {
    const char *marker_name;
    uint64_t duration_ns;
    uint32_t gpu_index;
    uint32_t call_count;
    float average_duration_ms;
} vkd3d_profile_marker;

typedef struct {
    uint64_t command_count;
    uint32_t submission_count;
    uint64_t max_command_batch_size;
} vkd3d_command_list_stats;

typedef struct {
    uint64_t frame_number;
    float frame_time_ms;
    float fps;
    uint32_t gpu_count;
    vkd3d_command_list_stats *command_stats;
} vkd3d_frame_profile;

void vkd3d_profiler_init(uint32_t gpu_count);
void vkd3d_profiler_shutdown(void);
void vkd3d_profiler_frame_begin(void);
void vkd3d_profiler_frame_end(void);
void vkd3d_profiler_command_list_begin(uint32_t gpu_index);
void vkd3d_profiler_command_list_end(uint32_t gpu_index, uint64_t command_count);
vkd3d_frame_profile* vkd3d_profiler_get_frame_stats(void);
void vkd3d_profiler_print_summary(void);
void vkd3d_profiler_export_csv(const char *filename);

/* ===== Memory Tracker System ===== */
typedef enum {
    VKD3D_MEMORY_TYPE_VRAM = 0,
    VKD3D_MEMORY_TYPE_SHARED = 1,
    VKD3D_MEMORY_TYPE_HOST = 2,
} vkd3d_memory_type;

typedef struct {
    uint64_t total_allocated;
    uint64_t total_freed;
    uint64_t peak_usage;
    uint64_t current_usage;
    uint32_t allocation_count;
} vkd3d_gpu_memory_stats;

void vkd3d_memory_tracker_init(uint32_t gpu_count);
void vkd3d_memory_tracker_shutdown(void);
void vkd3d_memory_track_allocation(uint64_t allocation_id, uint64_t size, vkd3d_memory_type type, uint32_t gpu_index, const char *name);
void vkd3d_memory_track_deallocation(uint64_t allocation_id);
vkd3d_gpu_memory_stats* vkd3d_memory_get_stats(uint32_t gpu_index);
void vkd3d_memory_print_report(void);
void vkd3d_memory_set_warning_threshold(uint64_t threshold_bytes);
uint64_t vkd3d_memory_get_used(uint32_t gpu_index);

/* ===== Command Visualizer System ===== */
typedef enum {
    VKD3D_COMMAND_TYPE_DRAW = 0,
    VKD3D_COMMAND_TYPE_COMPUTE = 1,
    VKD3D_COMMAND_TYPE_COPY = 2,
    VKD3D_COMMAND_TYPE_CLEAR = 3,
    VKD3D_COMMAND_TYPE_BARRIER = 4,
} vkd3d_command_type;

typedef struct {
    vkd3d_command_type type;
    const char *description;
    uint32_t gpu_index;
    uint64_t timestamp;
} vkd3d_command_record;

void vkd3d_command_visualizer_init(uint32_t gpu_count);
void vkd3d_command_visualizer_shutdown(void);
void vkd3d_command_record_draw(uint32_t gpu_index, const char *description);
void vkd3d_command_record_compute(uint32_t gpu_index, const char *description);
void vkd3d_command_record_copy(uint32_t gpu_index, uint64_t size);
void vkd3d_command_visualizer_print_timeline(uint32_t gpu_index);
void vkd3d_command_visualizer_export_html(const char *filename);
uint32_t vkd3d_command_get_count_by_type(uint32_t gpu_index, vkd3d_command_type type);

/* ===== Dashboard System ===== */
typedef struct {
    uint32_t gpu_index;
    float gpu_utilization;
    float memory_utilization;
    uint64_t memory_used_mb;
} vkd3d_gpu_status;

typedef struct {
    vkd3d_gpu_status *gpu_statuses;
    uint32_t gpu_count;
    float current_fps;
} vkd3d_dashboard_state;

void vkd3d_dashboard_init(uint32_t gpu_count);
void vkd3d_dashboard_shutdown(void);
void vkd3d_dashboard_update(void);
void vkd3d_dashboard_print_status(void);
void vkd3d_dashboard_export_json(const char *filename);
vkd3d_dashboard_state* vkd3d_dashboard_get_state(void);

/* ===== Main Integration Interface ===== */
void vkd3d_multigpu_monitoring_init(uint32_t gpu_count, const char *log_file_path);
void vkd3d_multigpu_monitoring_shutdown(void);
void vkd3d_multigpu_export_statistics(const char *output_dir);

#endif /* VKD3D_MONITORING_H */
