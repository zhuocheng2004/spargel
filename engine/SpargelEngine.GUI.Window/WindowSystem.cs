namespace SpargelEngine.GUI.Window;


public abstract class WindowSystem<TW> where TW : Window
{
    public abstract void Init();

    public abstract void Terminate();
    
    public abstract TW Create(uint width, uint height, string title);
    
    public abstract void PollEvents();
}
