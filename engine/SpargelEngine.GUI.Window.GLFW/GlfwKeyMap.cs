namespace SpargelEngine.GUI.Window.GLFW;


public static class GlfwKeyMap
{
    private static readonly Dictionary<Key, int> KeyMap = new()
    {
        { Key.Escape, Glfw.KeyEscape }
    };

    public static int GetGlfwKey(Key key)
    {
        return KeyMap[key];
    }
}
