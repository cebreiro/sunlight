using System.Windows.Controls;
using Sunlight.ManagementStudio.ViewModels.Pages;
using Wpf.Ui.Abstractions.Controls;

namespace Sunlight.ManagementStudio.Views.Pages;

public partial class WorldPage : INavigableView<WorldPageViewModel>
{
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

            first = false;

            WorldListPanel.Children.Add(radioButton);
        }

        ViewModel.OnWorldSelected(0);
    }

    private void Reset()
    {
        WorldListPanel.Children.Clear();
    }
}
