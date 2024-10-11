using System.Reflection;
using System.Runtime.InteropServices;

namespace SpargelEngine.Graphics.Vulkan;


public class VulkanGeneric : Vulkan
{
    private const string VulkanLibName = "vulkan";
    
    static VulkanGeneric()
    {
        NativeLibrary.SetDllImportResolver(Assembly.GetExecutingAssembly(), DllImportResolver);
    }

    private static IntPtr DllImportResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
    {
        if (libraryName != "vulkan") return IntPtr.Zero;
        
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            return NativeLibrary.Load("vulkan-1.dll");
        if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            return NativeLibrary.Load("libMoltenVK.dylib"); // TODO
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            return NativeLibrary.Load("libvulkan.so.1");

        return IntPtr.Zero;
    }
    

    public override void Initialize() { }

    public override void Terminate() { }

    public override unsafe Result EnumerateInstanceExtensionProperties(byte* pLayerName, uint* pPropertyCount,
        ExtensionProperties* pProperties)
    {
        return vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
    }

    public override unsafe Result CreateInstance(InstanceCreateInfo* pCreateInfo, nint pAllocator, Instance* pInstance)
    {
        return vkCreateInstance(pCreateInfo, pAllocator, pInstance);
    }

    public override void DestroyInstance(Instance instance, IntPtr pAllocator)
    {
        vkDestroyInstance(instance, pAllocator);
    }

    [DllImport(VulkanLibName)]
    private static extern unsafe Result vkEnumerateInstanceExtensionProperties(byte* pLayerName, uint* pPropertyCount,
        ExtensionProperties* pProperties);

    [DllImport(VulkanLibName)]
    private static extern unsafe Result vkCreateInstance(InstanceCreateInfo* pCreateInfo, nint pAllocator,
        Instance* pInstance);
    
    [DllImport(VulkanLibName)]
    private static extern void vkDestroyInstance(Instance instance, nint pAllocator);
}
