using System.Runtime.InteropServices;

namespace SpargelEngine.Demo.MacWindow
{
  public class Program
  {
    static void Main(string[] args)
    {
      var cls_NSApplication = objc_getClass("NSApplication");
      var sel_sharedApplication = sel_getUid("sharedApplication");
      // [NSApplication sharedApplication]
      var app =
        objc_msgSend_IntPtr_IntPtr_IntPtr(
          cls_NSApplication, sel_sharedApplication);

      var sel_setActivationPolicy = sel_getUid("setActivationPolicy:");
      // [NSApp setActicationPolicy:NSApplicationActivationPolicyRegular]
      objc_msgSend_IntPtr_IntPtr_Int64_Boolean(
          app, sel_setActivationPolicy, 0);

      var cls_NSWindow = objc_getClass("NSWindow");
      var sel_alloc = sel_getUid("alloc");
      var sel_looong =
        sel_getUid("initWithContentRect:styleMask:backing:defer:");
      // id window =
      //   [[NSWindow alloc]
      //      initWithContentRect:NSMakeRect(0, 0, 120, 120),
      //      styleMask:NSTitledWindowMask
      //      backing:NSBackingStoreBuffered
      //      defer:NO];
      var window =
        objc_msgSend_IntPtr_IntPtr_IntPtr(
          cls_NSWindow, sel_alloc);
      window =
        objc_msgSend_IntPtr_IntPtr_CGRect_Int64_Int64_Boolean_IntPtr(
          window, sel_looong,
          new CGRect(0, 0, 300, 300),
          (1 << 0) | (1 << 1) | (1 << 3),
          2,
          false);

      var cls_NSString = objc_getClass("NSString");
      var sel_stringWithUTF8String = sel_getUid("stringWithUTF8String:");
      var title =
        objc_msgSend_IntPtr_IntPtr_string_IntPtr(
          cls_NSString, sel_stringWithUTF8String,
          "Spargel Engine");
      
      var sel_setTitle = sel_getUid("setTitle:");
      // [window setTitle: ...];
      objc_msgSend_IntPtr_IntPtr_IntPtr_void(
          window, sel_setTitle,
          title);

      var sel_makeKeyAndOrderFront = sel_getUid("makeKeyAndOrderFront:");
      // [window makeKeyAndOrderFront:nil];
      objc_msgSend_IntPtr_IntPtr_IntPtr_void(window, sel_makeKeyAndOrderFront, 0);

      var sel_run =  sel_getUid("run");
      objc_msgSend_IntPtr_IntPtr_void(app, sel_run);
    }

    public record struct CGRect(Double x1, Double y1, Double x2, Double y2);

    private const string FoundationFramework = "/System/Library/Frameworks/Foundation.framework/Foundation";
    private const string AppKitFramework = "/System/Library/Frameworks/AppKit.framework/AppKit";

    [DllImport(AppKitFramework, CharSet = CharSet.Ansi)]
    public static extern IntPtr objc_getClass(string name);

    [DllImport(FoundationFramework, CharSet = CharSet.Ansi)]
    public static extern IntPtr sel_getUid(string name);

    [DllImport(FoundationFramework, EntryPoint="objc_msgSend")]
    public static extern IntPtr objc_msgSend_retIntPtr(IntPtr target, IntPtr selector);

    [DllImport(FoundationFramework, EntryPoint = "objc_msgSend")]
    public static extern IntPtr objc_msgSend_IntPtr_IntPtr_IntPtr(IntPtr target, IntPtr selector);

    [DllImport(FoundationFramework, EntryPoint = "objc_msgSend")]
    public static extern Boolean objc_msgSend_IntPtr_IntPtr_Int64_Boolean(IntPtr target, IntPtr selector, Int64 arg);

    [DllImport(FoundationFramework, EntryPoint = "objc_msgSend")]
    public static extern IntPtr objc_msgSend_IntPtr_IntPtr_CGRect_Int64_Int64_Boolean_IntPtr(IntPtr target, IntPtr selector, CGRect rect, Int64 style, Int64 type, Boolean defer);

    [DllImport(FoundationFramework, EntryPoint = "objc_msgSend")]
    public static extern void objc_msgSend_IntPtr_IntPtr_void(IntPtr target, IntPtr selector);

    [DllImport(FoundationFramework, EntryPoint = "objc_msgSend")]
    public static extern void objc_msgSend_IntPtr_IntPtr_IntPtr_void(IntPtr target, IntPtr selector, IntPtr ptr);

    [DllImport(FoundationFramework, EntryPoint = "objc_msgSend", CharSet = CharSet.Ansi)]
    public static extern IntPtr objc_msgSend_IntPtr_IntPtr_string_IntPtr(IntPtr target, IntPtr selector, string str);
  }
}
