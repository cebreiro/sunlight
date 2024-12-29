using Sunlight.ManagementStudio.Models.Event.Args;

namespace Sunlight.ManagementStudio.Models.Event;

public interface IEventListener
{
    public long Listen(Action<AuthenticationEventArgs> action);
    public long Listen(Action<DisconnectionEventArgs> action);
}
