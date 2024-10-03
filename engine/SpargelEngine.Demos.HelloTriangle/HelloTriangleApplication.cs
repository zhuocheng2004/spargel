using System.Runtime.InteropServices;
using SpargelEngine.Graphics.Vulkan;
using SpargelEngine.Graphics.Vulkan.Linux;

namespace SpargelEngine.Demos.HelloTriangle;


public class HelloTriangleApplication
{
    private readonly Vulkan _vulkan = new VulkanLinux();

    public void Run()
    {
        InitWindow();
        InitVulkan();
        MainLoop();
        Cleanup();
    }

    private void InitWindow() { }

    private void InitVulkan()
    {
        ShowExtensions();
    }
    
    private void MainLoop() { }
    
    private void Cleanup() { }

    private void ShowExtensions() {
        unsafe
        {
            uint extensionCount = 0;
            if (_vulkan.EnumerateInstanceExtensionProperties(null, &extensionCount, null)
                != Vulkan.Result.Success)
            {
                throw new Exception("Failed to enumerate instance extension properties.");
            }
            
            var extensions = new Vulkan.ExtensionProperties[(int)extensionCount];
            fixed (Vulkan.ExtensionProperties* pExtensions = &extensions[0])
            {
                if (_vulkan.EnumerateInstanceExtensionProperties(null, &extensionCount, pExtensions)
                    != Vulkan.Result.Success)
                {
                    throw new Exception("Failed to enumerate instance extension properties.");
                }
            }
            
            Console.WriteLine("available extensions:");

            foreach (var extension in extensions)
            {
                var pName = extension.ExtensionName;
                var name = Marshal.PtrToStringAnsi((nint)pName) ?? "<null>";
                Console.WriteLine($"\t{name}");
            }
        }
    }
}
