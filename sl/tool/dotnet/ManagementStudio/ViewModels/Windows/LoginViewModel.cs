using System.Diagnostics.Tracing;
using System.Net;
using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Sunlight.Api;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Models.Controller;
using Sunlight.ManagementStudio.Models.Event;
using Sunlight.ManagementStudio.Models.Event.Args;
using Sunlight.ManagementStudio.Models.Setting;
using Sunlight.ManagementStudio.Views.Pages;
using Sunlight.ManagementStudio.Views.Windows;
using Wpf.Ui.Controls;

namespace Sunlight.ManagementStudio.ViewModels.Windows;

public partial class LoginViewModel : ObservableObject
{
    private readonly IServiceProvider _serviceProvider;
    private readonly ISunlightController _sunlightController;
    private readonly IEventProducer _eventProducer;

    private bool _isPendingConnection = false;
    private bool _isConnected = false;

    public bool IsAuthenticated { get; private set; } = false;

    public bool IsActive
    {
        get
        {
            if (_isPendingConnection)
            {
                return false;
            }

            if (_isConnected)
            {
                return false;
            }

            return true;
        }
    }

    public string Address { get; set; } = "127.0.0.1";
    public string Port { get; set; } = "8989";
    public string Id { get; set; } = string.Empty;

    public LoginWindow? LoginWindow { get; set; } = null;

    public LoginViewModel(IServiceProvider serviceProvider, ISunlightController sunlightController, IEventListener eventListener, IEventProducer eventProducer)
    {
        _serviceProvider = serviceProvider;
        _sunlightController = sunlightController;
        _eventProducer = eventProducer;

        _ = eventListener.Listen(OnDisconnected);

        ConnectionSetting connectionSetting = _serviceProvider.GetRequired<SettingsProvider>().ConnectionSetting;

        if (!string.IsNullOrEmpty(connectionSetting.Address))
        {
            Address = connectionSetting.Address;
        }

        if (!string.IsNullOrEmpty(connectionSetting.Port))
        {
            Port = connectionSetting.Port;
        }

        if (!string.IsNullOrEmpty(connectionSetting.Id))
        {
            Id = connectionSetting.Id;
        }
    }

    [RelayCommand]
    private void OnConnectButtonClicked(object sender)
    {
        _isPendingConnection = true;
        OnPropertyChanged(nameof(IsActive));

        if (LoginWindow == null)
        {
            return;
        }

        string password = LoginWindow.PasswordBox.Password;

        if (!IPAddress.TryParse(Address, out _))
        {
            Application.Current.Dispatcher.InvokeAsync(() =>
            {
                _isPendingConnection = false;

                OnPropertyChanged(nameof(IsActive));

                LoginWindow?.ShowErrorDialogAsync($"invalid ip-address. {Address}");
            });
            

            return;
        }

        Task.Run(async () =>
        {
            try
            {
                bool connect = await _sunlightController.Connect(Address, ushort.Parse(Port));

                if (!connect)
                {
                    Application.Current.Dispatcher.InvokeAsync(() =>
                    {
                        _isPendingConnection = false;
                        _isConnected = false;

                        OnPropertyChanged(nameof(IsActive));

                        LoginWindow?.ShowErrorDialogAsync($"fail to connect to {Address}:{Port}");
                    });

                    return;
                }

                AuthenticationRequest request = new();
                request.Id = Id;
                request.Password = password;

                AuthenticationResponse response = await _sunlightController.Authenticate(request);

                Application.Current.Dispatcher.InvokeAsync(() =>
                {
                    _isPendingConnection = false;

                    if (response.Success == 0)
                    {
                        _isConnected = false;
                        OnPropertyChanged(nameof(IsActive));

                        LoginWindow?.ShowErrorDialogAsync("fail to authenticate.");
                    }
                    else
                    {
                        IsAuthenticated = true;
                        OnPropertyChanged(nameof(IsActive));

                        LoginWindow?.Hide();

                        _eventProducer.Produce(new AuthenticationEventArgs());

                        MainWindow mainWindow = _serviceProvider.GetRequired<MainWindow>();

                        mainWindow.IsEnabled = true;
                        mainWindow.TitleBar.ShowMinimize = true;
                        mainWindow.TitleBar.ShowMaximize = true;
                        mainWindow.TitleBar.ShowClose = true;

                        mainWindow.Activate();

                        _ = mainWindow.NavigationView.Navigate(typeof(DashBoardPage));

                        mainWindow.Notify(
                            title: "Connection is successfully established",
                            message: $"id: {Id}, EndPoint: {Address}:{Port}",
                            seconds:3.0,
                            appearance: ControlAppearance.Success);
                    }
                });
            }
            catch (Exception e)
            {
                Application.Current.Dispatcher.InvokeAsync(() =>
                {
                    LoginWindow?.ShowErrorDialogAsync($"{e.Message}");
                });
            }
        });
    }

    public void OnDisconnected(DisconnectionEventArgs args)
    {
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            _isPendingConnection = false;
            _isConnected = false;
            OnPropertyChanged(nameof(IsActive));
        });
    }

    [RelayCommand]
    private void OnSaveButtonClicked(object _)
    {
        ConnectionSetting connectionSetting = _serviceProvider.GetRequired<SettingsProvider>().ConnectionSetting;
        connectionSetting.Address = Address;
        connectionSetting.Port = Port;
        connectionSetting.Id = Id;
    }
}
