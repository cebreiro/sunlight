using Sunlight.ManagementStudio.ViewModels.Pages;
using Wpf.Ui.Abstractions.Controls;

namespace Sunlight.ManagementStudio.Views.Pages;

public partial class DashBoardPage : INavigableView<DashBoardPageViewModel>
{
    public DashBoardPageViewModel ViewModel { get; }

    public DashBoardPage(DashBoardPageViewModel viewModel)
    {
        ViewModel = viewModel;
        DataContext = this;

        ViewModel.LogViewer.Logs.CollectionChanged += (s, e) =>
        {
            LogScrollViewer.ScrollToEnd();
        };

        InitializeComponent();
    }
}
