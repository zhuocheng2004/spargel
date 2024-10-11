namespace SpargelEngine.Core.Ecs;


public class SystemManager(World world)
{
    private readonly SortedDictionary<int, List<ISystem>> _map = new();

    private List<ISystem> GetSystemListOrCreate(int order)
    {
        if (_map.TryGetValue(order, out var list))
            return list;
        
        List<ISystem> newList = [];
        _map.Add(order, newList);
        return newList;
    }

    public void Reset()
    {
        _map.Clear();
    }

    public void RegisterSystem(ISystem system)
    {
        GetSystemListOrCreate(system.Order()).Add(system);
    }

    public void UnregisterSystem(ISystem system)
    {
        GetSystemListOrCreate(system.Order()).Remove(system);
    }

    public void OnCreate()
    {
        foreach (var system in _map.Keys.SelectMany(order => _map[order]))
        {
            system.OnCreate();
        }
    }

    public void OnDestroy()
    {
        foreach (var system in _map.Keys.SelectMany(order => _map[order]))
        {
            system.OnDestroy();
        }
    }

    public void Update()
    {
        foreach (var system in _map.Keys.SelectMany(order => _map[order]))
        {
            system.OnUpdate(world);
        }
    }
}
