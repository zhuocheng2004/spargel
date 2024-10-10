using System.Runtime.InteropServices;

namespace SpargelEngine.Graphics.OpenGL.Linux;


public class OpenGLLinux : OpenGL
{
    private const string GlLibName = "libGL.so.1";
    
    public override void Clear(uint buf)
    {
        glClear(buf);
    }

    public override void ClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    public override void Viewport(int x, int y, uint w, uint h)
    {
        glViewport(x, y, w, h);
    }


    [DllImport(GlLibName)]
    private static extern void glClear(uint buf);
    
    [DllImport(GlLibName)]
    private static extern void glClearColor(float r, float g, float b, float a);

    [DllImport(GlLibName)]
    private static extern void glViewport(int x, int y, uint w, uint h);
}
