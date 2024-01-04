#pragma once

#include "Common.h"

bool ClickHandler(std::size_t /*id*/, int viewport, int /*x*/, int /*y*/,
                  point3d p, tButtonType /*type*/, tMouseEventType event);

void FrameHandler(std::size_t id, unsigned viewport, void* /*data*/);

void KeyboardHandler(std::size_t id, tKeyboardModifier /*mod*/, char key);

void WindowHandler(std::size_t id, tWindowEventType type);
