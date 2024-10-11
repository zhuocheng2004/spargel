namespace SpargelEngine.Core.Ecs;


public class Entity
{
    public readonly Dictionary<Type, IComponent> Components = new();
}
