using System.Runtime.InteropServices;
using SpargelEngine.Graphics.Vulkan;
using SpargelEngine.Graphics.Vulkan.Linux;
using SpargelEngine.GUI.Window.GLFW;

namespace SpargelEngine.Demos.HelloTriangle;


public class HelloTriangleApplication
{
    private const uint Width = 800;
    private const uint Height = 600;
    
    private readonly GlfwWindowSystem _windowSystem = new();
    private readonly Vulkan _vulkan = new VulkanLinux();
    
    private GlfwWindow? _window;
    private Vulkan.Instance _instance;

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
        
        Glfw.glfwWindowHint(Glfw.ClientApi, Glfw.NoApi);
        Glfw.glfwWindowHint(Glfw.Resizable, Glfw.False);
        
        _window = _windowSystem.Create(Width, Height, "Vulkan");
    }

    private void InitVulkan()
    {
        ShowExtensions();
        CreateInstance();
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
        _vulkan.DestroyInstance(_instance, nint.Zero);
            
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

    private void CreateInstance()
    {
        unsafe
        {
            Vulkan.ApplicationInfo appInfo;
            appInfo.Type = Vulkan.StructureType.ApplicationInfo;
            appInfo.ApplicationName = (byte*) Marshal.StringToHGlobalAnsi("Hello Triangle");
            appInfo.ApplicationVersion = Vulkan.MakeVersion(1, 0, 0);
            appInfo.EngineName = (byte*) Marshal.StringToHGlobalAnsi("No Engine");
            appInfo.EngineVersion = Vulkan.MakeVersion(1, 0, 0);
            appInfo.ApiVersion = Vulkan.ApiVersion_1_0;
            
            Vulkan.InstanceCreateInfo createInfo;
            createInfo.Type = Vulkan.StructureType.InstanceCreateInfo;
            createInfo.ApplicationInfo = &appInfo;

            uint glfwExtensionCount = 0;
            byte** glfwExtensions;
            
            glfwExtensions = Glfw.glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
            
            createInfo.EnabledExtensionCount = glfwExtensionCount;
            createInfo.EnabledExtensionNames = glfwExtensions;

            createInfo.EnabledLayerCount = 0;

            fixed (Vulkan.Instance* pInstance = &_instance)
            {
                if (_vulkan.CreateInstance(&createInfo, nint.Zero, pInstance) != Vulkan.Result.Success)
                {
                    throw new Exception("Failed to create instance.");
                }
            }
        }
    }
}
