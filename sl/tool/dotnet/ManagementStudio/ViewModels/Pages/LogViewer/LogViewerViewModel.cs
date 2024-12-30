using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Collections.ObjectModel;

namespace Sunlight.ManagementStudio.ViewModels.Pages.LogViewer
{
    public partial class LogViewerViewModel : ObservableObject
    {
        [ObservableProperty]
        private ObservableCollection<LogItem> _logs = new();
        public LogViewerViewModel()
        {
            Logs.Add(new LogItem(DateTime.Now, LogLevel.Critical, "test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0test0"));
            Logs.Add(new LogItem(DateTime.Now, LogLevel.Debug, "test1"));
            Logs.Add(new LogItem(DateTime.Now, LogLevel.Info, "test2"));
            Logs.Add(new LogItem(DateTime.Now, LogLevel.Warning, "test3"));
            Logs.Add(new LogItem(DateTime.Now, LogLevel.Error, "test4"));
        }

        [RelayCommand]
        private void OnLogConsoleCloseButtonClicked(object sender)
        {
            Logs.Clear();
        }
    }
}
