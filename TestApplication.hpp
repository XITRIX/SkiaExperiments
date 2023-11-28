//
// Created by Daniil Vinogradov on 23/11/2023.
//

#pragma once

#include <SDL.h>

#include <skia.h>
#include "tools/sk_app/Application.h"
#include "tools/sk_app/Window.h"
#include "tools/skui/ModifierKey.h"

class SkSurface;

class HelloWorld : public sk_app::Application, sk_app::Window::Layer {
public:
    HelloWorld(int argc, char** argv, void* platformData);
    ~HelloWorld() override;

    void onIdle() override;

    void onBackendCreated() override;
    void onPaint(SkSurface*) override;
    bool onChar(SkUnichar c, skui::ModifierKey modifiers) override;

private:
    void updateTitle();

    sk_app::Window* fWindow;
    sk_app::Window::BackendType fBackendType;

    SkScalar fRotationAngle;
};

class TestApplication {
public:
    TestApplication();

private:
    const int fpsCap = 120;
    SDL_Renderer *renderer = nullptr;
    SDL_Window *window = nullptr;
};
