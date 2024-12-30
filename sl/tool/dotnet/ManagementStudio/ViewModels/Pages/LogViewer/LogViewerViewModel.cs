using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Sunlight.ManagementStudio.Models.Event;
using System.Collections.ObjectModel;
using Sunlight.ManagementStudio.Models.Event.Args;

namespace Sunlight.ManagementStudio.ViewModels.Pages.LogViewer
{
    public partial class LogViewerViewModel : ObservableObject
    {
        private long? _disconnectEventKey = null;

        [ObservableProperty]
        private ObservableCollection<LogItem> _logs = new();
        public LogViewerViewModel()
        {
        }

        [RelayCommand]
        private void OnLogConsoleCloseButtonClicked(object sender)
        {
            Logs.Clear();
        }
    }
}
