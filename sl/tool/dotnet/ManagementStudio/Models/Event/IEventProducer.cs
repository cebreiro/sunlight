using Sunlight.ManagementStudio.Models.Event.Args;

namespace Sunlight.ManagementStudio.Models.Event;

public interface IEventProducer
{
    void Produce(AuthenticationEventArgs args);
    void Produce(DisconnectionEventArgs args);
}
