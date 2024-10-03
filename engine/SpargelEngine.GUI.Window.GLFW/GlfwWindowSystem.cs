using System.Runtime.InteropServices;

namespace SpargelEngine.GUI.Window.GLFW;


public class GlfwWindowSystem : WindowSystem
{
    public const string GlfwLibName = "libglfw.so.3";

    public const int False = 0;
    
    public const int ClientApi = 0x00022001;
    public const int NoApi = 0;
    public const int Resizable = 0x00020003;
    
    public override void Init()
    {
        glfwInit();
    }

    public override void Terminate()
    {
        glfwTerminate();
    }

    public override GlfwWindow Create(uint width, uint height, string title)
    {
        var handle = glfwCreateWindow((int) width, (int) height, title, 0, 0);
        return new GlfwWindow(handle);
    }

    public override void PollEvents()
    {
        glfwPollEvents();
    }

    public static void WindowHint(int hint, int value)
    {
        glfwWindowHint(hint, value);
    }

    public static unsafe byte** GetRequiredInstanceExtensions(uint* count)
    {
        return glfwGetRequiredInstanceExtensions(count);
    }

    
    [DllImport(GlfwLibName)]
    private static extern void glfwInit();
    
    [DllImport(GlfwLibName)]
    private static extern void glfwTerminate();

    [DllImport(GlfwLibName)]
    private static extern nint glfwCreateWindow(int width, int height, string title, nint monitor, nint share);
    
    [DllImport(GlfwLibName)]
    private static extern void glfwPollEvents();

    [DllImport(GlfwLibName)]
    private static extern void glfwWindowHint(int hint, int value);

    [DllImport(GlfwLibName)]
    private static extern unsafe byte** glfwGetRequiredInstanceExtensions(uint* count);
}
