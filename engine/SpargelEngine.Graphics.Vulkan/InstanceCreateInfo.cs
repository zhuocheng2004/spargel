namespace SpargelEngine.Graphics.Vulkan;


public abstract partial class Vulkan
{
    public struct InstanceCreateInfo
    {
        public StructureType Type;
        public unsafe void* Next;
        public uint Flags;
        public unsafe ApplicationInfo* ApplicationInfo;
        public uint EnabledLayerCount;
        public unsafe byte** EnabledLayerNames;
        public uint EnabledExtensionCount;
        public unsafe byte** EnabledExtensionNames;
    }
}
