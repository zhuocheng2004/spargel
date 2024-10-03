namespace SpargelEngine.Graphics.Vulkan;


public partial class Vulkan
{
    public struct ExtensionProperties
    {
        public unsafe fixed byte extensionName[(int) MaxExtensionNameSize];
        public uint specVersion;
    }
}
