//
// Created by Daniil Vinogradov on 29/11/2023.
//

#pragma once

#include <SDL.h>
#include <skia.h>

class TestApplication {
public:
    TestApplication();

private:
    const int fpsCap = 120;
    SDL_Renderer *renderer = nullptr;
    SDL_Window *window = nullptr;
    sk_sp<GrDirectContext> ctx;

    static TestApplication* shared;

    static int resizingEventWatcher(void* data, SDL_Event* event);
    void draw();
};

