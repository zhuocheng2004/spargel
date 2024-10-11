namespace SpargelEngine.GUI.Window;


public abstract class Window
{
    public delegate void FramebufferSizeCallback(Window window, int width, int height);
    
    public abstract bool ShouldClose();
    
    public abstract int GetWidth();
    
    public abstract int GetHeight();

    public abstract void SetShouldClose(bool value);
    
    public abstract void Destroy();
    
    public abstract KeyStatus GetKey(Key key);

    /**
     * @return: previously set callback
     */
    public abstract FramebufferSizeCallback? SetFramebufferSizeCallback(FramebufferSizeCallback callback);
}
