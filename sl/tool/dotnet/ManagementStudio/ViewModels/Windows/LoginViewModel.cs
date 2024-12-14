using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.VisualBasic.CompilerServices;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Views.Windows;

namespace Sunlight.ManagementStudio.ViewModels.Windows;

public partial class LoginViewModel(IServiceProvider serviceProvider) : ObservableObject
{
    [ObservableProperty]
    private bool _isPendingConnection = false;
    public string Ip { get; set; } = string.Empty;
    public string Port { get; set; } = string.Empty;
    public string Id { get; set; } = string.Empty;

    [RelayCommand]
    private void OnConnectButtonClicked(object sender)
    {
        if (sender is not LoginWindow loginWindow)
        {
            return;
        }

        IsPendingConnection = true;

        MessageBox.Show(serviceProvider.GetRequiredService<MainWindow>(), $"ip: {Ip}, Port: {Port}, Id: {Id}, Password: {loginWindow.PasswordBox.Password}, ", "debug");
    }
}
