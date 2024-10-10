namespace SpargelEngine.Graphics.OpenGL;


public abstract partial class OpenGL
{
    /*
     * Types:
     *   bitfield   -> uint
     *   boolean    -> byte
     *   enum       -> uint
     *   sizei      -> uint
     *   sizeiptr   -> nint
     */
    public const byte False = 0;
    public const byte True = 1;
    public const uint UnsignedInt = 0x1405;
    public const uint Float = 0x1406;

    public const uint Triangles = 0x0004;

    public const uint ColorBufferBit = 0x00004000;

    public const uint ArrayBuffer = 0x8892;
    public const uint ElementArrayBuffer = 0x8893;
    public const uint StaticDraw = 0x88E4;
    public const uint FragmentShader = 0x8B30;
    public const uint VertexShader = 0x8B31;
    public const uint CompileStatus = 0x8B81;
    public const uint LinkStatus = 0x8B82;
    
    
    public abstract void AttachShader(uint program, uint shader);
    
    public abstract void BindBuffer(uint target, uint buffer);
    
    public abstract void BindVertexArray(uint array);

    public abstract unsafe void BufferData(uint target, nint size, void* data, uint usage);
    
    public abstract void Clear(uint buf);
    
    public abstract void ClearColor(float r, float g, float b, float a);

    public abstract void CompileShader(uint shader);

    public abstract uint CreateProgram();
    
    public abstract uint CreateShader(uint type);
    
    public abstract unsafe void DeleteBuffers(uint n, uint* buffers);

    public abstract void DeleteProgram(uint program);
    
    public abstract void DeleteShader(uint shader);

    public abstract unsafe void DeleteVertexArrays(uint n, uint* arrays);
    
    public abstract void DrawArrays(uint mode, int first, uint count);
    
    public abstract unsafe void DrawElements(uint mode, uint count, uint type, void* indices);
    
    public abstract void EnableVertexAttribArray(uint index);
    
    public abstract unsafe void GenBuffers(uint n, uint* buffers);
    
    public abstract unsafe void GenVertexArrays(uint n, uint* arrays);

    public abstract unsafe void GetProgramiv(uint program, uint pname, int* @params);
    
    public abstract unsafe void GetProgramInfoLog(uint program, uint bufSize, uint* length, byte* infoLog);
    
    public abstract unsafe void GetShaderInfoLog(uint shader, uint bufSize, uint* length, byte* infoLog);
    
    public abstract unsafe void GetShaderiv(uint shader, uint pname, int* @params);

    public abstract void LinkProgram(uint program);
    
    public abstract unsafe void ShaderSource(uint shader, uint count, byte** @string, int* length);
    
    public abstract void UseProgram(uint program);
    
    public abstract unsafe void VertexAttribPointer(uint index, int size, uint type, byte normalized, uint stride, void* pointer);
    
    public abstract void Viewport(int x, int y, uint w, uint h);
}
