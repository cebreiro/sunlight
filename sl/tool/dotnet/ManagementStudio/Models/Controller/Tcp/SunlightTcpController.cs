using System.Collections.Concurrent;
using Nito.AsyncEx;
using Sunlight.Api;

namespace Sunlight.ManagementStudio.Models.Controller.Tcp;

public class SunlightTcpController : ISunlightController
{
    private readonly AsyncLock _mutex = new();

    private SunlightTcpClient? _tcpClient = null;
    private bool _pendingConnection = false;

    private int _nextRequestId = 1;

    private ConcurrentDictionary<int, ReceiveHandler> _receiveHandlers = new();

    private struct ReceiveHandler(Action<Response> successHandler, Action failureHandler)
    {
        public readonly Action<Response> SuccessHandler = successHandler;
        public readonly Action FailureHandler = failureHandler;
    }

    public Task<bool> Connect(string address, ushort port)
    {
        TaskCompletionSource<bool> completionSource = new();

        using (_mutex.Lock())
        {
            if (_pendingConnection)
            {
                completionSource.SetException(new Exception("connection is pending"));

                return completionSource.Task;
            }

            _pendingConnection = true;

            SunlightTcpClient tcpClient = new(address, port);

            tcpClient.Connect().ContinueWith(task =>
            {
                bool connected = task.Result;

                using (_mutex.Lock())
                {
                    if (connected)
                    {
                        _tcpClient = tcpClient;
                        _tcpClient.SetReceiveHandler(OnReceiveResponse);
                        _tcpClient.SetDisconnectHandler(OnDisconnected);
                    }

                    _pendingConnection = false;
                }

                completionSource.SetResult(connected);
            });
        }

        return completionSource.Task;
    }

    public Task<AuthenticationResponse> Authenticate(AuthenticationRequest authentication)
    {
        TaskCompletionSource<AuthenticationResponse> completionSource = new();

        using (_mutex.Lock())
        {
            if (_tcpClient == null)
            {
                completionSource.SetException(new Exception("tcp client is not connected"));

                return completionSource.Task;
            }

            int requestId = _nextRequestId++;

            System.Diagnostics.Debug.Assert(!_receiveHandlers.ContainsKey(requestId));

            _receiveHandlers[requestId] = new ReceiveHandler(
                response =>
                {
                    completionSource.SetResult(response.Authentication);
                },
                () => completionSource.SetException(new Exception("operation aborted")));

            Request request = new()
            {
                RequestId = requestId,
                Authentication = authentication
            };

            _tcpClient.Send(request);
        }

        return completionSource.Task;
    }

    private void OnReceiveResponse(Response response)
    {
        if (_receiveHandlers.TryRemove(response.RequestId, out ReceiveHandler receiveHandler))
        {
            receiveHandler.SuccessHandler.Invoke(response);
        }
        else
        {
            System.Diagnostics.Debug.Assert(false);
        }
    }

    private void OnDisconnected()
    {
        ConcurrentDictionary<int, ReceiveHandler> oldCollection = null;
        SunlightTcpClient tcpClient = null;

        using (_mutex.Lock())
        {
            if (_tcpClient == null)
            {
                return;
            }

            tcpClient = _tcpClient;
            oldCollection = _receiveHandlers;

            _nextRequestId = 1;
            _tcpClient = null;
            _receiveHandlers = new ConcurrentDictionary<int, ReceiveHandler>();
        }

        tcpClient.Dispose();

        foreach (var (_, handler) in oldCollection)
        {
            handler.FailureHandler.Invoke();
        }
    }
}
