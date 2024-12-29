using Newtonsoft.Json;

namespace Sunlight.ManagementStudio.Models.Setting;

public class ConnectionSetting
{
    [JsonProperty("Address")]
    public string Address { get; set; } = string.Empty;

    [JsonProperty("Port")]
    public string Port { get; set; } = string.Empty;

    [JsonProperty("Id")]
    public string Id { get; set; } = string.Empty;
}
