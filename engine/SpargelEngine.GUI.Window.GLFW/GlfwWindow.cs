namespace SpargelEngine.GUI.Window.GLFW;


public class GlfwWindow(nint handle) : Window
{
    private FramebufferSizeCallback? _framebufferSizeCallback;

    public override bool ShouldClose()
    {
        return Glfw.glfwWindowShouldClose(handle) != 0;
    }

    public override void SetShouldClose(bool value)
    {
        Glfw.glfwSetWindowShouldClose(handle, value ? 1 : 0);
    }

    public override void Destroy()
    {
        Glfw.glfwDestroyWindow(handle);
    }

    public override KeyStatus GetKey(Key key)
    {
        var glfwKeyStatus = Glfw.glfwGetKey(handle, GlfwKeyMap.GetGlfwKey(key));
        return glfwKeyStatus switch
        {
            Glfw.Release => KeyStatus.Release,
            Glfw.Press => KeyStatus.Press,
            Glfw.Repeat => KeyStatus.Repeat,
            _ => throw new Exception($"unknown GLFW key status: {glfwKeyStatus}")
        };
    }

    public override FramebufferSizeCallback? SetFramebufferSizeCallback(FramebufferSizeCallback callback)
    {
        var oldCallback = _framebufferSizeCallback;
        _framebufferSizeCallback = callback;
        Glfw.glfwSetFramebufferSizeCallback(handle, (_, width, height) => _framebufferSizeCallback(this, width, height));
        return oldCallback;
    }

    public void MakeContextCurrent()
    {
        Glfw.glfwMakeContextCurrent(handle);
    }

    public void SwapBuffers()
    {
        Glfw.glfwSwapBuffers(handle);
    }
}
