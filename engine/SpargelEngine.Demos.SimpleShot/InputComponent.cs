using SpargelEngine.Core.Ecs;

namespace SpargelEngine.Demos.SimpleShot;


public struct InputComponent : IComponent
{
    public float VerticalInput;

    public uint SpaceCooldown;
    public uint SpaceCount;
}
