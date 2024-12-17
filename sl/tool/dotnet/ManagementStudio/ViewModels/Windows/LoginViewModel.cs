using System.CodeDom;
using System.Net;
using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Sunlight.Api;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Models.Controller;
using Sunlight.ManagementStudio.Models.Event;
using Sunlight.ManagementStudio.Models.Setting;
using Sunlight.ManagementStudio.Views.Windows;

namespace Sunlight.ManagementStudio.ViewModels.Windows;

public partial class LoginViewModel : ObservableObject, IDisposable
{
    private readonly IServiceProvider _serviceProvider;
    private readonly ISunlightController _sunlightController;
    private readonly IEventListener _eventListener;

    private long? _disconnectionEventKey = null;

    private bool _isPendingConnection = false;
    private bool _isConnected = false;

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

    public LoginViewModel(IServiceProvider serviceProvider, ISunlightController sunlightController, IEventListener eventListener)
    {
        _serviceProvider = serviceProvider;
        _sunlightController = sunlightController;
        _eventListener = eventListener;

        _disconnectionEventKey = _eventListener.Listen((args) =>
        {
            Application.Current.Dispatcher.Invoke(() =>
            {
                _isConnected = false;
                OnPropertyChanged(nameof(IsActive));
            });
        });

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

    ~LoginViewModel()
    {
        Dispose();
    }

    [RelayCommand]
    private void OnConnectButtonClicked(object sender)
    {
        if (sender is not LoginWindow loginWindow)
        {
            return;
        }

        _isPendingConnection = true;
        OnPropertyChanged(nameof(IsActive));

        string password = loginWindow.PasswordBox.Password;

        if (!IPAddress.TryParse(Address, out _))
        {
            MessageBox.Show(_serviceProvider.GetRequired<MainWindow>(), "invalid ip address", "error");

            return;
        }

        Task.Run(async () =>
        {
            try
            {
                bool connect = await _sunlightController.Connect(Address, ushort.Parse(Port));

                Application.Current.Dispatcher.Invoke(() =>
                {
                    _isPendingConnection = false;
                    _isConnected = connect;

                    OnPropertyChanged(nameof(IsActive));

                    MessageBox.Show(_serviceProvider.GetRequired<MainWindow>(), $"connected: {connect}", "error");
                });

                AuthenticationRequest request = new();
                request.Id = Id;
                request.Password = password;

                AuthenticationResponse response = await _sunlightController.Authenticate(request);

                Application.Current.Dispatcher.Invoke(() =>
                {
                    if (response.Success != 0)
                    {
                        MessageBox.Show(_serviceProvider.GetRequired<MainWindow>(), "auth success", "error");
                    }
                    else
                    {
                        MessageBox.Show(_serviceProvider.GetRequired<MainWindow>(), "auth fail", "error");
                    }
                });

            }
            catch (Exception e)
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    MessageBox.Show(_serviceProvider.GetRequired<MainWindow>(), $"{e.Message}", "error");
                });
            }
        });
    }

    [RelayCommand]
    private void OnSaveButtonClicked(object sender)
    {
        if (sender is not LoginWindow loginWindow)
        {
            return;
        }

        ConnectionSetting connectionSetting = _serviceProvider.GetRequired<SettingsProvider>().ConnectionSetting;
        connectionSetting.Address = Address;
        connectionSetting.Port = Port;
        connectionSetting.Id = Id;
    }

    public void Dispose()
    {
        if (_disconnectionEventKey == null)
        {
            return;
        }

        _eventListener.RemoveDisconnectionEventListener(_disconnectionEventKey.Value);

        _disconnectionEventKey = null;

        GC.SuppressFinalize(this);
    }
}
