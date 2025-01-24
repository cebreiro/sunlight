using System.Windows;
using System.Windows.Controls;
using Sunlight.ManagementStudio.ViewModels.Pages;
using Wpf.Ui.Abstractions.Controls;

namespace Sunlight.ManagementStudio.Views.Pages;

public partial class WorldPage : INavigableView<WorldPageViewModel>
{
    private readonly Dictionary<object, string> _worldKeys = new();

    public WorldPageViewModel ViewModel { get; }

    public WorldPage(WorldPageViewModel viewModel)
    {
        DataContext = this;
        ViewModel = viewModel;

        InitializeComponent();

        ViewModel.InitializeHandler = Initialize;
        ViewModel.ResetHandler = Reset;
    }

    private void Initialize(List<string> worldIds)
    {
        WorldListPanel.Children.Clear();

        bool first = true;

        foreach (string id in worldIds)
        {
            RadioButton radioButton = new()
            {
                GroupName = "WorldPageWorldRadioButtonGroup",
                IsChecked = first,
                Content = $"World {id}",
            };

            radioButton.Checked += OnWorldRadioButtonClicked;

            first = false;

            _worldKeys.Add(radioButton, id);
            WorldListPanel.Children.Add(radioButton);
        }

        ViewModel.OnWorldSelected(worldIds[0]);
    }

    private void Reset()
    {
        _worldKeys.Clear();
        WorldListPanel.Children.Clear();
    }

    private void OnWorldRadioButtonClicked(object sender, RoutedEventArgs e)
    {
        if (sender is not RadioButton radioButton)
        {
            return;
        }

        if (!_worldKeys.TryGetValue(radioButton, out var key))
        {
            System.Diagnostics.Debug.Assert(false);

            return;
        }

        ViewModel.OnWorldSelected(key);
    }
}
