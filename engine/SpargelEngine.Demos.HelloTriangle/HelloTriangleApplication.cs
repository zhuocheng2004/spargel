using System.Runtime.InteropServices;
using SpargelEngine.Graphics.Vulkan;
using SpargelEngine.Graphics.Vulkan.Linux;
using SpargelEngine.GUI.Window;
using SpargelEngine.GUI.Window.GLFW;

namespace SpargelEngine.Demos.HelloTriangle;


public class HelloTriangleApplication
{
    private const uint Width = 800;
    private const uint Height = 600;
    
    private readonly WindowSystem _windowSystem = new GlfwWindowSystem();
    private readonly Vulkan _vulkan = new VulkanLinux();
    
    private Window? _window;

    public void Run()
    {
        InitWindow();
        InitVulkan();
        MainLoop();
        Cleanup();
    }

    private void InitWindow()
    {
        _windowSystem.Init();
        
        _window = _windowSystem.Create(Width, Height, "Vulkan");
    }

    private void InitVulkan()
    {
        ShowExtensions();
    }

    private void MainLoop()
    {
        while (_window != null && !_window.ShouldClose())
        {
            _windowSystem.PollEvents();
        }
    }

    private void Cleanup()
    {
        _window?.Destroy();
        _windowSystem.Terminate();
    }

    private void ShowExtensions() {
        unsafe
        {
            uint extensionCount = 0;
            if (_vulkan.EnumerateInstanceExtensionProperties(null, &extensionCount, null)
                != Vulkan.Result.Success)
            {
                throw new Exception("Failed to enumerate instance extension properties.");
            }
            
            var extensions = stackalloc Vulkan.ExtensionProperties[(int)extensionCount];
            if (_vulkan.EnumerateInstanceExtensionProperties(null, &extensionCount, extensions)
                != Vulkan.Result.Success)
            {
                throw new Exception("Failed to enumerate instance extension properties.");
            }

            
            Console.WriteLine("available extensions:");
            for (var i = 0; i < extensionCount; i++)
            {
                var extension = extensions[i];
                var pName = extension.ExtensionName;
                var name = Marshal.PtrToStringAnsi((nint)pName) ?? "<null>";
                Console.WriteLine($"\t#{i}:\t{name} ({extension.SpecVersion})");
            }
        }
    }
}
