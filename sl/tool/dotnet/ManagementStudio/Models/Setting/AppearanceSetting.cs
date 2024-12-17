using Newtonsoft.Json;
using Wpf.Ui.Appearance;

namespace Sunlight.ManagementStudio.Models.Setting;

public class AppearanceSetting
{
    [JsonProperty("Theme")]
    public string Theme { get; set; } = ApplicationTheme.Dark.ToString();
}
