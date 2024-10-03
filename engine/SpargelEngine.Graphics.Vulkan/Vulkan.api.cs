namespace SpargelEngine.Graphics.Vulkan;


public partial class Vulkan
{
    public const uint MaxExtensionNameSize = 256;
    
    public abstract unsafe Result EnumerateInstanceExtensionProperties(byte* pLayerName, uint* pPropertyCount, 
        ExtensionProperties* pProperties);

    public abstract unsafe Result CreateInstance(InstanceCreateInfo* pCreateInfo, nint pAllocator, Instance* pInstance);
    
    public abstract void DestroyInstance(Instance instance, nint pAllocator);
}
