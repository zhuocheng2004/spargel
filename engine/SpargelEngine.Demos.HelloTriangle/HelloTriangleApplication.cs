using System.Runtime.InteropServices;
using SpargelEngine.Graphics.Vulkan;
using SpargelEngine.Graphics.Vulkan.Linux;
using SpargelEngine.GUI.Window.GLFW;

namespace SpargelEngine.Demos.HelloTriangle;


public class HelloTriangleApplication
{
    private const uint Width = 800;
    private const uint Height = 600;
    
    private readonly GlfwWindowSystem _windowSystem = new GlfwWindowSystem();
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
        
        GlfwWindowSystem.WindowHint(GlfwWindowSystem.ClientApi, GlfwWindowSystem.NoApi);
        GlfwWindowSystem.WindowHint(GlfwWindowSystem.Resizable, GlfwWindowSystem.False);
        
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
                var pName = extension.extensionName;
                var name = Marshal.PtrToStringAnsi((nint)pName) ?? "<null>";
                Console.WriteLine($"\t#{i}:\t{name} ({extension.specVersion})");
            }
        }
    }

    private void CreateInstance()
    {
        unsafe
        {
            Vulkan.ApplicationInfo appInfo;
            appInfo.sType = Vulkan.StructureType.ApplicationInfo;
            appInfo.pApplicationName = (byte*) Marshal.StringToHGlobalAnsi("Hello Triangle");
            appInfo.applicationVersion = Vulkan.MakeVersion(1, 0, 0);
            appInfo.pEngineName = (byte*) Marshal.StringToHGlobalAnsi("No Engine");
            appInfo.engineVersion = Vulkan.MakeVersion(1, 0, 0);
            appInfo.apiVersion = Vulkan.ApiVersion_1_0;
            
            Vulkan.InstanceCreateInfo createInfo;
            createInfo.sType = Vulkan.StructureType.InstanceCreateInfo;
            createInfo.pApplicationInfo = &appInfo;

            uint glfwExtensionCount = 0;
            byte** glfwExtensions;
            
            glfwExtensions = GlfwWindowSystem.GetRequiredInstanceExtensions(&glfwExtensionCount);
            
            createInfo.enabledExtensionCount = glfwExtensionCount;
            createInfo.ppEnabledExtensionNames = glfwExtensions;

            createInfo.enabledLayerCount = 0;

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
