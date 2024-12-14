using System.Windows;
using System.Windows.Threading;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Models.Setting;
using Sunlight.ManagementStudio.ViewModels.Pages;
using Sunlight.ManagementStudio.ViewModels.Windows;
using Sunlight.ManagementStudio.Views.Pages;
using Sunlight.ManagementStudio.Views.Windows;
using Wpf.Ui;

namespace Sunlight.ManagementStudio;

public partial class App
{
    private readonly IHost _host;

    private App()
    {
        _host = Host.CreateDefaultBuilder()
            .ConfigureServices((_1, services) =>
            {
                _ = services.AddSingleton<SettingsProvider>();

                _ = services.AddNavigationViewPageProvider();
                _ = services.AddHostedService<ApplicationHostService>();

                _ = services.AddSingleton<INavigationService, NavigationService>();
                _ = services.AddSingleton<ISnackbarService, SnackbarService>();
                _ = services.AddSingleton<IContentDialogService, ContentDialogService>();

                _ = services.AddSingleton<MainWindowViewModel>();
                _ = services.AddSingleton<MainWindow>();

                _ = services.AddSingleton<SettingsViewModel>();
                _ = services.AddSingleton<SettingsPage>();

                _ = services.AddSingleton<HomePage>();

                _ = services.AddSingleton<LoginViewModel>();
                _ = services.AddSingleton<LoginWindow>();

            })
            .Build();
    }

    private void OnStartUp(object sender, StartupEventArgs e)
    {
        _host.Start();
    }

    private void OnExit(object sender, ExitEventArgs e)
    {
        _host.StopAsync().Wait();

        _host.Dispose();
    }

    private void OnDispatcherUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
    {
        MessageBox.Show(App.Current.MainWindow, $"{e.Exception.Message}", "exception", MessageBoxButton.OK, MessageBoxImage.Error);

        e.Handled = true;
    }
}
