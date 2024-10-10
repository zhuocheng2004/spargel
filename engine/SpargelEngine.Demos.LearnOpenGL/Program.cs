using SpargelEngine.Graphics.OpenGL;
using SpargelEngine.Graphics.OpenGL.Linux;
using SpargelEngine.GUI.Window;
using SpargelEngine.GUI.Window.GLFW;

namespace SpargelEngine.Demos.LearnOpenGL;


public static class Program
{
    private const uint ScreenWidth = 800;
    private const uint ScreenHeight = 600;

    private static readonly OpenGL Gl = new OpenGLLinux();
    private static readonly GlfwWindowSystem WindowSystem = new();
    private static GlfwWindow? _window;
    
    private static bool resized = false;
    private static int _width = 0, _height = 0;
    
    public static void Main()
    {
        WindowSystem.Init();
        Glfw.glfwWindowHint(Glfw.ContextVersionMajor, 3);
        Glfw.glfwWindowHint(Glfw.ContextVersionMinor, 3);
        Glfw.glfwWindowHint(Glfw.OpenGLProfile, Glfw.OpenGLCoreProfile);
        
        _window = WindowSystem.Create(ScreenWidth, ScreenHeight, "LearnOpenGL");
        
        _window.MakeContextCurrent();
        
        _window.SetFramebufferSizeCallback(FramebufferSizeCallback);

        while (!_window.ShouldClose())
        {
            ProcessInput();
            
            if (resized)
            {
                resized = false;
                Gl.Viewport(0, 0, (uint) _width, (uint) _height);
            }
            
            Gl.ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            Gl.Clear(OpenGL.ColorBufferBit);
            
            _window.SwapBuffers();
            WindowSystem.PollEvents();
        }
        
        _window.Destroy();
        WindowSystem.Terminate();
    }

    private static void ProcessInput()
    {
        if (_window == null) return;

        if (_window.GetKey(Key.Escape) == KeyStatus.Press)
            _window.SetShouldClose(true);
    }

    private static void FramebufferSizeCallback(Window window, int width, int height)
    {
        _width = width;
        _height = height;
        resized = true;
    }
}
