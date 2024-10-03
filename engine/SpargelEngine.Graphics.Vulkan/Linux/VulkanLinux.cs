using System.Runtime.InteropServices;

namespace SpargelEngine.Graphics.Vulkan.Linux;


public class VulkanLinux : Vulkan
{
    const string VulkanLibName = "libvulkan.so.1";

    public override unsafe Result EnumerateInstanceExtensionProperties(byte* pLayerName, uint* pPropertyCount,
        ExtensionProperties* pProperties)
    {
        return NEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
    }

    [DllImport(VulkanLibName, EntryPoint = "vkEnumerateInstanceExtensionProperties")]
    static extern unsafe Result NEnumerateInstanceExtensionProperties(byte* pLayerName, uint* pPropertyCount,
        ExtensionProperties* pProperties);
}
