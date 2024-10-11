using SpargelEngine.Core.Ecs;

namespace SpargelEngine.Demos.SimpleShot;


public class MoveAlongXSystem : ISystem
{
    public void OnCreate() { }

    public void OnDestroy() { }

    public void OnUpdate(World world)
    {
        var entityManager = world.EntityManager;
        var deltaTime = entityManager.GetGlobalComponent<TimeComponent>().DeltaTime;

        foreach (var entity in entityManager.GetEntities<MoveAlongXComponent>())
        {
            var speed = entityManager.GetComponent<MoveAlongXComponent>(entity).Speed;
            
            var position = entityManager.GetComponent<PositionComponent>(entity);
            position.X += speed * deltaTime;
            entityManager.SetComponent(entity, position);
        }
    }
}
