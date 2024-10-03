namespace SpargelEngine.Graphics.Vulkan;


public partial class Vulkan
{
    public struct ApplicationInfo
    {
        public StructureType sType;
        public unsafe void* pNext;
        public unsafe byte* pApplicationName;
        public uint applicationVersion;
        public unsafe byte* pEngineName;
        public uint engineVersion;
        public uint apiVersion;
    }
}
