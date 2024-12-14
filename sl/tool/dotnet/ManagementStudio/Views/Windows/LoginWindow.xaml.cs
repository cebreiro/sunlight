using System.Windows.Media;
using Sunlight.ManagementStudio.ViewModels.Windows;
using Wpf.Ui.Appearance;
using Wpf.Ui.Controls;

namespace Sunlight.ManagementStudio.Views.Windows;

public partial class LoginWindow
{
    private bool _isThemeChangeSubscribed = false;

    public LoginViewModel ViewModel { get; init; }

    public LoginWindow(LoginViewModel viewModel, IServiceProvider serviceProvider)
    {
        ViewModel = viewModel;
        DataContext = this;

        InitializeComponent();

        SystemThemeWatcher.Watch(this);
    }

    private void LoginWindow_OnActivated(object? sender, EventArgs e)
    {
        if (!_isThemeChangeSubscribed)
        {
            _isThemeChangeSubscribed = true;

            ApplicationThemeManager.Changed += OnApplicationThemeChanged;
        }
    }

    private void LoginWindow_OnClosed(object? sender, EventArgs e)
    {
        if (_isThemeChangeSubscribed)
        {
            _isThemeChangeSubscribed = false;

            ApplicationThemeManager.Changed -= OnApplicationThemeChanged;
        }
    }

    private void OnApplicationThemeChanged(ApplicationTheme theme, Color systemAccent)
    {
        System.Diagnostics.Debug.Assert(_isThemeChangeSubscribed);

        WindowBackgroundManager.UpdateBackground(this, theme, WindowBackdropType.Mica);
    }
}
