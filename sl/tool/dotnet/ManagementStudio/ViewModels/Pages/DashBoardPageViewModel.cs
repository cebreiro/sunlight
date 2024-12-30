using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using LiveChartsCore;
using LiveChartsCore.SkiaSharpView.Extensions;
using Sunlight.Api;
using Sunlight.ManagementStudio.Models.Controller;
using Sunlight.ManagementStudio.Models.Event;
using Sunlight.ManagementStudio.Models.Event.Args;
using Sunlight.ManagementStudio.ViewModels.Pages.LogViewer;
using Wpf.Ui.Controls;
using static System.Reflection.Metadata.BlobBuilder;

namespace Sunlight.ManagementStudio.ViewModels.Pages;

public partial class DashBoardPageViewModel(ISunlightController sunlightController) : ObservableObject
{
    enum UpdateType
    {
        UserCount,
        SystemInfo,
    }

    class UpdateContext
    {
        public int interval { get; set; } = 10;
        public CancellationTokenSource? Cts { get; set; } = null;
    }

    private Dictionary<UpdateType, UpdateContext> _updateContexts = new();

    [ObservableProperty]
    private string _userCount = "0";

    [ObservableProperty]
    private string _freeMemoryGB = "0";

    public IEnumerable<ISeries> CpuLoadSeries { get; set; } =
        GaugeGenerator.BuildSolidGauge(
            new GaugeItem(
                -1,
                series =>
                {
                    series.MaxRadialColumnWidth = 50;
                    series.DataLabelsSize = 50;
                }));

    [ObservableProperty]
    private ObservableCollection<LogItem> _logs = new ObservableCollection<LogItem>();

    [RelayCommand]
    private void OnLogConsoleCloseButtonClicked(object sender)
    {

        Logs.Clear();
    }

    public void ListenEvent(IEventListener eventListener)
    {
        _ = eventListener.Listen(OnAuthenticated);
        _ = eventListener.Listen(OnDisconnect);
    }

    private async Task RunUserCountRequest()
    {
        UserCountRequest request = new();

        UpdateContext context = _updateContexts[UpdateType.UserCount];
        Debug.Assert(context.Cts != null);

        while (!context.Cts.IsCancellationRequested)
        {
            UserCountResponse response = await sunlightController.GetUserCount(request);

            Application.Current.Dispatcher.InvokeAsync(() =>
            {
                Logs.Add(new LogItem(DateTime.Now, LogLevel.Critical, "test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0"));
                Logs.Add(new LogItem(DateTime.Now, LogLevel.Debug, "test1"));
                Logs.Add(new LogItem(DateTime.Now, LogLevel.Info, "test2"));
                Logs.Add(new LogItem(DateTime.Now, LogLevel.Warning, "test3"));
                Logs.Add(new LogItem(DateTime.Now, LogLevel.Error, "test4"));

                UserCount = response.UserCount.ToString();
            });

            await Task.Delay(TimeSpan.FromSeconds(context.interval));
        }
    }

    private async Task RunSystemInfoRequest()
    {
        SystemResourceInfoRequest request = new();

        UpdateContext context = _updateContexts[UpdateType.SystemInfo];
        Debug.Assert(context.Cts != null);

        while (!context.Cts.IsCancellationRequested)
        {
            SystemResourceInfoResponse response = await sunlightController.GetSystemInfo(request);

            Application.Current.Dispatcher.InvokeAsync(() =>
            {
                CpuLoadSeries = GaugeGenerator.BuildSolidGauge(
                    new GaugeItem(response.CpuLoadPercentage,
                        series =>
                        {
                            series.MaxRadialColumnWidth = 50;
                            series.DataLabelsSize = 50;
                        }));

                OnPropertyChanged(nameof(CpuLoadSeries));

                FreeMemoryGB = $"{response.FreeMemoryGb:F2}";
            });

            await Task.Delay(TimeSpan.FromSeconds(context.interval));
        }
    }

    public void OnAuthenticated(AuthenticationEventArgs args)
    {
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            System.Diagnostics.Debug.Assert(!_updateContexts.ContainsKey(UpdateType.UserCount));
            System.Diagnostics.Debug.Assert(!_updateContexts.ContainsKey(UpdateType.SystemInfo));

            _updateContexts.Add(UpdateType.UserCount, new UpdateContext()
            {
                Cts = new CancellationTokenSource(),
            });

            _updateContexts.Add(UpdateType.SystemInfo, new UpdateContext()
            {
                interval = 30,
                Cts = new CancellationTokenSource(),
            });


            Task.Run(RunUserCountRequest);
            Task.Run(RunSystemInfoRequest);
        });
    }

    public void OnDisconnect(DisconnectionEventArgs args)
    {
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            System.Diagnostics.Debug.Assert(_updateContexts.ContainsKey(UpdateType.UserCount));
            System.Diagnostics.Debug.Assert(_updateContexts.ContainsKey(UpdateType.SystemInfo));


            _updateContexts[UpdateType.UserCount].Cts?.Cancel();
            _updateContexts.Remove(UpdateType.UserCount);

            _updateContexts[UpdateType.SystemInfo].Cts?.Cancel();
            _updateContexts.Remove(UpdateType.SystemInfo);

        });
    }
}
