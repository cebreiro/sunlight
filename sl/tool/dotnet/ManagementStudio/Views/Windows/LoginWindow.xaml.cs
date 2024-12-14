using System.Windows.Media;
using Sunlight.ManagementStudio.ViewModels.Windows;
using Wpf.Ui.Appearance;
using Wpf.Ui.Controls;

namespace Sunlight.ManagementStudio.Views.Windows;

public partial class LoginWindow
{
    public LoginViewModel ViewModel { get; init; }

    public LoginWindow(LoginViewModel viewModel, IServiceProvider serviceProvider)
    {
        ViewModel = viewModel;
        DataContext = this;

        InitializeComponent();
    }

    private void LoginWindow_OnInitialized(object? sender, EventArgs e)
    {
        ApplicationThemeManager.Changed += OnApplicationThemeChanged;
    }

    private void LoginWindow_OnClosed(object? sender, EventArgs e)
    {
        ApplicationThemeManager.Changed -= OnApplicationThemeChanged;
    }

    private void OnApplicationThemeChanged(ApplicationTheme theme, Color systemAccent)
    {
        WindowBackgroundManager.UpdateBackground(this, theme, WindowBackdropType.Mica);
    }
}
