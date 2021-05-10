#pragma once

#include <cstdint>

typedef void* Handle;

Handle create_compute_instance();
void destroy_compute_instance(Handle instance);

Handle create_compute_device(Handle instance);
void destroy_compute_device(Handle device);

Handle create_compute_pipeline(Handle device, const char* shader_file);
void destroy_compute_pipeline(Handle device, Handle pipeline);

Handle create_compute_storage_buffer(Handle device, uint64_t size);
void* map_memory_from_buffer(Handle device, Handle buffer, uint64_t size, uint64_t offset);
void unmap_memory_from_buffer(Handle device, Handle buffer);
void destroy_compute_storage_buffer(Handle device, Handle buffer);

Handle create_compute_storage_image(Handle device, uint32_t width, uint32_t height, uint32_t depth);
void destroy_compute_storage_image(Handle device, Handle image);

void execute_compute_command(Handle device, Handle pipeline, uint64_t input_buffer_count, Handle* input_buffers, uint64_t output_buffer_count, Handle* output_buffers);
