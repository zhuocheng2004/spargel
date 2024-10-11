namespace SpargelEngine.Graphics.Vulkan;


public abstract partial class Vulkan
{
    public abstract void Initialize();
    
    public abstract void Terminate();
    
    public static uint MakeApiVersion(uint variant, uint major, uint minor, uint patch)
    {
        return (variant << 29) | (major << 22) | (minor << 12) | (patch << 0);
    }

    public static readonly uint ApiVersion_1_0 = MakeApiVersion(0, 1, 0, 0);
    
    public static uint MakeVersion(uint major, uint minor, uint patch)
    {
        return (major << 22) | (minor << 12) | (patch << 0);
    }
}
