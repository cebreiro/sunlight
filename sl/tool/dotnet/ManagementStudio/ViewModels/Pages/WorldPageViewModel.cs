using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using Sunlight.Api;
using Sunlight.ManagementStudio.Models.Controller;
using Sunlight.ManagementStudio.Models.Event;
using Sunlight.ManagementStudio.Models.Event.Args;
using Sunlight.ManagementStudio.ViewModels.Pages.World;
using Wpf.Ui.Abstractions.Controls;

namespace Sunlight.ManagementStudio.ViewModels.Pages;

public partial class WorldPageViewModel : ObservableObject, INavigationAware
{
    private bool _initialized = false;
    private readonly ISunlightController _sunlightController;
    private readonly Dictionary<string, WorldInfo> _worldInfo = new();

    public Action<List<string>>? InitializeHandler { get; set; }
    public Action? ResetHandler { get; set; }

    [ObservableProperty]
    private ObservableCollection<ZoneListViewItem> _zoneList = new();

    public WorldPageViewModel(ISunlightController sunlightController, IEventListener eventListener)
    {
        _sunlightController = sunlightController;

        eventListener.Listen(OnDisconnect);
    }

    public Task OnNavigatedToAsync()
    {
        Initialize();

        return Task.CompletedTask;
    }

    public Task OnNavigatedFromAsync()
    {
        return Task.CompletedTask;
    }

    public void OnWorldSelected(string worldId)
    {
        if (!_worldInfo.TryGetValue(worldId, out var worldInfo))
        {
            System.Diagnostics.Debug.Assert(false);

            return;
        }

        ZoneList.Clear();

        Encoding encoding = Encoding.GetEncoding("EUC-KR");

        foreach (ZoneInfo? zoneInfo in worldInfo.OpenZoneList.OrderBy(info => info.Id))
        {
            if (zoneInfo == null)
            {
                continue;
            }

            string name = encoding.GetString(zoneInfo.Name.ToByteArray());

            ZoneList.Add(new ZoneListViewItem(zoneInfo.Id, (ushort)zoneInfo.Port, name));
        }
    }

    private void Initialize()
    {
        if (_initialized)
        {
            return;
        }

        _initialized = true;

        Task.Run(async () =>
        {
            WorldInfoResponse response = await _sunlightController.GetWorldInfo(new WorldInfoRequest());

            Application.Current.Dispatcher.InvokeAsync(() =>
            {
                foreach (WorldInfo? worldInfo in response.WorldInfoList)
                {
                    if (worldInfo == null)
                    {
                        System.Diagnostics.Debug.Assert(false);

                        continue;
                    }

                    _worldInfo.Add(worldInfo.Id.ToString(), worldInfo);
                }

                if (InitializeHandler != null)
                {
                    var worldIds = _worldInfo.Values.Select((worldInfo) => worldInfo.Id.ToString()).ToList();

                    InitializeHandler(worldIds);
                }
            });
        });
    }

    private void OnDisconnect(DisconnectionEventArgs args)
    {
        if (!_initialized)
        {
            return;
        }

        _initialized = false;
        _worldInfo.Clear();

        ResetHandler?.Invoke();
    }
}
