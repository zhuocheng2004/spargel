using System.Runtime.InteropServices;

namespace SpargelEngine.GUI.Window.GLFW;


public class GlfwWindowSystem : WindowSystem
{
    public const string GlfwLibName = "libglfw.so.3";
    
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

    
    [DllImport(GlfwLibName)]
    private static extern void glfwInit();
    
    [DllImport(GlfwLibName)]
    private static extern void glfwTerminate();

    [DllImport(GlfwLibName)]
    private static extern nint glfwCreateWindow(int width, int height, string title, nint monitor, nint share);
    
    [DllImport(GlfwLibName)]
    private static extern void glfwPollEvents();
}
