using System.Reflection;
using System.Runtime.InteropServices;

namespace SpargelEngine.GUI.Window.GLFW;


public static class Glfw
{
    public delegate void GlfwFramebufferSizeCallback(nint window, int width, int height);
    
    private const string GlfwLibName = "glfw";

    public const int False = 0;
    
    public const int ClientApi = 0x00022001;
    public const int ContextVersionMajor = 0x00022002;
    public const int ContextVersionMinor = 0x00022003;
    public const int OpenGLForwardCompat = 0x00022006;
    public const int OpenGLProfile = 0x00022008;
    public const int NoApi = 0;
    public const int Resizable = 0x00020003;
    public const int OpenGLCoreProfile = 0x00032001;

    public const int Release = 0;
    public const int Press = 1;
    public const int Repeat = 2;

    public const int KeySpace = 32;
    public const int Key0 = 48;
    public const int Key1 = 49;
    public const int Key2 = 50;
    public const int Key3 = 51;
    public const int Key4 = 52;
    public const int Key5 = 53;
    public const int Key6 = 54;
    public const int Key7 = 55;
    public const int Key8 = 56;
    public const int Key9 = 57;
    public const int KeyA = 65;
    public const int KeyB = 66;
    public const int KeyC = 67;
    public const int KeyD = 68;
    public const int KeyE = 69;
    public const int KeyF = 70;
    public const int KeyG = 71;
    public const int KeyH = 72;
    public const int KeyI = 73;
    public const int KeyJ = 74;
    public const int KeyK = 75;
    public const int KeyL = 76;
    public const int KeyM = 77;
    public const int KeyN = 78;
    public const int KeyO = 79;
    public const int KeyP = 80;
    public const int KeyQ = 81;
    public const int KeyR = 82;
    public const int KeyS = 83;
    public const int KeyT = 84;
    public const int KeyU = 85;
    public const int KeyV = 86;
    public const int KeyW = 87;
    public const int KeyX = 88;
    public const int KeyY = 89;
    public const int KeyZ = 90;
    public const int KeyEscape = 256;
    public const int KeyEnter = 257;

    static Glfw()
    {
        NativeLibrary.SetDllImportResolver(Assembly.GetExecutingAssembly(), DllImportResolver);
    }

    private static IntPtr DllImportResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
    {
        if (libraryName != "glfw") return IntPtr.Zero;
        
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            return NativeLibrary.Load("glfw3.dll");
        if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            return NativeLibrary.Load("libglfw.3.dylib");
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            return NativeLibrary.Load("libglfw.so.3");
            
        return IntPtr.Zero;
    }
    
    [DllImport(GlfwLibName)]
    public static extern nint glfwCreateWindow(int width, int height, string title, nint monitor, nint share);

    [DllImport(GlfwLibName)]
    public static extern void glfwDestroyWindow(nint window);

    [DllImport(GlfwLibName)]
    public static extern int glfwGetKey(nint window, int key);

    [DllImport(GlfwLibName)]
    public static extern unsafe byte** glfwGetRequiredInstanceExtensions(uint* count);

    [DllImport(GlfwLibName)]
    public static extern void glfwInit();

    [DllImport(GlfwLibName)]
    public static extern unsafe void glfwGetWindowSize(nint window, int* width, int* height);
    
    [DllImport(GlfwLibName)]
    public static extern void glfwMakeContextCurrent(nint window);
    
    [DllImport(GlfwLibName)]
    public static extern void glfwPollEvents();
    
    [DllImport(GlfwLibName)]
    public static extern nint glfwSetFramebufferSizeCallback(nint window, GlfwFramebufferSizeCallback callback);

    [DllImport(GlfwLibName)]
    public static extern void glfwSetWindowShouldClose(nint window, int value);
    
    [DllImport(GlfwLibName)]
    public static extern void glfwSwapBuffers(nint window);
    
    [DllImport(GlfwLibName)]
    public static extern void glfwTerminate();

    [DllImport(GlfwLibName)]
    public static extern void glfwWindowHint(int hint, int value);
    
    [DllImport(GlfwLibName)]
    public static extern int glfwWindowShouldClose(nint window);
}
