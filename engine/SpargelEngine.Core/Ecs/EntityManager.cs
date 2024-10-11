namespace SpargelEngine.Core.Ecs;


public class EntityManager
{
    private readonly List<Entity> _entities = [];

    private readonly Dictionary<Type, List<Entity>> _map = new();
    
    private readonly Dictionary<Type, IComponent> _globalComponents = new();


    public void Reset()
    {
        _entities.Clear();
        _map.Clear();
        _globalComponents.Clear();
    }
    
    public int EntityCount => _entities.Count;

    public Entity CreateEntity()
    {
        var entity = new Entity();
        _entities.Add(entity);
        return entity;
    }

    public void RemoveEntity(Entity entity)
    {
        foreach (var type in entity.Components.Keys)
        {
            _map[type].Remove(entity);
        }

        _entities.Remove(entity);
    }

    private List<Entity> GetEntityListOrCreate(Type type)
    {
        if (_map.TryGetValue(type, out var list))
            return list;

        List<Entity> newList = [];
        _map[type] = newList;
        return newList;
    }

    private List<Entity> GetEntityListOrCreate<T>() where T : IComponent
    {
        return GetEntityListOrCreate(typeof(T));
    }

    public List<Entity> GetEntities<T>() where T : IComponent
    {
        return _map.ContainsKey(typeof(T)) ? _map[typeof(T)] : [];
    }

    public void AddComponent<T>(Entity entity, T component) where T : IComponent
    {
        entity.Components.Add(typeof(T), component);
        GetEntityListOrCreate<T>().Add(entity);
    }

    public void AddComponents(Entity entity, IEnumerable<IComponent> components)
    {
        foreach (var component in components)
        {
            entity.Components.Add(component.GetType(), component);
            GetEntityListOrCreate(component.GetType()).Add(entity);
        }
    }

    public void SetComponent<T>(Entity entity, T component) where T : IComponent
    {
        if (!entity.Components.ContainsKey(typeof(T))) throw new Exception("entity does not have component of type " + typeof(T));
        entity.Components[typeof(T)] = component;
    }

    public bool HasComponent<T>(Entity entity) where T : IComponent
    {
        return entity.Components.ContainsKey(typeof(T));
    }

    public T GetComponent<T>(Entity entity) where T : IComponent
    {
        var component = entity.Components[typeof(T)];
        return component switch
        {
            null => throw new Exception($"component of type {typeof(T)} not found"),
            T component1 => component1,
            _ => throw new Exception($"want component of type {typeof(T)}, found {component.GetType()}")
        };
    }

    public void AddGlobalComponent<T>(T component) where T : IComponent
    {
        _globalComponents.Add(typeof(T), component);
    }

    public void SetGlobalComponent<T>(T component) where T : IComponent
    {
        _globalComponents[typeof(T)] = component;
    }

    public bool HasGlobalComponent<T>() where T : IComponent
    {
        return _globalComponents.ContainsKey(typeof(T));
    }

    public T GetGlobalComponent<T>() where T : IComponent
    {
        var component =  _globalComponents[typeof(T)];
        return component switch
        {
            null => throw new Exception($"global component of type {typeof(T)} not found"),
            T component1 => component1,
            _ => throw new Exception($"want global component of type {typeof(T)}, found {component.GetType()}")
        };
    }
}
