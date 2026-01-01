#pragma once
#include <cstddef>
#include <cstdint>

// Create an SSBO from initial data. Returns true on success.
bool ssbo_create_from_data(const void *data, size_t byteSize);

// Update entire SSBO contents. byteSize must be <= original size.
void ssbo_update(const void *data, size_t byteSize);

// Bind SSBO to a shader storage binding point.
void ssbo_bind_base(uint32_t binding);

// Get underlying buffer handle (0 if none).
uint32_t ssbo_get_buffer();

// Destroy SSBO and free resources.
void ssbo_destroy();