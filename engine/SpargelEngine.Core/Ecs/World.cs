namespace SpargelEngine.Core.Ecs;


public class World
{
    public EntityManager EntityManager { get; } = new();
    
    public SystemManager SystemManager { get; }
    
    private bool _shouldShutdown = false;

    public World()
    {
        SystemManager = new SystemManager(this);
    }
    
    public void SetShouldShutdown(bool shouldShutdown)
    {
        _shouldShutdown = shouldShutdown;
    }

    public void Run()
    {
        SystemManager.OnCreate();
        
        var lastLogTime = Environment.TickCount;

        while (!_shouldShutdown)
        {
            SystemManager.Update();
            Thread.Sleep(1000 / 60);
            
            var curTime = Environment.TickCount;
            if (curTime - lastLogTime <= 500) continue;
            Console.WriteLine($"Entity Count: {EntityManager.EntityCount}");
            lastLogTime = curTime;
        }
        
        SystemManager.OnDestroy();
        
        SystemManager.Reset();
        EntityManager.Reset();
    }
}
