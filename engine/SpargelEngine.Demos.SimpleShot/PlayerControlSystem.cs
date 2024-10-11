using SpargelEngine.Core.Ecs;

namespace SpargelEngine.Demos.SimpleShot;


public class PlayerControlSystem : ISystem
{
    public void OnCreate() { }

    public void OnDestroy() { }

    public void OnUpdate(World world)
    {
        var entityManager = world.EntityManager;

        var deltaTime = entityManager.GetGlobalComponent<TimeComponent>().DeltaTime;
        var verticalInput = entityManager.GetGlobalComponent<InputComponent>().VerticalInput;

        foreach (var player in entityManager.GetEntities<PlayerControlComponent>())
        {
            var playerControlComponent = entityManager.GetComponent<PlayerControlComponent>(player);
            var position = entityManager.GetComponent<PositionComponent>(player);
            position.Y += verticalInput * playerControlComponent.Speed * deltaTime;
            
            if (position.Y >= playerControlComponent.Range) position.Y = playerControlComponent.Range;
            if (position.Y <= -playerControlComponent.Range) position.Y = -playerControlComponent.Range;
            
            entityManager.SetComponent(player, position);
        }
    }
}
