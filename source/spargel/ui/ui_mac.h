#pragma once

#include <spargel/base/unique_ptr.h>
#include <spargel/ui/ui.h>

// platform
#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

@interface SpargelApplicationDelegate : NSObject <NSApplicationDelegate>
@end

@interface SpargelMetalView : NSView
@end

@interface SpargelWindowDelegate : NSObject <NSWindowDelegate>
@end

namespace spargel::ui {

    class platform_appkit final : public platform {
    public:
        platform_appkit();
        ~platform_appkit() override;

        void start_loop() override;

        base::unique_ptr<window> make_window(int width, int height) override;

    private:
        void init_global_menu();

        NSApplication* _app;
    };

    base::unique_ptr<platform> make_platform_appkit();

    class window_appkit final : public window {
    public:
        window_appkit(int width, int height);
        ~window_appkit() override;

        void set_title(char const* title) override;

        window_handle handle() override;

        // todo: remove
        void _set_drawable_size(float width, float height);

        // todo: refactor
        void _bridge_render();

    private:
        NSWindow* _window;
        SpargelWindowDelegate* _bridge;
        CAMetalLayer* _layer;
        float _drawable_width;
        float _drawable_height;
    };

}  // namespace spargel::ui
