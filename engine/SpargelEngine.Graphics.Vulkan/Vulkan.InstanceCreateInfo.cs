namespace SpargelEngine.Graphics.Vulkan;


public abstract partial class Vulkan
{
    public struct InstanceCreateInfo
    {
        public StructureType sType;
        public unsafe void* pNext;
        public uint flags;
        public unsafe ApplicationInfo* pApplicationInfo;
        public uint enabledLayerCount;
        public unsafe byte** ppEnabledLayerNames;
        public uint enabledExtensionCount;
        public unsafe byte** ppEnabledExtensionNames;
    }
}
