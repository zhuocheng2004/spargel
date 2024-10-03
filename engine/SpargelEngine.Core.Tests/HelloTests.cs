namespace SpargelEngine.Core.Tests;


public class HelloTests
{
    [Fact]
    public void ShouldSayHello()
    {
        Assert.Equal("Hello!", Hello.SayHello());
    }
}
