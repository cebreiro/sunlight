using System.Text;
using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using LiveChartsCore;
using LiveChartsCore.SkiaSharpView.Extensions;
using Sunlight.Api;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Models.Controller;
using Sunlight.ManagementStudio.Models.Event;
using Sunlight.ManagementStudio.Models.Event.Args;
using Sunlight.ManagementStudio.ViewModels.Pages.LogViewer;
using Sunlight.ManagementStudio.Views.Pages;
using Sunlight.ManagementStudio.Views.Windows;
using Wpf.Ui.Controls;
using LogItem = Sunlight.ManagementStudio.ViewModels.Pages.LogViewer.LogItem;

namespace Sunlight.ManagementStudio.ViewModels.Pages;

public partial class DashBoardPageViewModel(IServiceProvider serviceProvider, ISunlightController sunlightController) : ObservableObject
{
    enum UpdateType
    {
        UserCount,
        SystemInfo,
        LogViewer,
    }

    class UpdateContext
    {
        public int Interval { get; set; } = 10;
        public CancellationTokenSource Cts { get; set; } = null;
        public Func<UpdateContext, Task> Func { get; set; } = null;
    }

    private Dictionary<UpdateType, UpdateContext> _updateContexts = new();
    private long? _lastLogUpdateTime = null;

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

    public LogViewerViewModel LogViewer { get; } = serviceProvider.GetRequired<LogViewerViewModel>();

    public void ListenEvent(IEventListener eventListener)
    {
        _ = eventListener.Listen(OnAuthenticated);
        _ = eventListener.Listen(OnDisconnect);
    }

    private async Task RunUserCountRequest(UpdateContext context)
    {
        UserCountRequest request = new();

        while (!context.Cts.IsCancellationRequested)
        {
            UserCountResponse response = await sunlightController.GetUserCount(request);

            Application.Current.Dispatcher.InvokeAsync(() =>
            {
                UserCount = response.UserCount.ToString();
            });

            await Task.Delay(TimeSpan.FromSeconds(context.Interval));
        }
    }

    private async Task RunSystemInfoRequest(UpdateContext context)
    {
        SystemResourceInfoRequest request = new();

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

            await Task.Delay(TimeSpan.FromSeconds(context.Interval));
        }
    }

    private async Task RunLogGetRequest(UpdateContext context)
    {
        LogLevel[] logLevels = [LogLevel.Debug, LogLevel.Info, LogLevel.Warning, LogLevel.Error, LogLevel.Critical];

        LogGetRequest request = new();
        request.LogLevel.Add(logLevels.Select(level => (int)level));

        while (!context.Cts.IsCancellationRequested)
        {
            if (_lastLogUpdateTime != null)
            {
                request.StartDateTime = (long)(_lastLogUpdateTime + 1);
            }
            else
            {
                request.ClearStartDateTime();
            }

            LogGetResponse response = await sunlightController.GetLog(request);

            List<Api.LogItem> received = response.LogItemList.OrderBy(item => item.DateTime).ToList();

            if (received.Count > 0)
            {
                _lastLogUpdateTime = received[^1].DateTime;
            }

            Application.Current.Dispatcher.InvokeAsync(() =>
            {
                Encoding eucKrEncoding = Encoding.GetEncoding("EUC-KR");

                foreach (var receivedLog in received)
                {
                    LogViewer.Logs.Add(new LogItem()
                    {
                        DateTime = DateTimeOffset.FromUnixTimeMilliseconds((receivedLog.DateTime / 1000000)).DateTime,
                        LogLevel = (LogLevel)receivedLog.LogLevel,
                        Message = eucKrEncoding.GetString(receivedLog.Message.ToByteArray()),
                    });
                }
            });

            await Task.Delay(TimeSpan.FromSeconds(context.Interval));
        }
    }

    public void OnAuthenticated(AuthenticationEventArgs args)
    {
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            System.Diagnostics.Debug.Assert(_updateContexts.Count == 0);

            _updateContexts.Add(UpdateType.UserCount, new UpdateContext()
            {
                Cts = new CancellationTokenSource(),
                Func = RunUserCountRequest
            });

            _updateContexts.Add(UpdateType.SystemInfo, new UpdateContext()
            {
                Interval = 30,
                Cts = new CancellationTokenSource(),
                Func = RunSystemInfoRequest
            });

            _updateContexts.Add(UpdateType.LogViewer, new UpdateContext()
            {
                Interval = 10,
                Cts = new CancellationTokenSource(),
                Func = RunLogGetRequest
            });

            foreach (var (_, context) in _updateContexts)
            {
                Task.Run(async () =>
                {
                    try
                    {
                        await context.Func(context);
                    }
                    catch (Exception e)
                    {
                        serviceProvider.GetRequired<MainWindow>().Notify("Async Operation Error", e.Message, 5.0, ControlAppearance.Danger);
                    }
                });
            }
        });
    }

    public void OnDisconnect(DisconnectionEventArgs args)
    {
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            System.Diagnostics.Debug.Assert(_updateContexts.Count != 0);

            foreach (var (_, context) in _updateContexts)
            {
                context.Cts.Cancel();
            }

            _updateContexts.Clear();
        });
    }
}
