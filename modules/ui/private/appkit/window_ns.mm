#include "modules/ui/private/appkit/window_ns.h"

#import <AppKit/AppKit.h>

#include "modules/ui/private/metal/renderer_mtl.h"

@interface SpargelMetalView : NSView {
  spargel::ui::WindowNS* spargel_window_;
  NSTrackingArea* area_;
}
@end
@implementation SpargelMetalView
- (instancetype)initWithSpargelUIWindow:(spargel::ui::WindowNS*)window {
  [super init];
  spargel_window_ = window;
  area_ = [[NSTrackingArea alloc] initWithRect:self.bounds
                                       options:NSTrackingMouseMoved | NSTrackingActiveInKeyWindow
                                         owner:self
                                      userInfo:nil];
  [self addTrackingArea:area_];
  return self;
}
- (void)dealloc {
  if (area_) {
    [area_ release];
  }
  [super dealloc];
}
- (void)updateTrackingAreas {
  [self removeTrackingArea:area_];
  [area_ release];
  area_ = [[NSTrackingArea alloc] initWithRect:self.bounds
                                       options:NSTrackingMouseMoved | NSTrackingActiveInKeyWindow
                                         owner:self
                                      userInfo:nil];
  [self addTrackingArea:area_];
}
- (void)mouseMoved:(NSEvent*)event {
  // NSLog(@"mouse moved!");
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
  spargel_window_->getRenderer()->setDrawableSize(newSize.width, newSize.height);
}
@end

namespace spargel::ui {

WindowNS::WindowNS() : width_{300}, height_{300} {}
WindowNS::~WindowNS() = default;

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

  window_ = [[NSWindow alloc] initWithContentRect:rect
                                        styleMask:style
                                          backing:NSBackingStoreBuffered
                                            defer:NO
                                            screen:screen];
  window_.releasedWhenClosed = NO;
  window_.minSize = NSMakeSize(200, 200);

  [window_ makeKeyAndOrderFront:nil];
}

void WindowNS::setTitle(char const* title) {
  [window_ setTitle:[NSString stringWithUTF8String:title]];
}

void WindowNS::setWidth(int width) { width_ = width; }
void WindowNS::setHeight(int height) { height_ = height; }

int WindowNS::width() { return width_; }
int WindowNS::height() { return height_; }

void WindowNS::setRenderer(Renderer* r) {
  renderer_ = static_cast<RendererMTL*>(r);
  renderer_->setWindow(this);
  auto view = [[SpargelMetalView alloc] initWithSpargelUIWindow:this];
  view.layer = renderer_->layer();
  view.wantsLayer = YES;
  [view createDisplayLink:window_];
  window_.contentView = view;
  [view release];
}

void WindowNS::render() {
  renderer_->begin();
  delegate()->render(renderer_);
  renderer_->end();
}

RendererMTL* WindowNS::getRenderer() { return renderer_; }

void WindowNS::mouseMoved(double x, double y) { delegate()->onMouseMove(x, y); }

void WindowNS::mouseDown(double x, double y) { delegate()->onMouseDown(x, y); }

NSRect WindowNS::toBacking(double x, double y, double width, double height) {
  return [window_ convertRectToBacking:{{x, y}, {width, height}}];
}

Window* createAppKitWindow() { return new WindowNS; }

}  // namespace spargel::ui
