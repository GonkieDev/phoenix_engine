/*
 * Renderer backend:
 *     - low level interface with graphics API
 *     - interacts with frontend only and not by engine itself
 */

#pragma once

#include "renderer_types.hpp"

struct renderer_backend {
#if _DEBUG
    u64 frameNumber;
#endif
};

PXAPI b8   InitRendererBackend(char *appName, renderer_backend *backend, struct engine_state *engineState);
PXAPI void ShutdownRendererBackend(renderer_backend *backend);
PXAPI void RendererBackendOnResize(u16 width, u16 height, renderer_backend *backend);
PXAPI b8   RendererBackendBeginFrame(f32 deltaTime, renderer_backend *backend);
PXAPI b8   RendererBackendEndFrame(f32 deltaTime, renderer_backend *backend);
