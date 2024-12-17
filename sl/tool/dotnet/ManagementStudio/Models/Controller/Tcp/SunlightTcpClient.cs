using System.Net;
using System.Net.Sockets;
using Google.Protobuf;
using Nito.AsyncEx;
using Sunlight.Api;

namespace Sunlight.ManagementStudio.Models.Controller.Tcp;

public class SunlightTcpClient : IDisposable
{
    private const int PacketHeadSize = 4;

    private volatile Socket? _socket = null;

    private AsyncLock _sendMutex = new();
    private byte[]? _sendPending = null;
    private int _sendPendingStartOffset = 0;
    private int _sendPendingSize = 0;
    private Queue<byte[]> _sendWaits = new();

    private byte[] _receiveBuffer = new byte[1024];
    private long _receiveBufferSize = 1024;
    private long _receiveSize = 0;

    private List<Response> _responses = new();

    private Action<Response>? _receiveHandler = null;
    private Action? _disconnectHandler = null;

    public bool IsConnected()
    {
        return _socket?.Connected ?? false;
    }

    public async Task<bool> ConnectAsync(string address, ushort port)
    {
        if (!IPAddress.TryParse(address, out IPAddress ipAddress))
        {
            return false;
        }

        Socket newSocket = new(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        Socket? old = Interlocked.CompareExchange(ref _socket, newSocket, null);
        if (old != null)
        {
            newSocket.Dispose();

            return false;
        }

        bool success = false;

        try
        {
            await _socket.ConnectAsync(ipAddress, port);

            _sendPending = null;
            _sendWaits.Clear();

            _receiveSize = 0;

            _responses.Clear();

            success = true;
        }
        catch
        {
            // ignored
        }
        finally
        {
            if (!success)
            {
                _socket.Dispose();

                _socket = null;
            }
        }

        return success;
    }

    void Disconnect()
    {
        _socket?.Disconnect(false);
    }

    public async Task SendAsync(Request request)
    {
        byte[] buffer = MakeBuffer(request);

        using (await _sendMutex.LockAsync())
        {
            if (!IsConnected())
            {
                throw new Exception("tcp client is not connected");
            }

            if (_sendPending == null)
            {
                StartSend(buffer, 0, buffer.Length);
            }
            else
            {
                _sendWaits.Enqueue(buffer);
            }
        }
    }

    public void StartReceive()
    {
        if (!IsConnected())
        {
            return;
        }

        _socket?.ReceiveAsync(new ArraySegment<byte>(_receiveBuffer, (int)_receiveSize, (int)_receiveBufferSize))
            .ContinueWith(async (task) =>
            {
                try
                {
                    int bytes = await task;
                    if (bytes != 0)
                    {
                        if (HandleReceived(bytes))
                        {
                            StartReceive();
                        }
                        else
                        {
                            Disconnect();
                            OnDisconnect();
                        }

                        StartReceive();

                        return;
                    }
                }
                catch
                {
                    // ignored
                }

                OnDisconnect();
            });
    }

    private void StartSend(byte[] buffer, int start, int size)
    {
        System.Diagnostics.Debug.Assert(_socket != null);
        System.Diagnostics.Debug.Assert(buffer != null);
        System.Diagnostics.Debug.Assert(_sendPending == null);
        System.Diagnostics.Debug.Assert(size > 0 && size <= buffer.Length);

        if (!IsConnected())
        {
            return;
        }

        _sendPending = buffer;
        _sendPendingStartOffset = start;
        _sendPendingSize = size;

        OnSend(_socket.SendAsync(new ReadOnlyMemory<byte>(_sendPending, start, size)));
    }

    private async Task OnSend(ValueTask<int> task)
    {
        try
        {
            int bytes = await task;

            using (await _sendMutex.LockAsync())
            {
                System.Diagnostics.Debug.Assert(_sendPending != null);
                System.Diagnostics.Debug.Assert(bytes <= _sendPending.Length);

                byte[] prevSendPending = _sendPending;
                int startOffset = _sendPendingStartOffset + bytes;
                int size = _sendPendingSize - bytes;

                _sendPending = null;

                if (bytes == prevSendPending.Length)
                {
                    System.Diagnostics.Debug.Assert(startOffset == bytes);
                    System.Diagnostics.Debug.Assert(size == 0);

                    if (_sendWaits.TryDequeue(out byte[] sendContinuation))
                    {
                        StartSend(sendContinuation, 0, sendContinuation.Length);
                    }
                }
                else
                {
                    StartSend(prevSendPending, startOffset, size);
                }
            }
        }
        catch 
        {
            OnDisconnect();
        }
    }

    private void OnDisconnect()
    {
        Socket? socket = Interlocked.CompareExchange(ref _socket, null, _socket);
        if (socket == null)
        {
            return;
        }

        socket.Dispose();

        _disconnectHandler?.Invoke();
    }

    private bool HandleReceived(long size)
    {
        long requiredBufferSize = _receiveSize + size;

        if (requiredBufferSize > _receiveBufferSize)
        {
            var newReceiveBuffer = new byte[requiredBufferSize];

            Array.Copy(_receiveBuffer, 0, newReceiveBuffer, 0, _receiveSize);

            _receiveBuffer = newReceiveBuffer;
            _receiveBufferSize = requiredBufferSize;
        }

        _receiveSize += size;

        while (_receiveSize > PacketHeadSize)
        {
            int packetSize = BitConverter.ToInt32(_receiveBuffer, 0);

            if (_receiveSize < packetSize)
            {
                break;
            }

            Response? response = Response.Parser.ParseFrom(new ReadOnlySpan<byte>(_receiveBuffer, PacketHeadSize, packetSize - 4));
            if (response == null)
            {
                return false;
            }

            _responses.Add(response);    

            if (_receiveSize > packetSize)
            {
                Array.Copy(_receiveBuffer, packetSize, _receiveBuffer, 0, _receiveSize - packetSize);
            }

            _receiveSize -= packetSize;
        }

        if (_responses.Count > 0)
        {
            foreach (var response in _responses)
            {
                _receiveHandler?.Invoke(response);
            }

            _responses.Clear();
        }

        return true;
    }

    public void SetReceiveHandler(Action<Response> handler)
    {
        _receiveHandler = handler;
    }

    public void SetDisconnectHandler(Action handler)
    {
        _disconnectHandler = handler;
    }

    private byte[] MakeBuffer(Request request)
    {
        int packetSize = 4 + request.CalculateSize();
        byte[] buffer = new byte[packetSize];

        buffer[0] = (byte)(packetSize & 0xFF);
        buffer[1] = (byte)((packetSize >> 8) & 0xFF);
        buffer[2] = (byte)((packetSize >> 16) & 0xFF);
        buffer[3] = (byte)((packetSize >> 24) & 0xFF);

        request.WriteTo(new Span<byte>(buffer, 4, packetSize - 4));

        return buffer;
    }

    public void Dispose()
    {
        _socket?.Dispose();

        _socket = null;
    }
}
