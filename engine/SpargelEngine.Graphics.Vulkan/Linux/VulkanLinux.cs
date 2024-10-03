using System.Runtime.InteropServices;

namespace SpargelEngine.Graphics.Vulkan.Linux;


public class VulkanLinux : Vulkan
{
    private const string VulkanLibName = "libvulkan.so.1";

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
