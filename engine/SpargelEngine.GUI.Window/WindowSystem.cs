namespace SpargelEngine.GUI.Window;


public abstract class WindowSystem
{
    public abstract void Init();

    public abstract void Terminate();
    
    public abstract Window Create(uint width, uint height, string title);
    
    public abstract void PollEvents();
}
