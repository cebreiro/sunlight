using Newtonsoft.Json;
using System.IO;
using Wpf.Ui.Appearance;

namespace Sunlight.ManagementStudio.Models.Setting;

public class SettingsProvider
{
    private Settings _settings = new();

    public AppearanceSetting AppearanceSetting => _settings.AppearanceSetting;
    public ConnectionSetting ConnectionSetting => _settings.ConnectionSetting;

    public void LoadFromFile()
    {
        string filePath = GetSettingFilePath();

        if (!File.Exists(filePath))
        {
            return;
        }

        string fileContents = File.ReadAllText(filePath);

        _settings = JsonConvert.DeserializeObject<Settings>(fileContents) ?? new();
    }

    public async Task SaveToFileAsync()
    {
        AppearanceSetting.Theme = ApplicationThemeManager.GetAppTheme().ToString();

        string filePath = GetSettingFilePath();
        string? directory = Path.GetDirectoryName(filePath);

        if (directory != null && !Directory.Exists(directory))
        {
            Directory.CreateDirectory(directory);
        }

        await File.WriteAllTextAsync(filePath, JsonConvert.SerializeObject(_settings, Formatting.Indented)).ConfigureAwait(false);
    }

    static string GetSettingFilePath()
    {
        return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "sunlight", "management_studio_settings.json");
    }
}
