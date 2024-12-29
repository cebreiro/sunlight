using Sunlight.ManagementStudio.Models.Event.Args;

namespace Sunlight.ManagementStudio.Models.Event;

public class EventManager : IEventProducer, IEventListener
{
    private readonly EventDictionary<Action<AuthenticationEventArgs>> _authenticationSubscribers = new();
    private readonly EventDictionary<Action<DisconnectionEventArgs>> _disconnectionSubscribers = new();

    public long Listen(Action<AuthenticationEventArgs> action)
    {
        return _authenticationSubscribers.Add(action);
    }

    public long Listen(Action<DisconnectionEventArgs> action)
    {
        return _disconnectionSubscribers.Add(action);
    }

    public void Produce(AuthenticationEventArgs args)
    {
        _authenticationSubscribers.Visit((subscriber) =>
        {
            subscriber.Invoke(args);
        });
    }

    public void Produce(DisconnectionEventArgs args)
    {
        _disconnectionSubscribers.Visit((subscriber) =>
        {
            subscriber.Invoke(args);
        });
    }
}
