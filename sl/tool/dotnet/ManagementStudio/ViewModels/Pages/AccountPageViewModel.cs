using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Sunlight.Api;
using Sunlight.ManagementStudio.Helpers;
using Sunlight.ManagementStudio.Models.Controller;
using Sunlight.ManagementStudio.ViewModels.Pages.Account;
using Sunlight.ManagementStudio.Views.Pages;
using Sunlight.ManagementStudio.Views.Windows;
using Wpf.Ui.Controls;

namespace Sunlight.ManagementStudio.ViewModels.Pages;

public partial class AccountPageViewModel(IServiceProvider serviceProvider) : ObservableObject
{
    public CreateViewModel CreateViewModel { get; } = new();
    public ChangePasswordViewModel ChangePasswordViewModel { get; } = new();

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
            AccountCreationResponse? response = null;

            try
            {
                response =
                    await serviceProvider.GetRequired<ISunlightController>().CreateAccount(request);
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

                    if (response != null)
                    {
                        MainWindow mainWindow = serviceProvider.GetRequired<MainWindow>();

                        if (response.Success != 0)
                        {
                            mainWindow.Notify(
                                title: "Success to Account Creation",
                                message: $"id: {request.Id}, GM: {request.GmLevel}",
                                seconds: 3.0,
                                ControlAppearance.Success);
                        }
                        else
                        {
                            mainWindow.Notify(
                                title: "Failure to Account Creation",
                                message: $"id: {request.Id}, GM: {request.GmLevel}, Error: {response.ErrorMessage}",
                                seconds: 3.0,
                                ControlAppearance.Danger);
                        }
                    }
                });
            }
        });
    }

    [RelayCommand]
    private void OnChangePasswordOkButtonClicked(object sender)
    {
        if (sender is not AccountPage)
        {
            return;
        }

        AccountPasswordChangeRequest request = new()
        {
            Id = ChangePasswordViewModel.Id,
            Password = ChangePasswordViewModel.Password,
        };

        ChangePasswordViewModel.IsActive = false;

        Task.Run(async () =>
        {
            AccountPasswordChangeResponse? response = null;

            try
            {
                response =
                    await serviceProvider.GetRequired<ISunlightController>().ChangeAccountPassword(request);
            }
            catch
            {
                // ignored
            }
            finally
            {
                Application.Current.Dispatcher.InvokeAsync(() =>
                {
                    ChangePasswordViewModel.IsActive = true;

                    if (response != null)
                    {
                        MainWindow mainWindow = serviceProvider.GetRequired<MainWindow>();

                        if (response.Success != 0)
                        {
                            mainWindow.Notify(
                                title: "Success to Account Password Change",
                                message: $"id: {request.Id}",
                                seconds: 3.0,
                                ControlAppearance.Success);
                        }
                        else
                        {
                            mainWindow.Notify(
                                title: "Failure to Account Password Change",
                                message: $"id: {request.Id}, Error: {response.ErrorMessage}",
                                seconds: 3.0,
                                ControlAppearance.Danger);
                        }
                    }
                });
            }
        });
    }
}
