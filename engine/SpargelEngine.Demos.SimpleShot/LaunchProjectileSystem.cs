using SpargelEngine.Core.Ecs;

namespace SpargelEngine.Demos.SimpleShot;


public class LaunchProjectileSystem : ISystem
{
    public void OnCreate() { }

    public void OnDestroy() { }

    public void OnUpdate(World world)
    {
        var entityManager = world.EntityManager;

        var input = entityManager.GetGlobalComponent<InputComponent>();
        if (input.SpaceCount == 0) return;

        foreach (var entity in entityManager.GetEntities<LaunchProjectileComponent>())
        {
            var position = entityManager.GetComponent<PositionComponent>(entity);
            
            var projectile = entityManager.CreateEntity();
            entityManager.AddComponents(projectile, [
                position,
                new RectangleComponent { X0 = -2, X1 = 2, Y0 = -2, Y1 = 2 },
                new ColorComponent { R = 1.0f, G = 1.0f, B = 0.0f, A = 1.0f },
                new MoveAlongXComponent { Speed = 100 },
                new DestroySelfComponent { RangeX = 100, RangeY = 100 }
            ]);
        }

        input.SpaceCount--;
        entityManager.SetGlobalComponent(input);
    }
}
