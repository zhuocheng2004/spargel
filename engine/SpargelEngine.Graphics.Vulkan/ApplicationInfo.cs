namespace SpargelEngine.Graphics.Vulkan;


public partial class Vulkan
{
    public struct ApplicationInfo
    {
        public StructureType Type;
        public unsafe void* Next;
        public unsafe byte* ApplicationName;
        public uint ApplicationVersion;
        public unsafe byte* EngineName;
        public uint EngineVersion;
        public uint ApiVersion;
    }
}
