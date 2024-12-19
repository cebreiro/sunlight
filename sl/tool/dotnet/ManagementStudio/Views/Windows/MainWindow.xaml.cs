using Sunlight.ManagementStudio.ViewModels.Windows;
using Wpf.Ui;
using Wpf.Ui.Appearance;
using Wpf.Ui.Controls;

namespace Sunlight.ManagementStudio.Views.Windows;

public partial class MainWindow
{
    private readonly ISnackbarService _snackBarService;

    public MainWindowViewModel ViewModel { get; init; }

    public MainWindow(MainWindowViewModel viewModel,
        INavigationService navigationService,
        ISnackbarService snackBarService,
        IContentDialogService contentDialogService)
    {
        SystemThemeWatcher.Watch(this);

        _snackBarService = snackBarService;

        ViewModel = viewModel;
        DataContext = this;

        InitializeComponent();

        snackBarService.SetSnackbarPresenter(SnackBarPresenter);
        navigationService.SetNavigationControl(NavigationView);
        contentDialogService.SetDialogHost(RootContentDialog);
    }

    public void Notify(string title, string message, double seconds, ControlAppearance appearance)
    {
        _snackBarService.Show(title, message, appearance, null, TimeSpan.FromSeconds(seconds));
    }
}
