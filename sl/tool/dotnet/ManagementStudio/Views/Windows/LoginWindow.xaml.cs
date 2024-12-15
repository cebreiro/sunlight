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
