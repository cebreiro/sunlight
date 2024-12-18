using System;
using System.Windows;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Models.Event;
using Sunlight.ManagementStudio.Models.Event.Args;
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

        serviceProvider.GetRequired<IEventListener>().Listen(OnDisconnected);

        ApplyApplicationTheme();
        OpenLoginWindow(mainWindow);

        //mainWindow.NavigationView.Navigate(typeof(AccountPage));
    }

    private void ApplyApplicationTheme()
    {
        string themeSetting = serviceProvider.GetRequiredService<SettingsProvider>().AppearanceSetting.Theme;

        if (Enum.TryParse(themeSetting, true, out ApplicationTheme theme))
        {
            ApplicationThemeManager.Apply(theme);
        }
    }

    private void OpenLoginWindow(MainWindow mainWindow)
    {
        mainWindow.IsEnabled = false;
        mainWindow.TitleBar.ShowMinimize = false;
        mainWindow.TitleBar.ShowMaximize = false;
        mainWindow.TitleBar.ShowClose = false;

        LoginWindow loginWindow = serviceProvider.GetRequiredService<LoginWindow>();
        loginWindow.Owner = mainWindow;
        loginWindow.Closed += (sender, args) =>
        {
            Application.Current.Dispatcher.InvokeAsync(() =>
            {
                mainWindow.IsEnabled = true;
                mainWindow.TitleBar.ShowMinimize = true;
                mainWindow.TitleBar.ShowMaximize = true;
                mainWindow.TitleBar.ShowClose = true;

                mainWindow.Activate();

                _ = mainWindow.NavigationView.Navigate(typeof(HomePage));
            });
        };

        loginWindow.Show();
    }

    void OnDisconnected(DisconnectionEventArgs args)
    {
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            OpenLoginWindow(serviceProvider.GetRequired<MainWindow>());
        });
    }
}
