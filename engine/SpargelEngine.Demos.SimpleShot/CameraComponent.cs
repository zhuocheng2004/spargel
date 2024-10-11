using SpargelEngine.Core.Ecs;

namespace SpargelEngine.Demos.SimpleShot;


public struct CameraComponent : IComponent
{
    public enum Axis : byte
    {
        Horizontal,
        Vertical
    }
    
    public Axis MainAxis;
    public float Size;
    public float Near;
    public float Far;
}
