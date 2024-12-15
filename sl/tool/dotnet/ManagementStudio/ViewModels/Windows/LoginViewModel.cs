using System.Net;
using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Models.Controller;
using Sunlight.ManagementStudio.Views.Windows;

namespace Sunlight.ManagementStudio.ViewModels.Windows;

public partial class LoginViewModel(IServiceProvider serviceProvider, ISunlightController sunlightController)
    : ObservableObject
{
    [ObservableProperty]
    private bool _isPendingConnection = false;
    public string Address { get; set; } = "127.0.0.1";
    public string Port { get; set; } = "8989";
    public string Id { get; set; } = string.Empty;

    [RelayCommand]
    private void OnConnectButtonClicked(object sender)
    {
        if (sender is not LoginWindow loginWindow)
        {
            return;
        }

        IsPendingConnection = true;

        string password = loginWindow.PasswordBox.Password;

        if (!IPAddress.TryParse(Address, out _))
        {
            MessageBox.Show(serviceProvider.GetRequiredService<MainWindow>(), "invalid ip address", "error");

            return;
        }

        Task.Run(async () =>
        {
            bool connect = await sunlightController.Connect(Address, ushort.Parse(Port));

            if (!connect)
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    IsPendingConnection = false;

                    MessageBox.Show(serviceProvider.GetRequiredService<MainWindow>(), "fail to connect", "error");
                });

                return;
            }

            AuthenticationRequest request = new();
            request.Id = Id;
            request.Password = password;

            AuthenticationResponse response = await sunlightController.Authenticate(request);

            Application.Current.Dispatcher.Invoke(() =>
            {
                IsPendingConnection = false;

                if (response.Success != 0)
                {
                    MessageBox.Show(serviceProvider.GetRequiredService<MainWindow>(), "auth success", "error");
                }
                else
                {
                    MessageBox.Show(serviceProvider.GetRequiredService<MainWindow>(), "auth fail", "error");
                }
            });
        });
    }
}
