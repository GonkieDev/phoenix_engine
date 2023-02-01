#include <core/engine.hpp>

#include "renderer_backend.cpp"

PXAPI b8
RendererInit(wchar_t *gameName, struct engine_state *engineState)
{
    engineState->rendererBackend = {};

    // TODO: convert gameName to char *
    char *appName = "Phoenix Engine App";
    if (!InitRendererBackend(appName, &engineState->rendererBackend, engineState))
    {
        PXFATAL("Renderer backend failed to create!");
        return 0;
    }

    return 1;
}

PXAPI void
RendererShutdown(struct engine_state *engineState)
{
    ShutdownRendererBackend(&engineState->rendererBackend);
}

PXAPI b8
RendererBeginFrame(render_packet *packet, renderer_backend *backend)
{
    return RendererBackendBeginFrame(packet->deltaTime, backend);
}

PXAPI b8
RendererEndFrame(render_packet *packet, renderer_backend *backend)
{
    b8 result = RendererBackendEndFrame(packet->deltaTime, backend);
#if _DEBUG
    backend->frameNumber++;
#endif
    return result;
}

PXAPI b8
RendererDrawFrame(render_packet *packet, engine_state *engineState)
{
    if (RendererBeginFrame(packet, &engineState->rendererBackend))
    {
        b8 renderResult = RendererEndFrame(packet, &engineState->rendererBackend);

        if (!renderResult)
        {
            PXERROR("RendererBackendFrame() failed. Application shutting down...");
            return 0;
        }
    }

    return true;
}

PXAPI inline void
RendererOnResized(u16 width, u16 height, struct engine_state *engineState)
{
    RendererBackendOnResize(width, height, &engineState->rendererBackend);
}
