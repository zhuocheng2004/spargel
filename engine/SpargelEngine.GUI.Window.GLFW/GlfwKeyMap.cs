namespace SpargelEngine.GUI.Window.GLFW;


public static class GlfwKeyMap
{
    private static readonly Dictionary<Key, int> KeyMap = new()
    {
        { Key.Space, Glfw.KeySpace },
        { Key.A, Glfw.KeyA },
        { Key.D, Glfw.KeyD },
        { Key.S, Glfw.KeyS },
        { Key.W, Glfw.KeyW },
        { Key.Escape, Glfw.KeyEscape }
    };

    public static int GetGlfwKey(Key key)
    {
        return KeyMap[key];
    }
}
