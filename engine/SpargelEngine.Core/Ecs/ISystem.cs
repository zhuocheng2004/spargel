namespace SpargelEngine.Core.Ecs;


public interface ISystem
{
    public void OnCreate();

    public void OnDestroy();

    public void OnUpdate(World world);
    
    public int Order() => 0;
}
