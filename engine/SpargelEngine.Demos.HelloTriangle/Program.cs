namespace SpargelEngine.Demos.HelloTriangle;


public static class Program
{
    public static void Main()
    {
        var app = new HelloTriangleApplication();

        try
        {
            app.Run();
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
    }
}
