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

    typeface = SkTypeface::MakeFromFile("res/SF-Compact.ttf");
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
#elif defined(SK_METAL)
        title.append("Metal");
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
#elif defined(SK_METAL)
            fBackendType = Window::kMetal_BackendType;
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
