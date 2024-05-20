//
// Created by Daniil Vinogradov on 29/11/2023.
//

#include "SDLApplication.hpp"
#include "include/ports/SkFontMgr_empty.h"
#include "include/gpu/ganesh/gl/egl/GrGLMakeEGLInterface.h"
#include "include/gpu/ganesh/gl/mac/GrGLMakeMacInterface.h"
#include "libromfs/lib/include/romfs/romfs.hpp"
#include <include/gpu/ganesh/gl/GrGLDirectContext.h>
#include <include/gpu/GrDirectContext.h>

#include <glad/glad.h>

TestApplication* TestApplication::shared = nullptr;

int TestApplication::resizingEventWatcher(void* data, SDL_Event* event) {
    if (event->type == SDL_WINDOWEVENT &&
        event->window.type == SDL_WINDOWEVENT_RESIZED) {
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
    Uint32 windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;// | SDL_WINDOW_HIGH_PIXEL_DENSITY;


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

    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    SkGraphics::Init();

//    auto interface = GrGLInterfaces::MakeEGL();// GrGLMakeNativeInterface();
#if defined(PLATFORM_MAC)
    auto interface = GrGLInterfaces::MakeMac();
#elif  defined(PLATFORM_SWITCH)
    auto interface = GrGLInterfaces::MakeEGL();
#endif
    ctx = GrDirectContexts::MakeGL(interface);
//    GrMtlBackendContext backendContext = {};
//    ctx = GrDirectContext::MakeMetal(backendContext, )

    SDL_RegisterEvents(7);
    SDL_AddEventWatch(resizingEventWatcher, window);

#if defined(PLATFORM_MAC)
    mgr = SkFontMgr_New_CoreText(nullptr);
#else defined(PLATFORM_SWITCH)
    mgr = SkFontMgr_New_Custom_Empty();
#endif

    auto file = romfs::get("SF-Compact.ttf");
    typeface = mgr->makeFromData(SkData::MakeWithoutCopy(file.data(), file.size()));

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
                case SDL_KEYUP:// SDL_EVENT_KEY_UP:
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

    // Clear background
    canvas->clear(SK_ColorWHITE);

//    sk_sp<SkImageFilter> newBlurFilter = SkImageFilters::Blur(25, 25, SkTileMode::kClamp, nullptr);

    SkPaint paint;
    paint.setColor(SK_ColorRED);
    paint.setAntiAlias(true);

//    paint.setImageFilter(std::move(newBlurFilter));

    // Draw a rectangle with red paint
    SkRect rect = SkRect::MakeXYWH(10, 10, 512, 200);
    SkRRect rrect;
    SkVector corners[] = {{24, 36}, {36, 24}, {24, 24}, {24, 24}};
    rrect.setRectRadii(rect, corners);
    canvas->drawRRect(rrect, paint);

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

    SkFont font(typeface);
    font.setSubpixel(true);
    font.setSize(49);
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
    canvas->drawString(message, 200, 20, font, paint);
//    canvas->drawSimpleText(message, strlen(message), SkTextEncoding::kUTF8, 0, 0, font, paint);

    canvas->restore();

//    const SkRect middle = SkRect::MakeXYWH(64, 64, 128, 128);
//    // Use middle rectangle as clip mask
//    canvas->clipRect(middle, true);
//
//    sk_sp<SkImageFilter> newBlurFilter = SkImageFilters::Blur(25, 25, SkTileMode::kClamp, nullptr);
//    paint.setImageFilter(newBlurFilter);
//
//    SkCanvas::SaveLayerRec slr(&middle, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
//    canvas->saveLayer(slr);
//
//    // Fill the clip middle rectangle with a transparent white
//    canvas->drawColor(0x40FFFFFF);
//    canvas->restore();

    SkPaint p;
    p.setAntiAlias(true);
    p.setStyle(SkPaint::kFill_Style);
    p.setStrokeWidth(10);

//    // Draw red squares
//    p.setColor(SK_ColorRED);
//    const SkRect red = SkRect::MakeXYWH(0, 0, 128, 128);
//    canvas->drawRect(red, p);
//    const SkRect red2 = SkRect::MakeXYWH(128, 128, 128, 128);
//    canvas->drawRect(red2, p);
//
//    // Draw blue squares
//    p.setColor(SK_ColorBLUE);
////    p.setAlphaf(0.5f);
//    const SkRect blue = SkRect::MakeXYWH(128, 0, 128, 128);
//    canvas->drawRect(blue, p);
//    const SkRect blue2 = SkRect::MakeXYWH(0, 128, 128, 128);
//    canvas->drawRect(blue2, p);

    // Create middle overlay rectangle for background blur
    const SkRect middle = SkRect::MakeXYWH(64, 64, 128, 128);

    canvas->save();
    // Use middle rectangle as clip mask
    canvas->clipRect(middle, true);

    // Two blur filters, one that we're currently using and the newer one in current version of Skia.
    // Both blur filters select a tile mode for clamping the blur filter at the rectangle's edges.
    // However, the result on the CPU does NOT appear to clamp at all, while the result on GPU does!
//    sk_sp<SkImageFilter> oldBlurFilter = SkBlurImageFilter::Make(25, 25, nullptr, nullptr, SkBlurImageFilter::kClamp_TileMode);
    sk_sp<SkImageFilter> newBlurFilter = SkImageFilters::Blur(25, 25, SkTileMode::kClamp, nullptr);

    p.setImageFilter(std::move(newBlurFilter));

    // Make a separate layer using the blur filter, clipped to the middle rectangle's bounds
    SkCanvas::SaveLayerRec slr(&middle, &p, SkCanvas::kInitWithPrevious_SaveLayerFlag);
    canvas->saveLayer(slr);

    // Fill the clip middle rectangle with a transparent white
    canvas->drawColor(0x40FFFFFF);
    canvas->restore();
    canvas->restore();

    if (auto dContext = GrAsDirectContext(canvas->recordingContext())) {
        dContext->flushAndSubmit();
    }

    SDL_GL_SwapWindow(window);
}
