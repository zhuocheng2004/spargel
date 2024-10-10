namespace SpargelEngine.Graphics.OpenGL;


public abstract partial class OpenGL
{
    /*
     * Types:
     *   bitfield   -> uint
     *   sizei      -> uint
     */

    public const uint ColorBufferBit = 0x00004000;
    
    
    public abstract void Clear(uint buf);
    
    public abstract void ClearColor(float r, float g, float b, float a);
    
    public abstract void Viewport(int x, int y, uint w, uint h);
}
