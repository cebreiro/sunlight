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
    }
}
