using System.Windows;
using System.Windows.Media;
using CommunityToolkit.Mvvm.ComponentModel;
using Wpf.Ui.Abstractions.Controls;
using Wpf.Ui.Appearance;

namespace Sunlight.ManagementStudio.ViewModels.Pages;

public sealed partial class SettingsViewModel : ObservableObject, INavigationAware
{
    private bool _isInitialized = false;

    [ObservableProperty]
    private ApplicationTheme _currentApplicationTheme = ApplicationTheme.Unknown;

    public async Task OnNavigatedToAsync()
    {
        using CancellationTokenSource cts = new();

        await DispatchAsync(OnNavigatedTo, cts.Token);
    }

    public void OnNavigatedTo()
    {
        if (!_isInitialized)
        {
            _isInitialized = true;

            CurrentApplicationTheme = ApplicationThemeManager.GetAppTheme();

            ApplicationThemeManager.Changed += OnThemeChanged;
        }
    }

    public async Task OnNavigatedFromAsync()
    {
        using CancellationTokenSource cts = new();

        await DispatchAsync(OnNavigatedFrom, cts.Token);
    }

    public void OnNavigatedFrom()
    {

    }

    private static async Task DispatchAsync(Action action, CancellationToken cancellationToken)
    {
        if (cancellationToken.IsCancellationRequested)
        {
            return;
        }

        await Application.Current.Dispatcher.InvokeAsync(action);
    }

    partial void OnCurrentApplicationThemeChanged(ApplicationTheme oldValue, ApplicationTheme newValue)
    {
        ApplicationThemeManager.Apply(newValue);
    }

    private void OnThemeChanged(ApplicationTheme currentApplicationTheme, Color systemAccent)
    {
        if (CurrentApplicationTheme != currentApplicationTheme)
        {
            CurrentApplicationTheme = currentApplicationTheme;
        }
    }
}
