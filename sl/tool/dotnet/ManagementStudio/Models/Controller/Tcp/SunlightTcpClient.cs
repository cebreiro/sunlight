using System.Net;
using Google.Protobuf;
using NetCoreServer;
using Sunlight.Api;

namespace Sunlight.ManagementStudio.Models.Controller.Tcp;

public class SunlightTcpClient : TcpClient
{
    private const int PacketHeadSize = 4;

    private TaskCompletionSource<bool>? _connectionCompletionSource = null;

    private byte[] _receiveBuffer = new byte[1024];
    private long _receiveBufferSize = 1024;
    private long _receiveSize = 0;

    private List<Response> _responses = new();

    private Action<Response>? _receiveHandler = null;
    private Action? _disconnectHandler = null;

    public SunlightTcpClient(IPAddress address, int port)
        : base(address, port)
    {
    }

    public SunlightTcpClient(string address, int port)
        : base(address, port)
    {
    }

    public SunlightTcpClient(DnsEndPoint endpoint)
        : base(endpoint)
    {
    }

    public SunlightTcpClient(IPEndPoint endpoint)
        : base(endpoint)
    {
    }

    public Task<bool> Connect()
    {
        TaskCompletionSource<bool> tcs = new();

        if (_connectionCompletionSource != null)
        {
            tcs.SetResult(false);

            return tcs.Task;
        }

        if (!base.ConnectAsync())
        {
            tcs.SetResult(false);

            return tcs.Task;
        }

        _connectionCompletionSource = tcs;

        return tcs.Task;
    }

    protected override void OnConnected()
    {
        if (_connectionCompletionSource != null)
        {
            _connectionCompletionSource.SetResult(true);

            _connectionCompletionSource = null;
        }
    }

    protected override void OnDisconnected()
    {
        if (_connectionCompletionSource != null)
        {
            _connectionCompletionSource.SetResult(false);

            _connectionCompletionSource = null;
        }

        _disconnectHandler?.Invoke();
    }

    public void Send(Request request)
    {
        int packetSize = 4 + request.CalculateSize();
        byte[] buffer = new byte[packetSize];

        buffer[0] = (byte)(packetSize & 0xFF);
        buffer[1] = (byte)((packetSize >> 8) & 0xFF);
        buffer[2] = (byte)((packetSize >> 16) & 0xFF);
        buffer[3] = (byte)((packetSize >> 24) & 0xFF);

        request.WriteTo(new Span<byte>(buffer, 4, packetSize - 4));

        SendAsync(buffer);
    }

    protected override void OnReceived(byte[] buffer, long offset, long size)
    {
        long requiredBufferSize = _receiveSize + size;

        if (requiredBufferSize > _receiveBufferSize)
        {
            var newReceiveBuffer = new byte[requiredBufferSize];

            Array.Copy(_receiveBuffer, 0, newReceiveBuffer, 0, _receiveSize);

            _receiveBuffer = newReceiveBuffer;
            _receiveBufferSize = requiredBufferSize;
        }

        Array.Copy(buffer, offset, _receiveBuffer, _receiveSize, size);

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
                throw new Exception("fail to parse response");
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
                _receiveHandler.Invoke(response);
            }

            _responses.Clear();
        }
    }

    public void SetReceiveHandler(Action<Response> handler)
    {
        _receiveHandler = handler;
    }

    public void SetDisconnectHandler(Action handler)
    {
        _disconnectHandler = handler;
    }
}
