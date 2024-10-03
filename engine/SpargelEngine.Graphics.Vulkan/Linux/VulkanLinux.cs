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

    [DllImport(VulkanLibName)]
    private static extern unsafe Result vkEnumerateInstanceExtensionProperties(byte* pLayerName, uint* pPropertyCount,
        ExtensionProperties* pProperties);
}
