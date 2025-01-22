namespace Sunlight.ManagementStudio.ViewModels.Pages.World;

public record struct ZoneListViewItem(int Id, ushort Port, string Name)
{
    public int Id { get; init; } = Id;
    public ushort Port { get; init; } = Port;
    public string Name { get; init; } = Name;

}
