/*
 * Renderer frontend:
 *     - api for engine to interact with renderer
 */

#pragma once

#include "renderer_types.hpp"
#include "renderer_backend.hpp"

PXAPI b8 RendererInit(wchar_t *gameName, struct engine_state *engineState);
PXAPI void RendererShutdown(struct engine_state *engineState);

PXAPI void RendererOnResized(u16 width, u16 height, struct engine_state *engineState);

PXAPI b8 RendererDrawFrame(render_packet *packet);
