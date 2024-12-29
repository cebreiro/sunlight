using System.Collections.Concurrent;

namespace Sunlight.ManagementStudio.Models.Event;

public class EventDictionary<T>
{
    private long _nextKey = 1;
    private readonly ConcurrentDictionary<long, T> _dictionary = new();

    public long Add(T item)
    {
        long key = Interlocked.Increment(ref _nextKey);

        _dictionary[key] = item;

        return key;
    }

    public bool TryRemove(long key, out T? item)
    {
        return _dictionary.TryRemove(key, out item);
    }

    public void Visit(Action<T> visitor)
    {
        foreach (var (_, item) in _dictionary)
        {
            visitor.Invoke(item);
        }
    }
}
