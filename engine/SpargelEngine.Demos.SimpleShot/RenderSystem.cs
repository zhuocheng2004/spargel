using System.Runtime.InteropServices;
using SpargelEngine.Core.Ecs;
using SpargelEngine.Graphics.OpenGL;
using SpargelEngine.GUI.Window;
using SpargelEngine.GUI.Window.GLFW;

namespace SpargelEngine.Demos.SimpleShot;


public class RenderSystem : ISystem
{
    private const uint ScreenWidth = 800;
    private const uint ScreenHeight = 600;
    
    private const string VertexShaderSource = 
        """
        #version 330 core
        
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec4 aColor;
        
        out vec4 color;
        
        uniform mat4 viewMatrix;
        uniform mat4 projectionMatrix;
        
        void main()
        {
            gl_Position = projectionMatrix * viewMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
            color = aColor;
        }
        """;

    private const string FragmentShaderSource =
        """
        #version 330 core
        
        in vec4 color;
        
        out vec4 FragColor;
        
        void main()
        {
            FragColor = color;
        }
        """;
    
    private readonly OpenGL _gl = RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? new OpenGLWindows() : new OpenGLGeneric();
    private readonly GlfwWindowSystem _windowSystem = new();
    private GlfwWindow? _window;

    private bool _resized;
    private int _width, _height;

    private uint _program;
    private uint _vao;
    private uint _vbo;
    private uint _ebo;
    
    
    public void OnCreate()
    {
        _windowSystem.Init();
        Glfw.glfwWindowHint(Glfw.ContextVersionMajor, 3);
        Glfw.glfwWindowHint(Glfw.ContextVersionMinor, 3);
        Glfw.glfwWindowHint(Glfw.OpenGLProfile, Glfw.OpenGLCoreProfile);
        
        if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            Glfw.glfwWindowHint(Glfw.OpenGLForwardCompat, 1);
        
        _window = _windowSystem.Create(ScreenWidth, ScreenHeight, "SimpleShot");
        
        _window.MakeContextCurrent();
        
        _window.SetFramebufferSizeCallback(FramebufferSizeCallback);

        _width = _window.GetWidth();
        _height = _window.GetHeight();
        
        _gl.Initialize();
        // build and compile shader program
        var vertexShader = _gl.CreateShader(OpenGL.VertexShader);
        _gl.ShaderSource(vertexShader, VertexShaderSource);
        _gl.CompileShader(vertexShader);
        if (_gl.GetShaderi(vertexShader, OpenGL.CompileStatus) == OpenGL.False)
        {
            Console.WriteLine("ERROR::SHADER::VERTEX::COMPILATION_FAILED");
            Console.WriteLine(_gl.GetShaderInfoLog(vertexShader));
        }
        
        var fragmentShader = _gl.CreateShader(OpenGL.FragmentShader);
        _gl.ShaderSource(fragmentShader, FragmentShaderSource);
        _gl.CompileShader(fragmentShader);
        if (_gl.GetShaderi(fragmentShader, OpenGL.CompileStatus) == OpenGL.False)
        {
            Console.WriteLine("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
            Console.WriteLine(_gl.GetShaderInfoLog(fragmentShader));
        }

        _program = _gl.CreateProgram();
        _gl.AttachShader(_program, vertexShader);
        _gl.AttachShader(_program, fragmentShader);
        _gl.LinkProgram(_program);
        if (_gl.GetProgrami(_program, OpenGL.LinkStatus) == OpenGL.False)
        {
            Console.WriteLine("ERROR::SHADER::PROGRAM::LINKING_FAILED");
            Console.WriteLine(_gl.GetProgramInfoLog(_program));
        }
        _gl.DeleteShader(vertexShader);
        _gl.DeleteShader(fragmentShader);
        
        
        _vao = _gl.GenVertexArray();
        _vbo = _gl.GenBuffer();
        _ebo = _gl.GenBuffer();
        
        _gl.Enable(OpenGL.DepthTest);
        
        _gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        _gl.DepthFunc(OpenGL.Less);
    }

    public void OnDestroy()
    {
        _gl.DeleteVertexArray(_vao);
        _gl.DeleteBuffer(_vbo);
        _gl.DeleteBuffer(_ebo);
        _gl.DeleteProgram(_program);
        
        _gl.Terminate();
        
        _window?.Destroy();
        
        _windowSystem.Terminate();
    }

    public void OnUpdate(World world)
    {
        var entityManager = world.EntityManager;
        
        var timeComponent = entityManager.GetGlobalComponent<TimeComponent>();
        var tickCount = (float) Environment.TickCount / 1000;
        if (timeComponent.FirstFrame)
        {
            timeComponent.FirstFrame = false;
            timeComponent.DeltaTime = 0.0f;
        }
        else
        {
            timeComponent.DeltaTime = tickCount - timeComponent.LastFrameTime;
        }
        timeComponent.LastFrameTime = tickCount;
        entityManager.SetGlobalComponent(timeComponent);

        if (_window?.ShouldClose() ?? true)
        {
            world.SetShouldShutdown(true);
            return;
        }

        if (_resized)
        {
            _resized = false;
            _gl.Viewport(0, 0, (uint) _width, (uint) _height);
        }
        
        ProcessInput(world);

        foreach (var camera in entityManager.GetEntities<CameraComponent>())
        {
            var cameraComponent = entityManager.GetComponent<CameraComponent>(camera);
            var cameraPosition = entityManager.GetComponent<PositionComponent>(camera);
            
            // build buffer
            _gl.BindVertexArray(_vao);
            var vertexBufferData = new List<float>();
            var indexBufferData = new List<uint>();
            var count = 0U; // number of rectangles
            foreach (var entity in entityManager.GetEntities<RectangleComponent>())
            {
                if (!entityManager.HasComponent<PositionComponent>(entity)) continue;
                
                var position = entityManager.GetComponent<PositionComponent>(entity);
                var rectangle = entityManager.GetComponent<RectangleComponent>(entity);
                var color = entityManager.GetComponent<ColorComponent>(entity);

                vertexBufferData.Add(position.X + rectangle.X0);
                vertexBufferData.Add(position.Y + rectangle.Y0);
                vertexBufferData.Add(position.Z);
                vertexBufferData.Add(0);
                vertexBufferData.Add(color.R);
                vertexBufferData.Add(color.G);
                vertexBufferData.Add(color.B);
                vertexBufferData.Add(color.A);
                
                vertexBufferData.Add(position.X + rectangle.X1);
                vertexBufferData.Add(position.Y + rectangle.Y0);
                vertexBufferData.Add(position.Z);
                vertexBufferData.Add(0);
                vertexBufferData.Add(color.R);
                vertexBufferData.Add(color.G);
                vertexBufferData.Add(color.B);
                vertexBufferData.Add(color.A);
                
                vertexBufferData.Add(position.X + rectangle.X1);
                vertexBufferData.Add(position.Y + rectangle.Y1);
                vertexBufferData.Add(position.Z);
                vertexBufferData.Add(0);
                vertexBufferData.Add(color.R);
                vertexBufferData.Add(color.G);
                vertexBufferData.Add(color.B);
                vertexBufferData.Add(color.A);
                
                vertexBufferData.Add(position.X + rectangle.X0);
                vertexBufferData.Add(position.Y + rectangle.Y1);
                vertexBufferData.Add(position.Z);
                vertexBufferData.Add(0);
                vertexBufferData.Add(color.R);
                vertexBufferData.Add(color.G);
                vertexBufferData.Add(color.B);
                vertexBufferData.Add(color.A);

                var offset = count * 4;
                indexBufferData.Add(offset + 0);
                indexBufferData.Add(offset + 1);
                indexBufferData.Add(offset + 2);
                indexBufferData.Add(offset + 0);
                indexBufferData.Add(offset + 2);
                indexBufferData.Add(offset + 3);
                
                count++;
            }

            unsafe
            {
                _gl.BindBuffer(OpenGL.ArrayBuffer, _vbo);
                fixed (void* ptr = vertexBufferData.ToArray())
                {
                    _gl.BufferData(OpenGL.ArrayBuffer, vertexBufferData.Count * sizeof(float), ptr, OpenGL.StaticDraw);
                }
                
                _gl.BindBuffer(OpenGL.ElementArrayBuffer, _ebo);
                fixed (void* ptr = indexBufferData.ToArray())
                {
                    _gl.BufferData(OpenGL.ElementArrayBuffer, indexBufferData.Count * sizeof(uint), ptr, OpenGL.StaticDraw);
                }
                
                _gl.VertexAttribPointer(0, 3, OpenGL.Float, false, 8 * sizeof(float), (void*) 0);
                _gl.EnableVertexAttribArray(0);
                _gl.VertexAttribPointer(1, 4, OpenGL.Float, false, 8 * sizeof(float), (void*) (4 * sizeof(float)));
                _gl.EnableVertexAttribArray(1);
            }
            _gl.BindVertexArray(0);

            float w, h;
            if (cameraComponent.MainAxis == CameraComponent.Axis.Horizontal)
            {
                w = cameraComponent.Size;
                h = w * _height / _width;
            }
            else
            {
                h = cameraComponent.Size;
                w = h * _width / _height;
            }
            float far = cameraComponent.Far, near = cameraComponent.Near;
            
            // prepare MVP matrix
            float[] viewMatrix = [
                1.0f, 0.0f, 0.0f, -cameraPosition.X,
                0.0f, 1.0f, 0.0f, -cameraPosition.Y,
                0.0f, 0.0f, 1.0f, -cameraPosition.Z,
                0.0f, 0.0f, 0.0f, 1.0f
            ];
            float[] projectionMatrix = [
                2 / w, 0.0f, 0.0f, 0.0f,
                0.0f, 2 / h, 0.0f, 0.0f,
                0.0f, 0.0f, 1 / (far - near), -near / (far - near),
                0.0f, 0.0f, 0.0f, 1.0f
            ];
            var locationV = _gl.GetUniformLocation(_program, "viewMatrix");
            var locationP = _gl.GetUniformLocation(_program, "projectionMatrix");
            
            _gl.Clear(OpenGL.ColorBufferBit | OpenGL.DepthBufferBit);
            
            _gl.UseProgram(_program);
            _gl.BindVertexArray(_vao);
            
            _gl.UniformMatrix4(locationV, viewMatrix, true);
            _gl.UniformMatrix4(locationP, projectionMatrix, true);
            
            _gl.DrawElements(OpenGL.Triangles, count * 6, OpenGL.UnsignedInt);
            
        }
        
        _window.SwapBuffers();
        _windowSystem.PollEvents();
    }
    
    private void ProcessInput(World world)
    {
        if (_window == null) return;

        if (_window.GetKey(Key.Escape) == KeyStatus.Press)
        {
            _window.SetShouldClose(true);
            return;
        }

        var verticalInput = 0;
        if (_window.GetKey(Key.W) == KeyStatus.Press) verticalInput += 1;
        if (_window.GetKey(Key.S) == KeyStatus.Press) verticalInput -= 1;
        var input = world.EntityManager.GetGlobalComponent<InputComponent>();
        var oldVerticalInput = input.VerticalInput;
        input.VerticalInput = verticalInput + 0.9f * (oldVerticalInput - verticalInput);

        if (input.SpaceCooldown == 0)
        {
            if (_window.GetKey(Key.Space) == KeyStatus.Press)
            {
                input.SpaceCount++;
                input.SpaceCooldown = 10;
            }
        }
        else
        {
            input.SpaceCooldown--;
        }
        
        world.EntityManager.SetGlobalComponent(input);
    }

    private void FramebufferSizeCallback(Window window, int width, int height)
    {
        _width = width;
        _height = height;
        _resized = true;
    }
}
