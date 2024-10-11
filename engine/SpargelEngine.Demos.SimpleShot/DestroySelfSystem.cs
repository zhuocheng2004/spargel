using SpargelEngine.Core.Ecs;

namespace SpargelEngine.Demos.SimpleShot;


public class DestroySelfSystem : ISystem
{
    public void OnCreate() { }

    public void OnDestroy() { }

    public void OnUpdate(World world)
    {
        var entityManager = world.EntityManager;

        List<Entity> entitiesToRemove = [];
        
        foreach (var entity in entityManager.GetEntities<DestroySelfComponent>())
        {
            var position = entityManager.GetComponent<PositionComponent>(entity);
            var destroySelfComponent = entityManager.GetComponent<DestroySelfComponent>(entity);
            var rangeX = destroySelfComponent.RangeX;
            var rangeY = destroySelfComponent.RangeY;

            if (position.X < -rangeX || position.X > rangeX || position.Y < -rangeY || position.Y > rangeY)
                entitiesToRemove.Add(entity);
        }

        foreach (var entity in entitiesToRemove)
            entityManager.RemoveEntity(entity);
    }
}
