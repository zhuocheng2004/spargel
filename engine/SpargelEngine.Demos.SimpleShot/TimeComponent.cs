using SpargelEngine.Core.Ecs;

namespace SpargelEngine.Demos.SimpleShot;


public struct TimeComponent : IComponent
{
    public bool FirstFrame = true; 
    public float LastFrameTime;
    public float DeltaTime;
    
    public TimeComponent() { }
}
