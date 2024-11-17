#include "modules/ui/private/appkit/window_ns.h"

#import <AppKit/AppKit.h>

#include "modules/ui/private/metal/renderer_mtl.h"

@interface SpargelMetalView : NSView {
  spargel::ui::window_ns* spargel_window_;
  NSTrackingArea* area_;
}
@end
@implementation SpargelMetalView
- (instancetype)initWithSpargelUIWindow:(spargel::ui::window_ns*)window {
  [super init];
  spargel_window_ = window;
  area_ = [[NSTrackingArea alloc] initWithRect:self.bounds
                                       options:NSTrackingMouseMoved | NSTrackingActiveInKeyWindow
                                         owner:self
                                      userInfo:nil];
  [self addTrackingArea:area_];
  return self;
}
- (void)updateTrackingAreas {
  [self removeTrackingArea:area_];
  area_ = [[NSTrackingArea alloc] initWithRect:self.bounds
                                       options:NSTrackingMouseMoved | NSTrackingActiveInKeyWindow
                                         owner:self
                                      userInfo:nil];
  [self addTrackingArea:area_];
}
- (void)mouseMoved:(NSEvent*)event {
  // NSLog(@"mouse moved!");
  auto location = [event locationInWindow];
  spargel_window_->mouse_moved(location.x, location.y);
}
- (void)mouseDown:(NSEvent *)event {
  auto location = [event locationInWindow];
  spargel_window_->mouse_down(location.x, location.y);
}
- (void)createDisplayLink:(NSWindow*)window {
  auto display_link = [window displayLinkWithTarget:self selector:@selector(render:)];
  [display_link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
}
- (void)render:(CADisplayLink*)sender {
  // NSLog(@"%f", sender.targetTimestamp);
  spargel_window_->render();
}
- (void)viewDidChangeBackingProperties {
  [super viewDidChangeBackingProperties];
  [self resizeDrawable:self.window.screen.backingScaleFactor];
}
- (void)setFrameSize:(NSSize)size {
  [super setFrameSize:size];
  [self resizeDrawable:self.window.screen.backingScaleFactor];
}
- (void)setBoundsSize:(NSSize)size {
  [super setBoundsSize:size];
  [self resizeDrawable:self.window.screen.backingScaleFactor];
}
- (void)resizeDrawable:(CGFloat)scaleFactor {
  CGSize newSize = self.bounds.size;
  newSize.width *= scaleFactor;
  newSize.height *= scaleFactor;

  if (newSize.width <= 0 || newSize.width <= 0) {
    return;
  }
  spargel_window_->get_renderer()->set_drawable_size(newSize.width, newSize.height);
}
@end

namespace spargel::ui {

window_ns::window_ns() : width_{300}, height_{300} {}
window_ns::~window_ns() = default;

void window_ns::init(int width, int height) {
  @autoreleasepool {
    width_ = width;
    height_ = height;

    auto screen = [NSScreen mainScreen];

    auto style = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
                 NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;
    auto rect = NSMakeRect(0, 0, width_, height_);
    // center the window
    rect.origin.x = (screen.frame.size.width - width_) / 2;
    rect.origin.y = (screen.frame.size.height - height_) / 2;

    window_ = [[NSWindow alloc] initWithContentRect:rect
                                          styleMask:style
                                            backing:NSBackingStoreBuffered
                                              defer:NO
                                             screen:screen];
    window_.releasedWhenClosed = NO;
    window_.minSize = NSMakeSize(200, 200);

    // auto delegate = [[SpargelWindowDelegate alloc] init];
    // window_.delegate = delegate_;

    // auto view = [[SpargelContentView alloc] init];
    // window_.contentView = view_;
    // [window_ makeFirstResponder:view_];

    [window_ makeKeyAndOrderFront:nil];
  }
}

void window_ns::set_title(char const* title) {
  [window_ setTitle:[NSString stringWithUTF8String:title]];
}

void window_ns::set_width(int width) { width_ = width; }
void window_ns::set_height(int height) { height_ = height; }

int window_ns::width() { return width_; }
int window_ns::height() { return height_; }

void window_ns::bind_renderer(renderer* r) {
  renderer_ = static_cast<renderer_mtl*>(r);
  renderer_->set_window(this);
  auto view = [[SpargelMetalView alloc] initWithSpargelUIWindow:this];
  view.layer = renderer_->layer();
  view.wantsLayer = YES;
  [view createDisplayLink:window_];
  window_.contentView = view;
}

void window_ns::render() {
  renderer_->begin();
  delegate()->render(renderer_);
  renderer_->end();
}

renderer_mtl* window_ns::get_renderer() { return renderer_; }

void window_ns::mouse_moved(double x, double y) { delegate()->on_mouse_move(x, y); }

void window_ns::mouse_down(double x, double y) { delegate()->on_mouse_down(x, y); }

NSRect window_ns::to_backing(double x, double y, double width, double height) {
  return [window_ convertRectToBacking:{{x, y}, {width, height}}];
}

window* create_appkit_window() { return new window_ns; }

}  // namespace spargel::ui
