//
// Created by Daniil Vinogradov on 23/11/2023.
//

#include <cstdio>
#include "TestApplication.hpp"

#include <string.h>

using namespace sk_app;
using skwindow::DisplayParams;

Application* Application::Create(int argc, char** argv, void* platformData) {
    return new HelloWorld(argc, argv, platformData);
}

HelloWorld::HelloWorld(int argc, char** argv, void* platformData)
#if defined(SK_GL)
: fBackendType(Window::kNativeGL_BackendType),
#elif defined(SK_VULKAN)
: fBackendType(Window::kVulkan_BackendType),
#elif defined(SK_METAL)
: fBackendType(Window::kMetal_BackendType),
#else
        : fBackendType(Window::kRaster_BackendType),
#endif
          fRotationAngle(0) {
    SkGraphics::Init();

    fWindow = Window::CreateNativeWindow(platformData);
    fWindow->setRequestedDisplayParams(DisplayParams());

    // register callbacks
    fWindow->pushLayer(this);

    fWindow->attach(fBackendType);
}

HelloWorld::~HelloWorld() {
    fWindow->detach();
    delete fWindow;
}

void HelloWorld::updateTitle() {
    if (!fWindow) {
        return;
    }

    SkString title("Hello World ");
    if (Window::kRaster_BackendType == fBackendType) {
        title.append("Raster");
    } else {
#if defined(SK_GL)
        title.append("GL");
#elif defined(SK_VULKAN)
        title.append("Vulkan");
#elif defined(SK_DAWN)
        title.append("Dawn");
#else
        title.append("Unknown GPU backend");
#endif
    }

    fWindow->setTitle(title.c_str());
}

void HelloWorld::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();
}

void HelloWorld::onPaint(SkSurface* surface) {
    auto canvas = surface->getCanvas();

    // Clear background
    canvas->clear(SK_ColorWHITE);

    SkPaint paint;
    paint.setColor(SK_ColorRED);

    // Draw a rectangle with red paint
    SkRect rect = SkRect::MakeXYWH(10, 10, 128, 128);
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
    fRotationAngle += 0.2f;
    if (fRotationAngle > 360) {
        fRotationAngle -= 360;
    }
    canvas->rotate(fRotationAngle);

    // Draw the text
    canvas->drawSimpleText(message, strlen(message), SkTextEncoding::kUTF8, 0, 0, font, paint);

    canvas->restore();
}

void HelloWorld::onIdle() {
    // Just re-paint continuously
    fWindow->inval();
}

bool HelloWorld::onChar(SkUnichar c, skui::ModifierKey modifiers) {
    if (' ' == c) {
        if (Window::kRaster_BackendType == fBackendType) {
#if defined(SK_GL)
            fBackendType = Window::kNativeGL_BackendType;
#elif defined(SK_VULKAN)
            fBackendType = Window::kVulkan_BackendType;
#else
            SkDebugf("No GPU backend configured\n");
            return true;
#endif
        } else {
            fBackendType = Window::kRaster_BackendType;
        }
        fWindow->detach();
        fWindow->attach(fBackendType);
    }
    return true;
}

/////////////////////////////////////////////////////////////////

TestApplication::TestApplication() {
    SDL_Init(SDL_INIT_EVERYTHING);
    Uint32 windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;


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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    SkGraphics::Init();

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


        SDL_SetRenderDrawColor( renderer, 0x00, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( renderer );
//
        SDL_RenderPresent( renderer );

        // Handle window resize.
//        update();
    }

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}