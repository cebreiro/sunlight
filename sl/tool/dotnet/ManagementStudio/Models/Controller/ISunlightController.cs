using Sunlight.Api;

namespace Sunlight.ManagementStudio.Models.Controller;

public interface ISunlightController
{
    public Task<bool> Connect(string address, ushort port);
    public Task<AuthenticationResponse> Authenticate(AuthenticationRequest authentication);
}
