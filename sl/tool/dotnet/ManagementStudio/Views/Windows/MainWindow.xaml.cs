using Sunlight.ManagementStudio.ViewModels.Windows;
using Wpf.Ui;
using Wpf.Ui.Appearance;
using Wpf.Ui.Controls;

namespace Sunlight.ManagementStudio.Views.Windows;

public partial class MainWindow
{
    public MainWindowViewModel ViewModel { get; init; }

    public MainWindow(MainWindowViewModel viewModel,
        INavigationService navigationService,
        ISnackbarService snackBarService,
        IContentDialogService contentDialogService)
    {
        ViewModel = viewModel;
        DataContext = this;

        InitializeComponent();

        snackBarService.SetSnackbarPresenter(SnackBarPresenter);
        navigationService.SetNavigationControl(NavigationView);
        contentDialogService.SetDialogHost(RootContentDialog);

        SystemThemeWatcher.Watch(this);
    }
}
