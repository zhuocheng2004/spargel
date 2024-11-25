# The Main Loop

## AppKit Event Loop

The standard way:

```c
NSApplicationMain(argc, argv);
```

It is equivalent to

```c
[NSApplication sharedApplication];
// todo: load main nib file
[NSApp run];
```

The crucial point is that we need to call `[NSApp run]` to properly setup the `NSApplication`.
Besides that, the event loop is rather easy:

```c
while (true) {
  @autoreleasepool {
    // polling event
    NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                        untilDate:[NSDate distantPast] // use distantFuture to wait
                                           inMode:NSDefaultRunLoopMode
                                          dequeue:YES];
    if (event == nil) break;
    [NSApp sendEvent:event];
  }
}
```

See [chromium](https://chromium.googlesource.com/chromium/src/+/main/base/message_loop/message_pump_apple.mm) and
[glfw](https://github.com/glfw/glfw/blob/master/src/cocoa_window.m).

If we decide to explicitly dispatch events like above, then the difficulty is how to finish `NSApplication` initialization.

GLFW uses the following hack

```c
@interface GLFWApplicationDelegate : NSObject <NSApplicationDelegate>
- (void)applicationDidFinishLaunching:(NSNotification*)notification;
@end
@implementation GLFWApplicationDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)notification {
  send_empty_event();
  [NSApp stop:nil];
}
// ...
void platform_init() {
  [NSApplication sharedApplication];
  NSApp.delegate = [[GLFWApplicationDelegate alloc] init];
  [NSApp run];
}
@end
```

This approach sounds promising, but it actually leaves `NSApplication` in a broken state:
`[NSApp isRunning]` is `NO`!
Thus things like `-[NSApplication terminate:]` and
`-[NSApplicationDelegate applicationShouldTerminateAfterLastWindowClosed:]` will not work.

Although Apple says `-[NSApplication run]` is a candidate for overriding, but
no one is doing this, at least to our knowledge.

So it's probably better to just call `NSApplicationMain`.

Aside:
Apple's move towards a combined dynamic linker cache makes it increasingly more
difficult to inspect the internals of various system frameworks.

> The default `terminate:` implementation ends the process directly, never leaving the event loop.

## `MTLView`, `CAMetalLayer`, and `CADisplayLink`

> `CADisplayLink`: A timer object that allows your app to synchronize its drawing to the refresh rate of the display.

## Wayland Event Loop

First connect to a Wayland display:

```c
wl_display* display = wl_display_connect(nullptr);
```

Then get fd:

```c
int fd = wl_display_get_fd(display);
```

The loop should have the form:

```c
while (wl_display_prepare_read_queue(display, queue) != 0) {
  wl_display_dispatch_queue_pending(display, queue);
}
wl_display_flush(display);

ret = poll_fd(fd);

if (has_error(ret)) {
  wl_display_cancel_read(display);
}
else {
  wl_display_read_events(display);
}

wl_display_dispatch_queue_pending(display, queue);
```

Use `epoll` to poll for several fds.

## Win32 Event Loop

Standard and easy:
```c
MSG msg = {};
while (GetMessage(&msg, NULL, 0, 0) > 0) {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
}
```

This waits for events.
Use `PeekMessage` for polling events.

## Failure of Classical Game Loop

The classical loop:

```c
while (!should_quit) {
  // dispatch all available events
  poll_events();
  // update game state
  game_logic();
  render();
}
```

The loop is typically blocked by `vkAcquireNextImageKHR` and it's equivalences.

### Apple has to be different

It seems that Apple suggest us to use `CADisplayLink`.
The exciting news is that it directly provides a `targetTimestamp`, the scheduled
time for the next frame to be displayed.

## Flutter's Main Loop

See [templates](https://github.com/flutter/flutter/tree/1ef8d512829dad0797ecf5259c508a9f2fa49ad4/packages/flutter_tools/templates/app_shared).
