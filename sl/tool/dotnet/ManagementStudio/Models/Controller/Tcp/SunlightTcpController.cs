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

    public Task<AuthenticationResponse> Authenticate(AuthenticationRequest authentication)
    {
        return SendRequestAsync(authentication,
            (request, param) => request.Authentication = param,
            response => response.Authentication);
    }

    public Task<AccountCreationResponse> CreateAccount(AccountCreationReuqest accountCreation)
    {
        return SendRequestAsync(accountCreation,
            (request, param) => request.AccountCreation = param,
            response => response.AccountCreation);
    }

    public Task<AccountPasswordChangeResponse> ChangeAccountPassword(AccountPasswordChangeRequest accountPasswordChange)
    {
        return SendRequestAsync(accountPasswordChange,
            (request, param) => request.AccountPasswordChange = param,
            response => response.AccountPasswordChange);
    }

    public Task<UserCountResponse> GetUserCount(UserCountRequest userCountRequest)
    {
        return SendRequestAsync(userCountRequest,
            (request, param) => request.UserCount = param,
            response => response.UserCount);
    }

    public Task<SystemResourceInfoResponse> GetSystemInfo(SystemResourceInfoRequest systemResourceInfoRequest)
    {
        return SendRequestAsync(systemResourceInfoRequest,
            (request, param) => request.SystemResourceInfo = param,
            response => response.SystemResourceInfo);
    }

    public Task<WorldInfoResponse> GetWorldInfo(WorldInfoRequest worldInfoRequest)
    {
        return SendRequestAsync(worldInfoRequest,
            (request, param) => request.WorldInfo = param,
            response => response.WorldInfo);
    }

    public Task<LogGetResponse> GetLog(LogGetRequest logRequest)
    {
        return SendRequestAsync(logRequest,
            (request, param) => request.LogGet = param,
            response => response.LogGet);
    }

    private async Task<TResponse> SendRequestAsync<TRequest, TResponse>(
        TRequest request,
        Action<Request, TRequest> configureRequest,
        Func<Response, TResponse> configureResponse)
    {
        TaskCompletionSource<TResponse> completionSource = new();

        using (await _mutex.LockAsync())
        {
            int requestId = _nextRequestId++;
            System.Diagnostics.Debug.Assert(!_receiveHandlers.ContainsKey(requestId));

            _receiveHandlers[requestId] = new ReceiveHandler(
                response =>
                {
                    completionSource.SetResult(configureResponse(response));
                },
                () => completionSource.SetException(new Exception("operation aborted")));

            Request pbRequest = new()
            {
                RequestId = requestId,
            };

            configureRequest(pbRequest, request);

            Task task = _tcpClient.SendAsync(pbRequest);

            ConfigureSendFailHandler(task, requestId);
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
