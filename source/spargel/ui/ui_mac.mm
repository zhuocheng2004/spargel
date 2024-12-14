#include <spargel/base/allocator.h>
#include <spargel/base/assert.h>
#include <spargel/ui/ui.h>
#include <spargel/ui/ui_mac.h>

// platform
#include <Carbon/Carbon.h>

@implementation SpargelApplicationDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}
@end

@implementation SpargelMetalView {
    spargel::ui::window_appkit* _bridge;
}
- (instancetype)initWithSpargelUIWindow:(spargel::ui::window_appkit*)w {
    [super init];
    _bridge = w;
    return self;
}
- (void)createDisplayLink:(NSWindow*)window {
    CADisplayLink* display_link = [window displayLinkWithTarget:self selector:@selector(render:)];
    [display_link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
}
- (void)render:(CADisplayLink*)sender {
    _bridge->_bridge_render();
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
    _bridge->_set_drawable_size((float)newSize.width, (float)newSize.height);
}
- (void)keyDown:(NSEvent*)event {
    auto code = [event keyCode];
    _bridge->_bridge_key_down(code);
    // [self interpretKeyEvents:@[ event ]];
}
- (BOOL)acceptsFirstResponder {
    return YES;
}
@end

@implementation SpargelWindowDelegate {
    spargel::ui::window_appkit* _bridge;
}
- (instancetype)initWithSpargelUIWindow:(spargel::ui::window_appkit*)w {
    [super init];
    _bridge = w;
    return self;
}
@end

namespace spargel::ui {

    base::unique_ptr<platform> make_platform_appkit() {
        return base::make_unique<platform_appkit>();
    }

    platform_appkit::platform_appkit() : platform(platform_kind::appkit) {
        _app = [NSApplication sharedApplication];
        [_app setActivationPolicy:NSApplicationActivationPolicyRegular];

        SpargelApplicationDelegate* delegate = [[SpargelApplicationDelegate alloc] init];
        // NSApp.delegate is a weak reference
        _app.delegate = delegate;

        init_global_menu();
    }

    platform_appkit::~platform_appkit() {}

    void platform_appkit::start_loop() { [_app run]; }

    base::unique_ptr<window> platform_appkit::make_window(int width, int height) {
        spargel_assert(width > 0 && height > 0);
        return base::make_unique<window_appkit>(width, height);
    }

    void platform_appkit::init_global_menu() {
        NSMenu* menu_bar = [[NSMenu alloc] init];

        NSMenu* app_menu = [[NSMenu alloc] initWithTitle:@"Spargel"];
        [app_menu addItemWithTitle:@"About Spargel" action:nil keyEquivalent:@""];
        [app_menu addItemWithTitle:@"Check for Updates" action:nil keyEquivalent:@""];
        [app_menu addItemWithTitle:@"Quit Spargel" action:@selector(terminate:) keyEquivalent:@"q"];

        NSMenuItem* app_menu_item = [[NSMenuItem alloc] init];
        [app_menu_item setSubmenu:app_menu];
        [menu_bar addItem:app_menu_item];

        // mainMenu is a strong reference
        _app.mainMenu = menu_bar;
    }

    window_appkit::window_appkit(int width, int height) {
        NSScreen* screen = [NSScreen mainScreen];

        NSWindowStyleMask style = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
                                  NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;

        NSRect rect = NSMakeRect(0, 0, width, height);
        // center the window
        rect.origin.x = (screen.frame.size.width - width) / 2;
        rect.origin.y = (screen.frame.size.height - height) / 2;

        _bridge = [[SpargelWindowDelegate alloc] initWithSpargelUIWindow:this];

        _window = [[NSWindow alloc] initWithContentRect:rect
                                              styleMask:style
                                                backing:NSBackingStoreBuffered
                                                  defer:NO
                                                 screen:screen];
        // weak reference
        _window.delegate = _bridge;
        _window.releasedWhenClosed = NO;
        // TODO: fix this
        _window.minSize = NSMakeSize(200, 200);

        _layer = [[CAMetalLayer alloc] init];

        SpargelMetalView* view = [[SpargelMetalView alloc] initWithSpargelUIWindow:this];
        view.layer = _layer;
        view.wantsLayer = YES;
        [view createDisplayLink:_window];

        // strong reference
        _window.contentView = view;

        [view release];

        [_window makeKeyAndOrderFront:nil];
    }

    window_appkit::~window_appkit() {
        [_window release];
        [_bridge release];
    }

    void window_appkit::set_title(char const* title) {
        _window.title = [NSString stringWithUTF8String:title];
    }

    void window_appkit::_set_drawable_size(float width, float height) {
        _drawable_width = width;
        _drawable_height = height;
        _layer.drawableSize = NSMakeSize(width, height);
    }

    window_handle window_appkit::handle() {
        window_handle handle;
        handle.apple.layer = _layer;
        return handle;
    }

    void window_appkit::_bridge_render() {
        if (delegate() != nullptr) {
            delegate()->render();
        }
    }

    void window_appkit::_bridge_key_down(int key) {
        if (delegate() != nullptr) {
            keyboard_event e{key};
            delegate()->on_keyboard(e);
        }
    }

}  // namespace spargel::ui
