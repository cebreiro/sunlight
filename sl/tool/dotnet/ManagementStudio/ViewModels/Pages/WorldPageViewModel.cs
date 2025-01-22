using System.Collections.ObjectModel;
using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using Sunlight.ManagementStudio.ViewModels.Pages.World;
using Wpf.Ui.Abstractions.Controls;

namespace Sunlight.ManagementStudio.ViewModels.Pages;

public partial class WorldPageViewModel : ObservableObject, INavigationAware
{
    private bool _isInitialized = false;

    [ObservableProperty]
    private ObservableCollection<ZoneListViewItem> _zoneList = new();

    public WorldPageViewModel()
    {
        ZoneList.Add(new ZoneListViewItem(101, 1234, "test1"));
        ZoneList.Add(new ZoneListViewItem(102, 5678, "test2"));
    }

    public Task OnNavigatedToAsync()
    {
        return Task.CompletedTask;
        //await Application.Current.Dispatcher.InvokeAsync(Initialize);
    }

    public Task OnNavigatedFromAsync()
    {
        return Task.CompletedTask;
    }

    private async Task Initialize()
    {
        if (_isInitialized)
        {
            return;
        }

        _isInitialized = true;

        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            
        });
    }
}
