using SpargelEngine.Core.Ecs;

namespace SpargelEngine.Demos.SimpleShot;


public static class Program
{
    public static void Main()
    {
        var world = new World();
        var systemManager = world.SystemManager;
        var entityManager = world.EntityManager;
        
        // systems
        systemManager.RegisterSystem(new RenderSystem());
        systemManager.RegisterSystem(new PlayerControlSystem());
        systemManager.RegisterSystem(new MoveAlongXSystem());
        systemManager.RegisterSystem(new DestroySelfSystem());
        systemManager.RegisterSystem(new LaunchProjectileSystem());
        
        // global components
        entityManager.SetGlobalComponent(new TimeComponent());
        entityManager.SetGlobalComponent(new InputComponent());
            
        // entities
        var camera = entityManager.CreateEntity();
        entityManager.AddComponents(camera, [
            new CameraComponent { MainAxis = CameraComponent.Axis.Vertical, Size = 100, Near = 0, Far = 100 },
            new PositionComponent { X = 0, Y = 0, Z = -10 }
        ]);
        
        var player = entityManager.CreateEntity();
        entityManager.AddComponents(player, [
            new PositionComponent { X = -50, Y = 0, Z = 0 },
            new PlayerControlComponent { Speed = 50, Range = 40 },
            new RectangleComponent { X0 = -5, X1 = 5, Y0 = -5, Y1 = 5 },
            new ColorComponent { R = 0.0f, G = 1.0f, B = 0.0f, A = 1.0f },
            new LaunchProjectileComponent()
        ]);
        
        world.Run();
    }
}
