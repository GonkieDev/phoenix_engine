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
