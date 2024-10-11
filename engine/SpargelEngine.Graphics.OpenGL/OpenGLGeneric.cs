using System.Reflection;
using System.Runtime.InteropServices;

namespace SpargelEngine.Graphics.OpenGL;


public class OpenGLGeneric : OpenGL
{
    private const string GlLibName = "opengl";
    
    static OpenGLGeneric()
    {
        NativeLibrary.SetDllImportResolver(Assembly.GetExecutingAssembly(), DllImportResolver);
    }

    private static IntPtr DllImportResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
    {
        if (libraryName != "opengl") return IntPtr.Zero;
        
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            return NativeLibrary.Load("opengl32.dll");
        if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            return NativeLibrary.Load("OpenGL.framework");  // TODO
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            return NativeLibrary.Load("libGL.so.1");

        return IntPtr.Zero;
    }
    

    public override void Initialize() { }

    public override void Terminate() { }

    public override void AttachShader(uint program, uint shader)
    {
        glAttachShader(program, shader);
    }

    public override void BindBuffer(uint target, uint buffer)
    {
        glBindBuffer(target, buffer);
    }

    public override void BindVertexArray(uint array)
    {
        glBindVertexArray(array);
    }

    public override unsafe void BufferData(uint target, IntPtr size, void* data, uint usage)
    {
        glBufferData(target, size, data, usage);
    }

    public override void Clear(uint buf)
    {
        glClear(buf);
    }

    public override void ClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    public override void CompileShader(uint shader)
    {
        glCompileShader(shader);
    }

    public override uint CreateProgram()
    {
        return glCreateProgram();
    }

    public override uint CreateShader(uint type)
    {
        return glCreateShader(type);
    }

    public override unsafe void DeleteBuffers(uint n, uint* buffers)
    {
        glDeleteBuffers(n, buffers);
    }

    public override void DeleteProgram(uint program)
    {
        glDeleteProgram(program);
    }

    public override void DeleteShader(uint shader)
    {
        glDeleteShader(shader);
    }

    public override unsafe void DeleteVertexArrays(uint n, uint* arrays)
    {
        glDeleteVertexArrays(n, arrays);
    }

    public override void DepthFunc(uint func)
    {
        glDepthFunc(func);
    }

    public override void DrawArrays(uint mode, int first, uint count)
    {
        glDrawArrays(mode, first, count);
    }

    public override unsafe void DrawElements(uint mode, uint count, uint type, void* indices)
    {
        glDrawElements(mode, count, type, indices);
    }

    public override void Enable(uint target)
    {
        glEnable(target);
    }

    public override void EnableVertexAttribArray(uint index)
    {
        glEnableVertexAttribArray(index);
    }

    public override unsafe void GenBuffers(uint n, uint* buffers)
    {
        glGenBuffers(n, buffers);
    }

    public override unsafe void GenVertexArrays(uint n, uint* arrays)
    {
        glGenVertexArrays(n, arrays);
    }
    
    public override unsafe void GetProgramInfoLog(uint program, uint bufSize, uint* length, byte* infoLog)
    {
        glGetProgramInfoLog(program, bufSize, length, infoLog);
    }

    public override unsafe void GetProgramiv(uint program, uint pname, int* @params)
    {
        glGetProgramiv(program, pname, @params);
    }

    public override unsafe void GetShaderInfoLog(uint shader, uint bufSize, uint* length, byte* infoLog)
    {
        glGetShaderInfoLog(shader, bufSize, length, infoLog);
    }
    
    public override unsafe void GetShaderiv(uint shader, uint pname, int* @params)
    {
        glGetShaderiv(shader, pname, @params);
    }

    public override unsafe int GetUniformLocation(uint program, byte* name)
    {
        return glGetUniformLocation(program, name);
    }

    public override void LinkProgram(uint program)
    {
        glLinkProgram(program);
    }

    public override unsafe void ShaderSource(uint shader, uint count, byte** @string, int* length)
    {
        glShaderSource(shader, count, @string, length);
    }

    public override void Uniform4f(int location, float v0, float v1, float v2, float v3)
    {
        glUniform4f(location, v0, v1, v2, v3);
    }

    public override unsafe void UniformMatrix4fv(int location, uint count, bool transpose, float* value)
    {
        glUniformMatrix4fv(location, count, transpose, value);
    }

    public override void UseProgram(uint program)
    {
        glUseProgram(program);
    }

    public override unsafe void VertexAttribPointer(uint index, int size, uint type, byte normalized, uint stride, void* pointer)
    {
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    }

    public override void Viewport(int x, int y, uint w, uint h)
    {
        glViewport(x, y, w, h);
    }

    
    [DllImport(GlLibName)]
    private static extern void glAttachShader(uint program, uint shader);
    
    [DllImport(GlLibName)]
    private static extern void glBindBuffer(uint target, uint buffer);
    
    [DllImport(GlLibName)]
    private static extern void glBindVertexArray(uint array);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glBufferData(uint target, nint size, void* data, uint usage);

    [DllImport(GlLibName)]
    private static extern void glClear(uint buf);
    
    [DllImport(GlLibName)]
    private static extern void glClearColor(float r, float g, float b, float a);

    [DllImport(GlLibName)]
    private static extern void glCompileShader(uint shader);
    
    [DllImport(GlLibName)]
    private static extern uint glCreateProgram();
    
    [DllImport(GlLibName)]
    private static extern uint glCreateShader(uint type);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glDeleteBuffers(uint n, uint* buffers);
    
    [DllImport(GlLibName)]
    private static extern void glDeleteProgram(uint program);
    
    [DllImport(GlLibName)]
    private static extern void glDeleteShader(uint shader);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glDeleteVertexArrays(uint n, uint* arrays);

    [DllImport(GlLibName)]
    private static extern void glDepthFunc(uint func);
    
    [DllImport(GlLibName)]
    private static extern void glDrawArrays(uint mode, int first, uint count);

    [DllImport(GlLibName)]
    private static extern unsafe void glDrawElements(uint mode, uint count, uint type, void* indices);

    [DllImport(GlLibName)]
    private static extern void glEnable(uint target);
    
    [DllImport(GlLibName)]
    private static extern void glEnableVertexAttribArray(uint index);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glGenBuffers(uint n, uint* buffers);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glGenVertexArrays(uint n, uint* arrays);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glGetProgramInfoLog(uint program, uint bufSize, uint* length, byte* infoLog);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glGetProgramiv(uint program, uint pname, int* @params);

    [DllImport(GlLibName)]
    private static extern unsafe void glGetShaderInfoLog(uint shader, uint bufSize, uint* length, byte* infoLog);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glGetShaderiv(uint shader, uint pname, int* @params);
    
    [DllImport(GlLibName)]
    private static extern unsafe int glGetUniformLocation(uint program, byte* name);
    
    [DllImport(GlLibName)]
    private static extern void glLinkProgram(uint program);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glShaderSource(uint shader, uint count, byte** @string, int* length);
    
    [DllImport(GlLibName)]
    private static extern void glUniform4f(int location, float v0, float v1, float v2, float v3);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glUniformMatrix4fv(int location, uint count, bool transpose, float* value);
    
    [DllImport(GlLibName)]
    private static extern void glUseProgram(uint program);
    
    [DllImport(GlLibName)]
    private static extern unsafe void glVertexAttribPointer(uint index, int size, uint type, byte normalized, uint stride, void* pointer);
    
    [DllImport(GlLibName)]
    private static extern void glViewport(int x, int y, uint w, uint h);
}
