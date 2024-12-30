namespace Sunlight.ManagementStudio.ViewModels.Pages.LogViewer;

public record struct LogItem(DateTime DateTime, LogLevel LogLevel, string Message)
{
    public DateTime DateTime { get; init; } = DateTime;
    public LogLevel LogLevel { get; init; } = LogLevel;
    public string Message { get; init; } = Message;
}
