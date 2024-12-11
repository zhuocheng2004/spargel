#include <spargel/base/base.h>
#include <spargel/ui/ui.h>
#include <spargel/ui/ui_mac.h>

/* platform */
#include <Carbon/Carbon.h>

namespace spargel::ui {
    static void set_drawable_size(struct window* window, float width, float height);
    static void window_render(struct window* window);
}  // namespace spargel::ui

@implementation SpargelApplicationDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}
@end

@implementation SpargelMetalView
- (instancetype)initWithSpargelUIWindow:(spargel::ui::window*)window {
    [super init];
    _swindow = window;
    return self;
}
- (void)createDisplayLink:(NSWindow*)window {
    CADisplayLink* display_link = [window displayLinkWithTarget:self selector:@selector(render:)];
    [display_link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
}
- (void)render:(CADisplayLink*)sender {
    window_render(_swindow);
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
    set_drawable_size(_swindow, (float)newSize.width, (float)newSize.height);
}
/*
- (void)keyDown:(NSEvent*)event {
  NSLog(@"keyDown!");
  unsigned short keyCode = [event keyCode];
  if (keyCode == kVK_Escape) {
    spargel::base::print_backtrace();
  }
  [self interpretKeyEvents:@[ event ]];
}
*/
- (BOOL)acceptsFirstResponder {
    return YES;
}
@end

@implementation SpargelWindowDelegate
- (instancetype)initWithSpargelUIWindow:(spargel::ui::window*)window {
    [super init];
    _swindow = window;
    return self;
}
@end

namespace spargel::ui {

    static void init_global_menu() {
        NSMenu* menu_bar = [[NSMenu alloc] init];

        NSMenu* app_menu = [[NSMenu alloc] initWithTitle:@"Spargel"];
        [app_menu addItemWithTitle:@"About Spargel" action:nil keyEquivalent:@""];
        [app_menu addItemWithTitle:@"Check for Updates" action:nil keyEquivalent:@""];
        [app_menu addItemWithTitle:@"Quit Spargel" action:@selector(terminate:) keyEquivalent:@"q"];

        NSMenuItem* app_menu_item = [[NSMenuItem alloc] init];
        [app_menu_item setSubmenu:app_menu];
        [menu_bar addItem:app_menu_item];

        /* mainMenu is a strong reference */
        NSApp.mainMenu = menu_bar;
    }

    void init_platform() {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        SpargelApplicationDelegate* delegate = [[SpargelApplicationDelegate alloc] init];
        /* NSApp.delegate is a weak reference */
        NSApp.delegate = delegate;

        init_global_menu();
    }

    int platform_id() { return PLATFORM_APPKIT; }

    void platform_run() {
        @autoreleasepool {
            /* run until stop: or terminate: */
            [NSApp run];
        }
    }

    window_id create_window(int width, int height) {
        spargel_assert(width > 0 && height > 0);

        window_id window = (struct window*)malloc(sizeof(struct window));
        if (!window) return NULL;
        window->render_callback = NULL;
        window->render_data = NULL;

        NSScreen* screen = [NSScreen mainScreen];

        NSWindowStyleMask style = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
                                  NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;

        NSRect rect = NSMakeRect(0, 0, width, height);
        /* center the window */
        rect.origin.x = (screen.frame.size.width - width) / 2;
        rect.origin.y = (screen.frame.size.height - height) / 2;

        window->delegate = [[SpargelWindowDelegate alloc] initWithSpargelUIWindow:window];

        window->window = [[NSWindow alloc] initWithContentRect:rect
                                                     styleMask:style
                                                       backing:NSBackingStoreBuffered
                                                         defer:NO
                                                        screen:screen];
        /* weak reference */
        window->window.delegate = window->delegate;
        window->window.releasedWhenClosed = NO;
        window->window.minSize = NSMakeSize(200, 200);

        window->layer = [[CAMetalLayer alloc] init];

        SpargelMetalView* view = [[SpargelMetalView alloc] initWithSpargelUIWindow:window];
        view.layer = window->layer;
        view.wantsLayer = YES;
        [view createDisplayLink:window->window];

        /* strong reference */
        window->window.contentView = view;
        [view release];

        [window->window makeKeyAndOrderFront:nil];

        return window;
    }

    void destroy_window(window_id window) {
        [window->window release];
        [window->delegate release];
    }

    void window_set_title(window_id window, char const* title) {
        window->window.title = [NSString stringWithUTF8String:title];
    }

    void window_set_render_callback(window_id window, void (*render)(void*), void* data) {
        window->render_callback = render;
        window->render_data = data;
    }

    static void set_drawable_size(struct window* window, float width, float height) {
        window->drawable_width = width;
        window->drawable_height = height;
        window->layer.drawableSize = NSMakeSize(width, height);
    }

    static void window_render(struct window* window) {
        if (window->render_callback) {
            window->render_callback(window->render_data);
        }
    }

    struct window_handle window_get_handle(window_id window) {
        struct window_handle handle;
        handle.apple.layer = window->layer;
        return handle;
    }

}  // namespace spargel::ui
