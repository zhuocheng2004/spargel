#include "modules/ui/private/appkit/window_ns.h"

#import <AppKit/AppKit.h>

#include "modules/ui/private/metal/render_target_mtl.h"
#include "modules/ui/public/window_delegate.h"

@implementation SpargelWindowDelegate
- (instancetype)initWithSpargelUIWindow:(spargel::ui::WindowNS*)window {
  [super init];
  _window = window;
  return self;
}
- (void)windowWillClose:(NSNotification*)notification {
  _window->willClose();
}
@end

@interface SpargelMetalView : NSView {
  spargel::ui::WindowNS* spargel_window_;
  NSTrackingArea* area_;
}
@end
@implementation SpargelMetalView
- (instancetype)initWithSpargelUIWindow:(spargel::ui::WindowNS*)window {
  [super init];
  spargel_window_ = window;
  area_ = [self createTrackingArea];
  [self addTrackingArea:area_];
  return self;
}
- (void)dealloc {
  if (area_) {
    [area_ release];
  }
  [super dealloc];
}
- (NSTrackingArea*)createTrackingArea {
  return [[NSTrackingArea alloc] initWithRect:self.bounds
                                      options:NSTrackingMouseMoved | NSTrackingActiveInKeyWindow
                                        owner:self
                                     userInfo:nil];
}
- (void)updateTrackingAreas {
  [self removeTrackingArea:area_];
  [area_ release];
  area_ = [self createTrackingArea];
  [self addTrackingArea:area_];
}
- (void)mouseMoved:(NSEvent*)event {
  auto location = [event locationInWindow];
  spargel_window_->mouseMoved(location.x, location.y);
}
- (void)mouseDown:(NSEvent*)event {
  auto location = [event locationInWindow];
  spargel_window_->mouseDown(location.x, location.y);
}
- (void)createDisplayLink:(NSWindow*)window {
  auto display_link = [window displayLinkWithTarget:self selector:@selector(render:)];
  [display_link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
}
- (void)render:(CADisplayLink*)sender {
  // todo: sender.targetTimestamp
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
  spargel_window_->setDrawableSize(
      {static_cast<float>(newSize.width), static_cast<float>(newSize.height)});
}
@end

namespace spargel::ui {

WindowNS::WindowNS() {
  width_ = 0;
  height_ = 0;
  _render_target = new RenderTargetMTL(this);
}

WindowNS::~WindowNS() {
  [window_ release];
  [_ns_delegate release];
}

void WindowNS::init(int width, int height) {
  width_ = width;
  height_ = height;

  auto screen = [NSScreen mainScreen];

  auto style = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
               NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;
  auto rect = NSMakeRect(0, 0, width_, height_);
  // center the window
  rect.origin.x = (screen.frame.size.width - width_) / 2;
  rect.origin.y = (screen.frame.size.height - height_) / 2;

  _ns_delegate = [[SpargelWindowDelegate alloc] initWithSpargelUIWindow:this];

  window_ = [[NSWindow alloc] initWithContentRect:rect
                                        styleMask:style
                                          backing:NSBackingStoreBuffered
                                            defer:NO
                                           screen:screen];
  window_.delegate = _ns_delegate;
  window_.releasedWhenClosed = NO;
  window_.minSize = NSMakeSize(200, 200);

  auto view = [[SpargelMetalView alloc] initWithSpargelUIWindow:this];
  view.layer = _render_target->layer();
  view.wantsLayer = YES;
  [view createDisplayLink:window_];

  window_.contentView = view;
  [view release];

  [window_ makeKeyAndOrderFront:nil];
}

void WindowNS::setTitle(char const* title) {
  [window_ setTitle:[NSString stringWithUTF8String:title]];
}

void WindowNS::setWidth(int width) { width_ = width; }
void WindowNS::setHeight(int height) { height_ = height; }

int WindowNS::width() { return window_.contentView.bounds.size.width; }
int WindowNS::height() { return window_.contentView.bounds.size.height; }

RenderTarget* WindowNS::renderTarget() { return _render_target; }

void WindowNS::render() { delegate()->render(); }

void WindowNS::mouseMoved(float x, float y) { delegate()->onMouseMove(x, y); }

void WindowNS::mouseDown(float x, float y) { delegate()->onMouseDown(x, y); }

void WindowNS::setDrawableSize(RectSize size) { _render_target->setSize(size); }

void WindowNS::willClose() { delegate()->onClose(); }

Rect WindowNS::toBacking(Rect rect) {
  NSRect result = [window_ convertRectToBacking:NSMakeRect(rect.origin.x, rect.origin.y,
                                                           rect.size.width, rect.size.height)];
  return {{static_cast<float>(result.origin.x), static_cast<float>(result.origin.y)},
          {static_cast<float>(result.size.width), static_cast<float>(result.size.height)}};
}

Window* createAppKitWindow() { return new WindowNS; }

}  // namespace spargel::ui
