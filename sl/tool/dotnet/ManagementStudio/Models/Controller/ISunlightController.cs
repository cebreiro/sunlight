using Sunlight.Api;

namespace Sunlight.ManagementStudio.Models.Controller;

public interface ISunlightController
{
    public Task<bool> Connect(string address, ushort port);
    public Task<AuthenticationResponse> Authenticate(AuthenticationRequest authentication);
    public Task<AccountCreationResponse> CreateAccount(AccountCreationReuqest accountCreation);
    public Task<AccountPasswordChangeResponse> ChangeAccountPassword(AccountPasswordChangeRequest accountPasswordChange);
    public Task<UserCountResponse> GetUserCount(UserCountRequest userCountRequest);
    public Task<SystemResourceInfoResponse> GetSystemInfo(SystemResourceInfoRequest systemResourceInfoRequest);
    public Task<LogGetResponse> GetLog(LogGetRequest logRequest);
}
