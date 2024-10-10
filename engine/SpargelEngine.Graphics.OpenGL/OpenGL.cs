using System.Runtime.InteropServices;

namespace SpargelEngine.Graphics.OpenGL;


public abstract partial class OpenGL
{
    public void DeleteBuffer(uint buffer)
    {
        unsafe { DeleteBuffers(1, &buffer); }
    }

    public void DeleteVertexArray(uint array)
    {
        unsafe { DeleteVertexArrays(1, &array); }
    }

    public void DrawElements(uint mode, uint count, uint type)
    {
        unsafe { DrawElements(mode, count, type, (void*) 0); }
    }

    public uint GenBuffer()
    {
        unsafe
        {
            uint buffer;
            GenBuffers(1, &buffer);
            return buffer;
        }
    }

    public uint GenVertexArray()
    {
        unsafe
        {
            uint array;
            GenVertexArrays(1, &array);
            return array;
        }
    }
    
    public string GetProgramInfoLog(uint program, uint bufSize = 1024)
    {
        unsafe
        {
            byte* buf = stackalloc byte[(int) bufSize];
            uint length;
            GetProgramInfoLog(program, bufSize, &length, buf);
            return Marshal.PtrToStringAnsi((nint) buf, (int) length);
        }
    }

    public int GetProgrami(uint program, uint pname)
    {
        unsafe
        {
            int param;
            GetProgramiv(program, pname, &param);
            return param;
        }
    }
    
    public string GetShaderInfoLog(uint shader, uint bufSize = 1024)
    {
        unsafe
        {
            byte* buf = stackalloc byte[(int) bufSize];
            uint length;
            GetShaderInfoLog(shader, bufSize, &length, buf);
            return Marshal.PtrToStringAnsi((nint) buf, (int) length);
        }
    }

    public int GetShaderi(uint shader, uint pname)
    {
        unsafe
        {
            int param;
            GetShaderiv(shader, pname, &param);
            return param;
        }
    }

    public void ShaderSource(uint shader, string source)
    {
        unsafe
        {
            var @string = (byte*) Marshal.StringToHGlobalAnsi(source);
            ShaderSource(shader, 1, &@string, (int*) 0);
        }
    }
    
    public unsafe void VertexAttribPointer(uint index, int size, uint type, bool normalized, uint stride,
        void* pointer)
    {
        VertexAttribPointer(index, size, type, normalized ? True : False, stride, pointer);
    }
}
