namespace SpargelEngine.GUI.Window.GLFW;


public class GlfwWindowSystem : WindowSystem<GlfwWindow>
{
    public override void Init()
    {
        Glfw.glfwInit();
    }

    public override void Terminate()
    {
        Glfw.glfwTerminate();
    }

    public override GlfwWindow Create(uint width, uint height, string title)
    {
        var handle = Glfw.glfwCreateWindow((int) width, (int) height, title, 0, 0);
        if (handle != 0)
        {
            var window = new GlfwWindow(handle);
            window.Initialize();
            return window;
        }
        
        Terminate();
        throw new Exception("failed to create GLFW window");
    }

    public override void PollEvents()
    {
        Glfw.glfwPollEvents();
    }
}
