#include "handler.h"

void WindowHandler(const std::size_t id, const tWindowEventType type) {
    switch (type) {
        case kWindowCreated:
            InstallFrameHandler(FrameHandler, id, nullptr);
            SetNumPorts(id, 1);
            ReinitViewports(id, {-1.0f, -1.0f, 0.0f, 1.0f}, kScaleToSquare);
            AddViewport(id, {1.0f, -1.0f, 2.0f, 1.0f}, kScaleToSquare);
            AddViewport(id, {1.0f, 0.0f, 2.0f, 2.0f}, kScaleToSquare);
            break;
        case kWindowDestroyed:
            RemoveFrameHandler(FrameHandler, id, nullptr);
            break;
    }
}
