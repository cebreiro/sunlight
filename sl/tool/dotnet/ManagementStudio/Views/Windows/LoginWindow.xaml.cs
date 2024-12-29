using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using Sunlight.ManagementStudio.ViewModels.Windows;
using Wpf.Ui.Appearance;
using Wpf.Ui.Controls;
using TextBox = System.Windows.Controls.TextBox;

namespace Sunlight.ManagementStudio.Views.Windows;

public partial class LoginWindow
{
    public LoginViewModel ViewModel { get; init; }

    public LoginWindow(LoginViewModel viewModel, IServiceProvider serviceProvider)
    {
        ViewModel = viewModel;
        DataContext = this;

        viewModel.LoginWindow = this;

        InitializeComponent();
    }

    public async Task ShowErrorDialogAsync(string content)
    {
        Wpf.Ui.Controls.MessageBox messageBox = new();
        messageBox.Title = "Error";
        messageBox.Margin = new Thickness(20.0, 20.0, 5.0, 50.0);
        messageBox.Content = content;
        messageBox.CloseButtonText = "Ok";
        messageBox.Owner = this;
        messageBox.WindowStartupLocation = WindowStartupLocation.CenterOwner;

        _ = await messageBox.ShowDialogAsync();
    }

    private void LoginWindow_OnInitialized(object? sender, EventArgs e)
    {
        ApplicationThemeManager.Changed += OnApplicationThemeChanged;
    }

    private void LoginWindow_OnClosed(object? sender, EventArgs e)
    {
        ApplicationThemeManager.Changed -= OnApplicationThemeChanged;

        Application.Current.Shutdown();
    }

    private void OnApplicationThemeChanged(ApplicationTheme theme, Color systemAccent)
    {
        WindowBackgroundManager.UpdateBackground(this, theme, WindowBackdropType.Mica);
    }

    private void OnPortTextBoxPreviewTextInput(object sender, TextCompositionEventArgs e)
    {
        if (sender is not TextBox textBox)
        {
            return;
        }

        if (int.TryParse(e.Text, out int inputValue))
        {
            int textBoxValue = 0;

            if (string.IsNullOrEmpty(textBox.Text) || int.TryParse(textBox.Text, out textBoxValue))
            {
                int newValue = textBoxValue * 10 + inputValue;

                if (newValue > 0 && newValue <= ushort.MaxValue)
                {
                    e.Handled = false;

                    return;
                }
            }
        }

        e.Handled = true;
    }
}
