using CommunityToolkit.Mvvm.ComponentModel;
using Microsoft.VisualBasic.CompilerServices;

namespace Sunlight.ManagementStudio.ViewModels.Windows;

public class LoginViewModel : ObservableObject
{
    public string Address { get; set; } = string.Empty;
    public string Port { get; set; } = string.Empty;
    public string Id { get; set; } = string.Empty;
    public string Pw { get; set; } = string.Empty;
}
