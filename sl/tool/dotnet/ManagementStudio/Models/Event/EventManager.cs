using Sunlight.ManagementStudio.Models.Event.Args;

namespace Sunlight.ManagementStudio.Models.Event;

public class EventManager : IEventProducer, IEventListener
{
    private readonly EventDictionary<Action<DisconnectionEventArgs>> _disconnectionSubscribers = new();

    public long Listen(Action<DisconnectionEventArgs> action)
    {
        return _disconnectionSubscribers.Add(action);
    }

    public void RemoveDisconnectionEventListener(long key)
    {
        bool removed = _disconnectionSubscribers.TryRemove(key, out _);

        System.Diagnostics.Debug.Assert(removed);
    }

    public void Produce(DisconnectionEventArgs args)
    {
        _disconnectionSubscribers.Visit((subscriber) =>
        {
            subscriber.Invoke(args);
        });
    }
}
