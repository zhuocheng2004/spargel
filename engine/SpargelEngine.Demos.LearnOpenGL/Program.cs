using SpargelEngine.Graphics.OpenGL;
using SpargelEngine.Graphics.OpenGL.Linux;
using SpargelEngine.GUI.Window;
using SpargelEngine.GUI.Window.GLFW;

namespace SpargelEngine.Demos.LearnOpenGL;


public static class Program
{
    private const uint ScreenWidth = 800;
    private const uint ScreenHeight = 600;

    private const string VertexShaderSource = 
        """
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        out vec3 vertexColor;
        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
            vertexColor = aColor;
        }
        """;

    private const string FragmentShaderSource =
        """
        #version 330 core
        out vec4 FragColor;
        in vec3 vertexColor;
        void main()
        {
            FragColor = vec4(vertexColor, 1.0);
        }
        """;

    private static readonly OpenGL Gl = new OpenGLLinux();
    private static readonly GlfwWindowSystem WindowSystem = new();
    private static GlfwWindow? _window;
    
    private static bool _resized;
    private static int _width, _height;
    
    public static void Main()
    {
        WindowSystem.Init();
        Glfw.glfwWindowHint(Glfw.ContextVersionMajor, 3);
        Glfw.glfwWindowHint(Glfw.ContextVersionMinor, 3);
        Glfw.glfwWindowHint(Glfw.OpenGLProfile, Glfw.OpenGLCoreProfile);
        
        _window = WindowSystem.Create(ScreenWidth, ScreenHeight, "LearnOpenGL");
        
        _window.MakeContextCurrent();
        
        _window.SetFramebufferSizeCallback(FramebufferSizeCallback);
        
        
        // build and compile shader program
        var vertexShader = Gl.CreateShader(OpenGL.VertexShader);
        Gl.ShaderSource(vertexShader, VertexShaderSource);
        Gl.CompileShader(vertexShader);
        if (Gl.GetShaderi(vertexShader, OpenGL.CompileStatus) == OpenGL.False)
        {
            Console.WriteLine("ERROR::SHADER::VERTEX::COMPILATION_FAILED");
            Console.WriteLine(Gl.GetShaderInfoLog(vertexShader));
        }
        
        var fragmentShader = Gl.CreateShader(OpenGL.FragmentShader);
        Gl.ShaderSource(fragmentShader, FragmentShaderSource);
        Gl.CompileShader(fragmentShader);
        if (Gl.GetShaderi(fragmentShader, OpenGL.CompileStatus) == OpenGL.False)
        {
            Console.WriteLine("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
            Console.WriteLine(Gl.GetShaderInfoLog(fragmentShader));
        }

        var program = Gl.CreateProgram();
        Gl.AttachShader(program, vertexShader);
        Gl.AttachShader(program, fragmentShader);
        Gl.LinkProgram(program);
        if (Gl.GetProgrami(program, OpenGL.LinkStatus) == OpenGL.False)
        {
            Console.WriteLine("ERROR::SHADER::PROGRAM::LINKING_FAILED");
            Console.WriteLine(Gl.GetProgramInfoLog(program));
        }
        Gl.DeleteShader(vertexShader);
        Gl.DeleteShader(fragmentShader);

        float[] vertices = [
             0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,     // top right
            -0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,     // bottom left
             0.0f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f      // bottom left
        ];

        var vao = Gl.GenVertexArray();
        var vbo = Gl.GenBuffer();
        Gl.BindVertexArray(vao);
        
        unsafe
        {
            Gl.BindBuffer(OpenGL.ArrayBuffer, vbo);
            fixed (void* ptr = vertices) 
            {
                Gl.BufferData(OpenGL.ArrayBuffer, vertices.Length * sizeof(float), ptr, OpenGL.StaticDraw);
            }
            Gl.VertexAttribPointer(0, 3, OpenGL.Float, false, 6 * sizeof(float), (void*) 0);
            Gl.EnableVertexAttribArray(0);
            Gl.VertexAttribPointer(1, 3, OpenGL.Float, false, 6 * sizeof(float), (void*) (3 * sizeof(float)));
            Gl.EnableVertexAttribArray(1);
        }
        
        Gl.BindBuffer(OpenGL.ArrayBuffer, 0);
        Gl.BindVertexArray(0);

        while (!_window.ShouldClose())
        {
            if (_resized)
            {
                _resized = false;
                Gl.Viewport(0, 0, (uint) _width, (uint) _height);
            }
            
            ProcessInput();
            
            Gl.ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            Gl.Clear(OpenGL.ColorBufferBit);
            
            Gl.UseProgram(program);
            Gl.BindVertexArray(vao);
            Gl.DrawArrays(OpenGL.Triangles, 0, 3);
            
            _window.SwapBuffers();
            WindowSystem.PollEvents();
        }
        
        Gl.DeleteVertexArray(vao);
        Gl.DeleteBuffer(vbo);
        Gl.DeleteProgram(program);
        
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
        _resized = true;
    }
}
