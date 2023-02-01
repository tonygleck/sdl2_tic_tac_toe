
#pragma once

#include <SDL2/SDL.h>

#include "umock_c/umock_c_prod.h"

#define SDL_PushEvent mocked_SDL_PushEvent

#ifdef __cplusplus
extern "C"
{
#endif

MOCKABLE_FUNCTION(, int, mocked_SDL_PushEvent, SDL_Event*, event);

#ifdef __cplusplus
}
#endif
