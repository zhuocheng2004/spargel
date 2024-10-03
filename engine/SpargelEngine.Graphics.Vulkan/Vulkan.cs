namespace SpargelEngine.Graphics.Vulkan;


public abstract partial class Vulkan
{
    public const uint MaxExtensionNameSize = 256;
    
    public abstract unsafe Result EnumerateInstanceExtensionProperties(byte* pLayerName, uint* pPropertyCount, 
        ExtensionProperties* pProperties);
}
