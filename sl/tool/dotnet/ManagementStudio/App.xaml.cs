using System.Text;
using System.Windows;
using System.Windows.Threading;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Models.Controller;
using Sunlight.ManagementStudio.Models.Controller.Tcp;
using Sunlight.ManagementStudio.Models.Event;
using Sunlight.ManagementStudio.Models.Setting;
using Sunlight.ManagementStudio.ViewModels.Pages;
using Sunlight.ManagementStudio.ViewModels.Pages.LogViewer;
using Sunlight.ManagementStudio.ViewModels.Windows;
using Sunlight.ManagementStudio.Views.Pages;
using Sunlight.ManagementStudio.Views.Windows;
using Wpf.Ui;
using EventManager = Sunlight.ManagementStudio.Models.Event.EventManager;

namespace Sunlight.ManagementStudio;

public partial class App
{
    private readonly IHost _host;

    private App()
    {
        Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);

        _host = Host.CreateDefaultBuilder()
            .ConfigureServices((_1, services) =>
            {
                _ = services.AddSingleton<SettingsProvider>();

                _ = services.AddNavigationViewPageProvider();
                _ = services.AddHostedService<ApplicationHostService>();

                _ = services.AddSingleton<INavigationService, NavigationService>();
                _ = services.AddSingleton<ISnackbarService, SnackbarService>();
                _ = services.AddSingleton<IContentDialogService, ContentDialogService>();

                _ = services.AddSingleton<ISunlightController, SunlightTcpController>();
                _ = services.AddSingleton<EventManager, EventManager>();
                _ = services.AddSingleton<IEventProducer>(serviceProvider => serviceProvider.GetRequired<EventManager>());
                _ = services.AddSingleton<IEventListener>(serviceProvider => serviceProvider.GetRequired<EventManager>());

                _ = services.AddSingleton<MainWindowViewModel>();
                _ = services.AddSingleton<MainWindow>();

                _ = services.AddSingleton<SettingsViewModel>();
                _ = services.AddSingleton<SettingsPage>();

                _ = services.AddSingleton<AccountPageViewModel>();
                _ = services.AddSingleton<AccountPage>();

                _ = services.AddSingleton<LogViewerViewModel>();
                _ = services.AddSingleton<DashBoardPageViewModel>();
                _ = services.AddSingleton<DashBoardPage>();

                _ = services.AddSingleton<WorldPage>();
                _ = services.AddSingleton<WorldPageViewModel>();

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
