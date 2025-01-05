using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Sunlight.ManagementStudio.Models.Event;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Data;
using Sunlight.ManagementStudio.Models.Event.Args;
using static System.Reflection.Metadata.BlobBuilder;

namespace Sunlight.ManagementStudio.ViewModels.Pages.LogViewer
{
    public partial class LogViewerViewModel : ObservableObject
    {
        private ObservableCollection<LogItem> _logs = new();

        private bool _showDebug = false;
        private bool _showInfo = true;
        private bool _showWarning = true;
        private bool _showError = true;
        private bool _showCritical = true;

        public ICollectionView Logs { get; init; }

        public bool ShowDebug
        {
            get => _showDebug;
            set
            {
                if (_showDebug != value)
                {
                    _showDebug = value;

                    OnPropertyChanged();

                    Logs.Refresh();
                }
            }
        }
        public bool ShowInfo
        {
            get => _showInfo;
            set
            {
                if (_showInfo != value)
                {
                    _showInfo = value;

                    OnPropertyChanged();

                    Logs.Refresh();
                }
            }
        }
        public bool ShowWarning
        {
            get => _showWarning;
            set
            {
                if (_showWarning != value)
                {
                    _showWarning = value;

                    OnPropertyChanged();

                    Logs.Refresh();
                }
            }
        }
        public bool ShowError
        {
            get => _showError;
            set
            {
                if (_showError != value)
                {
                    _showError = value;

                    OnPropertyChanged();

                    Logs.Refresh();
                }
            }
        }
        public bool ShowCritical
        {
            get => _showCritical;
            set
            {
                if (_showCritical != value)
                {
                    _showCritical = value;

                    OnPropertyChanged();

                    Logs.Refresh();
                }
            }
        }
        
        public LogViewerViewModel()
        {
            Logs = CollectionViewSource.GetDefaultView(_logs);
            Logs.Filter = Filter;
        }

        public void Add(IEnumerable<LogItem> items)
        {
            foreach (LogItem logItem in items)
            {
                _logs.Add(logItem);
            }

            Logs.Refresh();
        }

        private bool Filter(object obj)
        {
            if (obj is not LogItem logItem)
            {
                return false;
            }

            switch (logItem.LogLevel)
            {
                case LogLevel.Debug:
                    return ShowDebug;
                case LogLevel.Info:
                    return ShowInfo;
                case LogLevel.Warning:
                    return ShowWarning;
                case LogLevel.Error:
                    return ShowError;
                case LogLevel.Critical:
                    return ShowCritical;
                default:
                    return false;
            }
        }

        [RelayCommand]
        private void OnLogConsoleCloseButtonClicked(object sender)
        {
            _logs.Clear();
            Logs.Refresh();
        }
    }
}
