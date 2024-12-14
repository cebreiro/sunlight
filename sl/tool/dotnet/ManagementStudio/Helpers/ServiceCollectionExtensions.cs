using Microsoft.Extensions.DependencyInjection;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Wpf.Ui.Abstractions;

namespace Sunlight.ManagementStudio.Helpers;

public static class ServiceCollectionExtensions
{
    public static IServiceCollection AddNavigationViewPageProvider(this IServiceCollection services)
    {
        _ = services.AddSingleton<INavigationViewPageProvider, NavigationViewPageProvider>();

        return services;
    }
}
