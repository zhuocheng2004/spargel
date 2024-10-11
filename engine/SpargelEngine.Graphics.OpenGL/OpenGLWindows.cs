using System.Runtime.InteropServices;

namespace SpargelEngine.Graphics.OpenGL;


public class OpenGLWindows : OpenGLGeneric
{
    private delegate void AttachShaderDelegate(uint program, uint shader);
    private delegate void BindBufferDelegate(uint target, uint buffer);
    private delegate void BindVertexArrayDelegate(uint array);
    private unsafe delegate void BufferDataDelegate(uint target, IntPtr size, void* data, uint usage);
    private delegate void CompileShaderDelegate(uint shader);
    private delegate uint CreateProgramDelegate();
    private delegate uint CreateShaderDelegate(uint type);
    private unsafe delegate void DeleteBuffersDelegate(uint n, uint* buffers);
    private delegate void DeleteProgramDelegate(uint program);
    private delegate void DeleteShaderDelegate(uint shader);
    private unsafe delegate void DeleteVertexArraysDelegate(uint n, uint* arrays);
    private delegate void EnableVertexAttribArrayDelegate(uint index);
    private unsafe delegate void GenBuffersDelegate(uint n, uint* buffers);
    private unsafe delegate void GenVertexArraysDelegate(uint n, uint* arrays);
    private unsafe delegate void GetProgramInfoLogDelegate(uint program, uint bufSize, uint* length, byte* infoLog);
    private unsafe delegate void GetProgramivDelegate(uint program, uint pname, int* @params);
    private unsafe delegate void GetShaderInfoLogDelegate(uint shader, uint bufSize, uint* length, byte* infoLog);
    private unsafe delegate void GetShaderivDelegate(uint shader, uint pname, int* @params);
    private unsafe delegate int GetUniformLocationDelegate(uint program, byte* name);
    private delegate void LinkProgramDelegate(uint program);
    private unsafe delegate void ShaderSourceDelegate(uint shader, uint count, byte** @string, int* length);
    private unsafe delegate void Uniform4fDelegate(int location, float v0, float v1, float v2, float v3);
    private unsafe delegate void UniformMatrix4fvDelegate(int location, uint count, bool transpose, float* value);
    private delegate void UseProgramDelegate(uint program);
    private unsafe delegate void VertexAttribPointerDelegate(uint index, int size, uint type, byte normalized, uint stride,
        void* pointer);
    
    
    private static TDelegate GetMethod<TDelegate>(string name)
    {
        var ptr = wglGetProcAddress(name);
        if (ptr == nint.Zero) throw new Exception($"cannot find OpenGL method {name}");
        return Marshal.GetDelegateForFunctionPointer<TDelegate>(wglGetProcAddress(name));
    }
    
    [DllImport("opengl32.dll")]
    private static extern nint wglGetProcAddress(string procName);
    
    private static readonly Exception NotInitializedException = new("OpenGL context is not initialized");
    
    private AttachShaderDelegate? _attachShader;
    private BindBufferDelegate? _bindBuffer;
    private BindVertexArrayDelegate? _bindVertexArray;
    private BufferDataDelegate? _bufferData;
    private CompileShaderDelegate? _compileShader;
    private CreateProgramDelegate? _createProgram;
    private CreateShaderDelegate? _createShader;
    private DeleteBuffersDelegate? _deleteBuffers;
    private DeleteProgramDelegate? _deleteProgram;
    private DeleteShaderDelegate? _deleteShader;
    private DeleteVertexArraysDelegate? _deleteVertexArrays;
    private EnableVertexAttribArrayDelegate? _enableVertexAttribArray;
    private GenBuffersDelegate? _genBuffers;
    private GenVertexArraysDelegate? _genVertexArrays;
    private GetProgramInfoLogDelegate? _getProgramInfoLog;
    private GetProgramivDelegate? _getProgramiv;
    private GetShaderInfoLogDelegate? _getShaderInfoLog;
    private GetShaderivDelegate? _getShaderiv;
    private GetUniformLocationDelegate? _getUniformLocation;
    private LinkProgramDelegate? _linkProgram;
    private ShaderSourceDelegate? _shaderSource;
    private Uniform4fDelegate? _uniform4f;
    private UniformMatrix4fvDelegate? _uniformMatrix4fv;
    private UseProgramDelegate? _useProgram;
    private VertexAttribPointerDelegate? _vertexAttribPointer;
    
    
    public override void Initialize() {
        _attachShader = GetMethod<AttachShaderDelegate>("glAttachShader");
        _bindBuffer = GetMethod<BindBufferDelegate>("glBindBuffer");
        _bindVertexArray = GetMethod<BindVertexArrayDelegate>("glBindVertexArray");
        _bufferData = GetMethod<BufferDataDelegate>("glBufferData");
        _compileShader = GetMethod<CompileShaderDelegate>("glCompileShader");
        _createProgram = GetMethod<CreateProgramDelegate>("glCreateProgram");
        _createShader = GetMethod<CreateShaderDelegate>("glCreateShader");
        _deleteBuffers = GetMethod<DeleteBuffersDelegate>("glDeleteBuffers");
        _deleteProgram = GetMethod<DeleteProgramDelegate>("glDeleteProgram");
        _deleteShader = GetMethod<DeleteShaderDelegate>("glDeleteShader");
        _deleteVertexArrays = GetMethod<DeleteVertexArraysDelegate>("glDeleteVertexArrays");
        _enableVertexAttribArray = GetMethod<EnableVertexAttribArrayDelegate>("glEnableVertexAttribArray");
        _genBuffers = GetMethod<GenBuffersDelegate>("glGenBuffers");
        _genVertexArrays = GetMethod<GenVertexArraysDelegate>("glGenVertexArrays");
        _getProgramInfoLog = GetMethod<GetProgramInfoLogDelegate>("glGetProgramInfoLog");
        _getProgramiv = GetMethod<GetProgramivDelegate>("glGetProgramiv");
        _getShaderInfoLog = GetMethod<GetShaderInfoLogDelegate>("glGetShaderInfoLog");
        _getUniformLocation = GetMethod<GetUniformLocationDelegate>("glGetUniformLocation");
        _getShaderiv = GetMethod<GetShaderivDelegate>("glGetShaderiv");
        _linkProgram = GetMethod<LinkProgramDelegate>("glLinkProgram");
        _shaderSource = GetMethod<ShaderSourceDelegate>("glShaderSource");
        _uniform4f = GetMethod<Uniform4fDelegate>("glUniform4f");
        _uniformMatrix4fv = GetMethod<UniformMatrix4fvDelegate>("glUniformMatrix4fv");
        _useProgram = GetMethod<UseProgramDelegate>("glUseProgram");
        _vertexAttribPointer = GetMethod<VertexAttribPointerDelegate>("glVertexAttribPointer");
    }

    public override void Terminate() { }
    

    public override void AttachShader(uint program, uint shader)
    {
        if (_attachShader == null) throw NotInitializedException;
        _attachShader(program, shader);
    }

    public override void BindBuffer(uint program, uint buffer)
    {
        if (_bindBuffer == null) throw NotInitializedException;
        _bindBuffer(program, buffer);
    }

    public override void BindVertexArray(uint array)
    {
        if (_bindVertexArray == null) throw NotInitializedException;
        _bindVertexArray(array);
    }

    public override unsafe void BufferData(uint target, IntPtr size, void* data, uint usage)
    {
        if (_bufferData == null) throw NotInitializedException;
        _bufferData(target, size, data, usage);
    }

    public override void CompileShader(uint shader)
    {
        if (_compileShader == null) throw NotInitializedException;
        _compileShader(shader);
    }

    public override uint CreateProgram()
    {
        if (_createProgram == null) throw NotInitializedException;
        return _createProgram();
    }

    public override uint CreateShader(uint type)
    {
        if (_createShader == null) throw NotInitializedException;
        return _createShader(type);
    }

    public override unsafe void DeleteBuffers(uint n, uint* buffers)
    {
        if (_deleteBuffers == null) throw NotInitializedException;
        _deleteBuffers(n, buffers);
    }

    public override void DeleteProgram(uint program)
    {
        if (_deleteProgram == null) throw NotInitializedException;
        _deleteProgram(program);
    }

    public override void DeleteShader(uint shader)
    {
        if (_deleteShader == null) throw NotInitializedException;
        _deleteShader(shader);
    }

    public override unsafe void DeleteVertexArrays(uint n, uint* arrays)
    {
        if (_deleteVertexArrays == null) throw NotInitializedException;
        _deleteVertexArrays(n, arrays);
    }

    public override void EnableVertexAttribArray(uint index)
    {
        if (_enableVertexAttribArray == null) throw NotInitializedException;
        _enableVertexAttribArray(index);
    }

    public override unsafe void GenBuffers(uint n, uint* buffers)
    {
        if (_genBuffers == null) throw NotInitializedException;
        _genBuffers(n, buffers);
    }

    public override unsafe void GenVertexArrays(uint n, uint* arrays)
    {
        if (_genVertexArrays == null) throw NotInitializedException;
        _genVertexArrays(n, arrays);
    }

    public override unsafe void GetProgramInfoLog(uint program, uint bufSize, uint* length, byte* infoLog)
    {
        if (_getProgramInfoLog == null) throw NotInitializedException;
        _getProgramInfoLog(program, bufSize, length, infoLog);
    }

    public override unsafe void GetProgramiv(uint program, uint pname, int* @params)
    {
        if (_getProgramiv == null) throw NotInitializedException;
        _getProgramiv(program, pname, @params);
    }

    public override unsafe void GetShaderInfoLog(uint shader, uint bufSize, uint* length, byte* infoLog)
    {
        if (_getShaderInfoLog == null) throw NotInitializedException;
        _getShaderInfoLog(shader, bufSize, length, infoLog);
    }

    public override unsafe void GetShaderiv(uint shader, uint pname, int* @params)
    {
        if (_getShaderiv == null) throw NotInitializedException;
        _getShaderiv(shader, pname, @params);
    }

    public override unsafe int GetUniformLocation(uint program, byte* name)
    {
        if (_getUniformLocation == null) throw NotInitializedException;
        return _getUniformLocation(program, name);
    }

    public override void LinkProgram(uint program)
    {
        if (_linkProgram == null) throw NotInitializedException;
        _linkProgram(program);
    }

    public override unsafe void ShaderSource(uint shader, uint count, byte** @string, int* length)
    {
        if (_shaderSource == null) throw NotInitializedException;
        _shaderSource(shader, count, @string, length);
    }

    public override void Uniform4f(int location, float v0, float v1, float v2, float v3)
    {
        if (_uniform4f == null) throw NotInitializedException;
        _uniform4f(location, v0, v1, v2, v3);
    }

    public override unsafe void UniformMatrix4fv(int location, uint count, bool transpose, float* value)
    {
        if (_uniformMatrix4fv == null) throw NotInitializedException;
        _uniformMatrix4fv(location, count, transpose, value);
    }

    public override void UseProgram(uint program)
    {
        if (_useProgram == null) throw NotInitializedException;
        _useProgram(program);
    }

    public override unsafe void VertexAttribPointer(uint index, int size, uint type, byte normalized, uint stride,
        void* pointer)
    {
        if (_vertexAttribPointer == null) throw NotInitializedException;
        _vertexAttribPointer(index, size, type, normalized, stride, pointer);
    }
}
