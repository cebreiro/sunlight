using System.Windows;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Sunlight.ManagementStudio.Models.Setting;
using Sunlight.ManagementStudio.Views.Pages;
using Sunlight.ManagementStudio.Views.Windows;
using Wpf.Ui.Appearance;

namespace Sunlight.ManagementStudio;

public class ApplicationHostService(IServiceProvider serviceProvider) : IHostedService
{
    public Task StartAsync(CancellationToken cancellationToken)
    {
        if (Application.Current.Windows.OfType<MainWindow>().Any())
        {
            return Task.CompletedTask;
        }

        serviceProvider.GetRequiredService<SettingsProvider>().LoadFromFileAsync();

        MainWindow mainWindow = serviceProvider.GetRequiredService<MainWindow>();
        mainWindow.Loaded += OnMainWindowLoaded;
        mainWindow?.Show();

        return Task.CompletedTask;
    }

    public async Task StopAsync(CancellationToken cancellationToken)
    {
        await serviceProvider.GetRequiredService<SettingsProvider>().SaveToFileAsync().ConfigureAwait(false);
    }

    private void OnMainWindowLoaded(object sender, RoutedEventArgs e)
    {
        if (sender is not MainWindow mainWindow)
        {
            return;
        }

        ApplyApplicationTheme();

        //_ = mainWindow.NavigationView.Navigate(typeof(HomePage));

        LoginWindow loginWindow = serviceProvider.GetRequiredService<LoginWindow>();
        loginWindow.Owner = mainWindow;
        loginWindow.Show();
    }

    private void ApplyApplicationTheme()
    {
        string themeSetting = serviceProvider.GetRequiredService<SettingsProvider>().AppearanceSetting.Theme;

        if (Enum.TryParse(themeSetting, true, out ApplicationTheme theme))
        {
            ApplicationThemeManager.Apply(theme);
        }
    }
}
