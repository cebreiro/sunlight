using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Sunlight.Api;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Models.Controller;
using Sunlight.ManagementStudio.ViewModels.Pages.Account;
using Sunlight.ManagementStudio.Views.Pages;
using Sunlight.ManagementStudio.Views.Windows;

namespace Sunlight.ManagementStudio.ViewModels.Pages;

public partial class AccountPageViewModel(IServiceProvider serviceProvider) : ObservableObject
{
    public CreateViewModel CreateViewModel { get; } = new();

    [RelayCommand]
    private void OnCreateOkButtonClicked(object sender)
    {
        if (sender is not AccountPage)
        {
            return;
        }

        AccountCreationReuqest request = new()
        {
            Id = CreateViewModel.Id,
            Password = CreateViewModel.Password,
            GmLevel = string.IsNullOrEmpty(CreateViewModel.GmLevel) ? 0 : byte.Parse(CreateViewModel.GmLevel),
        };

        CreateViewModel.IsActive = false;

        Task.Run(async () =>
        {
            try
            {
                AccountCreationResponse response =
                    await serviceProvider.GetRequired<ISunlightController>().CreateAccount(request);

                if (response != null)
                {
                    MessageBox.Show(serviceProvider.GetRequired<MainWindow>(),
                        $"success: {response.Success}, error: {response.ErrorMessage}", "result");
                }
            }
            catch
            {
                // ignored
            }
            finally
            {
                Application.Current.Dispatcher.InvokeAsync(() =>
                {
                    CreateViewModel.IsActive = true;
                });
            }
        });
    }
}
