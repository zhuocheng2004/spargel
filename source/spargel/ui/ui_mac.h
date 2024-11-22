#pragma once

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

@interface SpargelApplicationDelegate : NSObject <NSApplicationDelegate>
@end

@interface SpargelMetalView : NSView {
  struct spargel_ui_window* _swindow;
}
@end

@interface SpargelWindowDelegate : NSObject <NSWindowDelegate> {
  struct spargel_ui_window* _swindow;
}
@end

struct spargel_ui_window {
  NSWindow* window;
  SpargelWindowDelegate* delegate;
  CAMetalLayer* layer;
  float drawable_width;
  float drawable_height;

  void (*render_callback)(void*);
  void* render_data;
};
