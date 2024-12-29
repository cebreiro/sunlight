using Wpf.Ui.Abstractions;


namespace Sunlight.ManagementStudio.Helpers;

public class NavigationViewPageProvider(IServiceProvider serviceProvider) : INavigationViewPageProvider
{
    public object? GetPage(Type pageType)
    {
        return serviceProvider.GetService(pageType);
    }
}
