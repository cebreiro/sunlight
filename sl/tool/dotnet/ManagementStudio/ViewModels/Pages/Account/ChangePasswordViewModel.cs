using CommunityToolkit.Mvvm.ComponentModel;

namespace Sunlight.ManagementStudio.ViewModels.Pages.Account;

public class ChangePasswordViewModel : ObservableObject
{
    public bool IsActive { get; set; } = true;
    public string Id { get; set; } = string.Empty;
    public string Password { get; set; } = string.Empty;
}
