using Sunlight.ManagementStudio.ViewModels.Pages;
using Wpf.Ui.Abstractions.Controls;

namespace Sunlight.ManagementStudio.Views.Pages;

public partial class AccountPage : INavigableView<AccountPageViewModel>
{
    public AccountPageViewModel ViewModel { get; init; }

    public AccountPage(AccountPageViewModel viewModel)
    {
        ViewModel = viewModel;
        DataContext = this;

        InitializeComponent();
    }
}
