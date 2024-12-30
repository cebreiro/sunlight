using System.Collections.Concurrent;
using System.Text;
using Nito.AsyncEx;
using Sunlight.Api;
using Sunlight.ManagementStudio.Models.Event;
using Sunlight.ManagementStudio.Models.Event.Args;

namespace Sunlight.ManagementStudio.Models.Controller.Tcp;

public class SunlightTcpController : ISunlightController
{
    private readonly AsyncLock _mutex = new();

    private SunlightTcpClient _tcpClient = new(Encoding.ASCII.GetBytes("klasgdghasdhgfwiqufdvasbdjhkfqwygifaqsnmbfasdhjqwed"));

    private int _nextRequestId = 1;

    private ConcurrentDictionary<int, ReceiveHandler> _receiveHandlers = new();
    private readonly IEventProducer _eventProducer;

    public SunlightTcpController(IEventProducer eventProducer)
    {
        _eventProducer = eventProducer;

        _tcpClient.SetReceiveHandler(OnReceiveResponse);
        _tcpClient.SetDisconnectHandler(OnDisconnected);
    }

    private struct ReceiveHandler(Action<Response> successHandler, Action failureHandler)
    {
        public readonly Action<Response> SuccessHandler = successHandler;
        public readonly Action FailureHandler = failureHandler;
    }

    public async Task<bool> Connect(string address, ushort port)
    {
        if (_tcpClient.IsConnected())
        {
            return false;
        }

        try
        {
            if (await _tcpClient.ConnectAsync(address, port))
            {
                _tcpClient.StartReceive();

                return true;
            }
        }
        catch
        {
            // ignored
        }

        return false;
    }

    public async Task<AuthenticationResponse> Authenticate(AuthenticationRequest authentication)
    {
        TaskCompletionSource<AuthenticationResponse> completionSource = new();

        using (await _mutex.LockAsync())
        {
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

            ConfigureSendFailHandler(_tcpClient.SendAsync(request), requestId);
        }

        return await completionSource.Task;
    }

    public async Task<AccountCreationResponse> CreateAccount(AccountCreationReuqest accountCreation)
    {
        TaskCompletionSource<AccountCreationResponse> completionSource = new();

        using (await _mutex.LockAsync())
        {
            int requestId = _nextRequestId++;

            System.Diagnostics.Debug.Assert(!_receiveHandlers.ContainsKey(requestId));

            _receiveHandlers[requestId] = new ReceiveHandler(
                response =>
                {
                    completionSource.SetResult(response.AccountCreation);
                },
                () => completionSource.SetException(new Exception("operation aborted")));

            Request request = new()
            {
                RequestId = requestId,
                AccountCreation = accountCreation
            };

            ConfigureSendFailHandler(_tcpClient.SendAsync(request), requestId);
        }

        return await completionSource.Task;
    }

    public async Task<AccountPasswordChangeResponse> ChangeAccountPassword(AccountPasswordChangeRequest accountPasswordChange)
    {
        TaskCompletionSource<AccountPasswordChangeResponse> completionSource = new();

        using (await _mutex.LockAsync())
        {
            int requestId = _nextRequestId++;

            System.Diagnostics.Debug.Assert(!_receiveHandlers.ContainsKey(requestId));

            _receiveHandlers[requestId] = new ReceiveHandler(
                response =>
                {
                    completionSource.SetResult(response.AccountPasswordChange);
                },
                () => completionSource.SetException(new Exception("operation aborted")));

            Request request = new()
            {
                RequestId = requestId,
                AccountPasswordChange = accountPasswordChange
            };

            ConfigureSendFailHandler(_tcpClient.SendAsync(request), requestId);
        }

        return await completionSource.Task;
    }

    public async Task<UserCountResponse> GetUserCount(UserCountRequest userCountRequest)
    {
        TaskCompletionSource<UserCountResponse> completionSource = new();

        using (await _mutex.LockAsync())
        {
            int requestId = _nextRequestId++;

            System.Diagnostics.Debug.Assert(!_receiveHandlers.ContainsKey(requestId));

            _receiveHandlers[requestId] = new ReceiveHandler(
                response =>
                {
                    completionSource.SetResult(response.UserCount);
                },
                () => completionSource.SetException(new Exception("operation aborted")));

            Request request = new()
            {
                RequestId = requestId,
                UserCount = userCountRequest
            };

            ConfigureSendFailHandler(_tcpClient.SendAsync(request), requestId);
        }

        return await completionSource.Task;
    }

    public async Task<SystemResourceInfoResponse> GetSystemInfo(SystemResourceInfoRequest systemResourceInfoRequest)
    {
        TaskCompletionSource<SystemResourceInfoResponse> completionSource = new();

        using (await _mutex.LockAsync())
        {
            int requestId = _nextRequestId++;

            System.Diagnostics.Debug.Assert(!_receiveHandlers.ContainsKey(requestId));

            _receiveHandlers[requestId] = new ReceiveHandler(
                response =>
                {
                    completionSource.SetResult(response.SystemResourceInfo);
                },
                () => completionSource.SetException(new Exception("operation aborted")));

            Request request = new()
            {
                RequestId = requestId,
                SystemResourceInfo = systemResourceInfoRequest
            };

            ConfigureSendFailHandler(_tcpClient.SendAsync(request), requestId);
        }

        return await completionSource.Task;
    }

    public async Task<LogGetResponse> GetLog(LogGetRequest logRequest)
    {
        TaskCompletionSource<LogGetResponse> completionSource = new();
        using (await _mutex.LockAsync())
        {
            int requestId = _nextRequestId++;

            System.Diagnostics.Debug.Assert(!_receiveHandlers.ContainsKey(requestId));

            _receiveHandlers[requestId] = new ReceiveHandler(
                response =>
                {
                    completionSource.SetResult(response.LogGet);
                },
                () => completionSource.SetException(new Exception("operation aborted")));

            Request request = new()
            {
                RequestId = requestId,
                LogGet = logRequest
            };

            ConfigureSendFailHandler(_tcpClient.SendAsync(request), requestId);
        }

        return await completionSource.Task;
    }

    private void ConfigureSendFailHandler(Task task, int requestId)
    {
        task.ContinueWith(t =>
        {
            if (t.IsFaulted)
            {
                if (_receiveHandlers.TryRemove(requestId, out ReceiveHandler receiveHandler))
                {
                    receiveHandler.FailureHandler.Invoke();
                }
            }
        });
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
        ConcurrentDictionary<int, ReceiveHandler>? oldCollection = null;
 
        using (_mutex.Lock())
        {
            oldCollection = _receiveHandlers;

            _receiveHandlers = new ConcurrentDictionary<int, ReceiveHandler>();
        }

        foreach (var (_, handler) in oldCollection)
        {
            handler.FailureHandler.Invoke();
        }

        _eventProducer.Produce(new DisconnectionEventArgs());
    }
}
