using Newtonsoft.Json;

namespace Sunlight.ManagementStudio.Models.Setting;

public class Settings
{
    [JsonProperty("AppearanceSetting")]
    public AppearanceSetting AppearanceSetting { get; set; } = new();
}
