using Microsoft.Extensions.DependencyInjection;

namespace Sunlight.ManagementStudio.Helpers;

public static class ServiceProviderServiceExtensions
{
    public static T GetRequired<T>(this IServiceProvider provider) where T : notnull
    {
        System.Diagnostics.Debug.Assert(provider != null);

        return (T)provider.GetRequiredService(typeof(T));
    }
}
