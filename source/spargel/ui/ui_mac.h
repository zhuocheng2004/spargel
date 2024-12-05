#pragma once

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

@interface SpargelApplicationDelegate : NSObject <NSApplicationDelegate>
@end

@interface SpargelMetalView : NSView {
    struct sui_window* _swindow;
}
@end

@interface SpargelWindowDelegate : NSObject <NSWindowDelegate> {
    struct sui_window* _swindow;
}
@end

struct sui_window {
    NSWindow* window;
    SpargelWindowDelegate* delegate;
    CAMetalLayer* layer;
    float drawable_width;
    float drawable_height;

    void (*render_callback)(void*);
    void* render_data;
};
