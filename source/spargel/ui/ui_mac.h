#pragma once

#include <spargel/ui/ui.h>

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

@interface SpargelApplicationDelegate : NSObject <NSApplicationDelegate>
@end

@interface SpargelMetalView : NSView {
    spargel::ui::window* _swindow;
}
@end

@interface SpargelWindowDelegate : NSObject <NSWindowDelegate> {
    spargel::ui::window* _swindow;
}
@end

namespace spargel::ui {

    struct window {
        NSWindow* window;
        SpargelWindowDelegate* delegate;
        CAMetalLayer* layer;
        float drawable_width;
        float drawable_height;

        void (*render_callback)(void*);
        void* render_data;
    };

}  // namespace spargel::ui
