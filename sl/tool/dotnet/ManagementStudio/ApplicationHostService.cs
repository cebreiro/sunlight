using System.Windows;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Models.Event;
using Sunlight.ManagementStudio.Models.Event.Args;
using Sunlight.ManagementStudio.Models.Setting;
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

        serviceProvider.GetRequired<IEventListener>().Listen(OnDisconnected);

        serviceProvider.GetRequiredService<SettingsProvider>().LoadFromFile();

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

        mainWindow.IsEnabled = false;
        mainWindow.TitleBar.ShowMinimize = false;
        mainWindow.TitleBar.ShowMaximize = false;
        mainWindow.TitleBar.ShowClose = false;

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

    private void OnDisconnected(DisconnectionEventArgs args)
    {
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            LoginWindow loginWindow = serviceProvider.GetRequiredService<LoginWindow>();
            if (loginWindow.Visibility == Visibility.Hidden)
            {
                MainWindow mainWindow = serviceProvider.GetRequired<MainWindow>();

                mainWindow.IsEnabled = false;
                mainWindow.TitleBar.ShowMinimize = false;
                mainWindow.TitleBar.ShowMaximize = false;
                mainWindow.TitleBar.ShowClose = false;

                loginWindow.Owner = mainWindow;
                loginWindow.Show();
            }
        });
    }
}
