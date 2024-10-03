using System.Runtime.InteropServices;

namespace SpargelEngine.GUI.Window.GLFW;


public class GlfwWindow(nint handle) : Window
{
    public override bool ShouldClose()
    {
        return glfwWindowShouldClose(handle) != 0;
    }

    public override void Destroy()
    {
        glfwDestroyWindow(handle);
    }
    
    
    [DllImport(GlfwWindowSystem.GlfwLibName)]
    private static extern void glfwDestroyWindow(nint window);
    
    [DllImport(GlfwWindowSystem.GlfwLibName)]
    private static extern int glfwWindowShouldClose(nint window);
}
