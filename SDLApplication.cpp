//
// Created by Daniil Vinogradov on 29/11/2023.
//

#include "SDLApplication.hpp"
#include <include/gpu/ganesh/gl/GrGLDirectContext.h>
#include <include/gpu/GrDirectContext.h>

#import <OpenGL/gl3.h>

TestApplication* TestApplication::shared = nullptr;

int TestApplication::resizingEventWatcher(void* data, SDL_Event* event) {
    if (event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_RESIZED) {
        SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
        if (win == (SDL_Window*)data) {
            printf("resizing.....\n");
            shared->draw();
        }
    }
    return 0;
}

TestApplication::TestApplication() {
    shared = this;

    SDL_Init(SDL_INIT_EVERYTHING);
    Uint32 windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;


    SDL_Rect gScreenRect = { 0, 0, 1280, 720 };
    //Get device display mode
//    SDL_DisplayMode displayMode;
//    if( SDL_GetCurrentDisplayMode( 0, &displayMode ) == 0 )
//    {
//        gScreenRect.w = displayMode.w;
//        gScreenRect.h = displayMode.h;
//    }

    printf("Init size %d | %d\n ", gScreenRect.w, gScreenRect.h);
    window = SDL_CreateWindow("Angle Test",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              gScreenRect.w, gScreenRect.h,
                              windowFlags);


    SDL_GLContext context = SDL_GL_CreateContext(window);

    SkGraphics::Init();

    auto interface = GrGLMakeNativeInterface();
    ctx = GrDirectContexts::MakeGL(interface);

//    GrMtlBackendContext backendContext = {};
//    ctx = GrDirectContext::MakeMetal(backendContext, )

    SDL_RegisterEvents(7);
    SDL_AddEventWatch(resizingEventWatcher, window);

    bool closeRequired = false;
    while (!closeRequired) {
        static unsigned int a, b, delta;
        a = SDL_GetTicks();
        delta = a - b;

        if (delta < 1000/fpsCap) continue;
        b = a;

        SDL_Event event;
        SDL_PumpEvents();
//        printf("Frame\n");
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    // handling of close button
                    closeRequired = true;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_F1) {
//                        s_showStats = !s_showStats;
                    }
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                    switch (event.cbutton.button) {
                        case SDL_CONTROLLER_BUTTON_START:
                            closeRequired = true;
                            break;
                        case SDL_CONTROLLER_BUTTON_A:
//                            s_showStats = !s_showStats;
                            break;
                    }
                    break;
            }
        }

        draw();
    }

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void TestApplication::draw() {
    int w, h;
    SDL_GetWindowSizeInPixels(window, &w, &h);

    glViewport(0, 0, w, h);

    GrBackendRenderTarget target = GrBackendRenderTargets::MakeGL(w,
                                                                  h,
                                                                  1,
                                                                  8,
                                                                  GrGLFramebufferInfo{0, GL_RGBA8});

    static const SkSurfaceProps surfaceProps;
    auto surface = SkSurfaces::WrapBackendRenderTarget(ctx.get(), target, kBottomLeft_GrSurfaceOrigin,
                                                       kRGBA_8888_SkColorType,
                                                       nullptr,
                                                       &surfaceProps);

    SkCanvas* canvas = surface->getCanvas();

    canvas->clear(SK_ColorWHITE);

    SkPaint paint;
    paint.setColor(SK_ColorRED);

    // Draw a rectangle with red paint
    SkRect rect = SkRect::MakeXYWH(0, 40, 128, 128);
    canvas->drawRect(rect, paint);

    // Set up a linear gradient and draw a circle
    {
        SkPoint linearPoints[] = { { 0, 0 }, { 300, 300 } };
        SkColor linearColors[] = { SK_ColorGREEN, SK_ColorBLACK };
        paint.setShader(SkGradientShader::MakeLinear(linearPoints, linearColors, nullptr, 2,
                                                     SkTileMode::kMirror));
        paint.setAntiAlias(true);

        canvas->drawCircle(200, 200, 64, paint);

        // Detach shader
        paint.setShader(nullptr);
    }

    // Draw a message with a nice black paint
    SkFont font;
    font.setSubpixel(true);
    font.setSize(20);
    paint.setColor(SK_ColorBLACK);

    canvas->save();
    static const char message[] = "Hello World ";

    // Translate and rotate
    canvas->translate(300, 300);
//        fRotationAngle += 0.2f;
//        if (fRotationAngle > 360) {
//            fRotationAngle -= 360;
//        }
//        canvas->rotate(fRotationAngle);

    // Draw the text
    canvas->drawSimpleText(message, strlen(message), SkTextEncoding::kUTF8, 0, 0, font, paint);

    canvas->restore();

    if (auto dContext = GrAsDirectContext(canvas->recordingContext())) {
        dContext->flushAndSubmit();
    }

    SDL_GL_SwapWindow(window);
}