namespace SpargelEngine.Graphics.Vulkan;


public partial class Vulkan
{
    public struct ExtensionProperties
    {
        public unsafe fixed byte ExtensionName[(int) MaxExtensionNameSize];
        public uint SpecVersion;
    }
}
