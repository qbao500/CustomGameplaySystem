#include "CustomGameplaySystemEditor.h"

#include "AbilitySystemGlobals.h"
#include "SourceControlHelpers.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GameFramework/InputSettings.h"
#include "GAS/CustomAbilitySystemGlobals.h"
#include "Input/CustomInputComponent.h"
#include "Widgets/Notifications/SNotificationList.h"

DEFINE_LOG_CATEGORY(LogCustomEditor);

#define LOCTEXT_NAMESPACE "FCustomGameplaySystemEditor"

void FCustomGameplaySystemEditor::StartupModule()
{
	UE_LOG(LogCustomEditor, Warning, TEXT("CustomGameplaySystemEditor module has been loaded"));
	
	CheckAbilitySystemGlobalsClass();
	CheckMaxRPC();
	CheckInputComponent();
}

void FCustomGameplaySystemEditor::ShutdownModule()
{
	UE_LOG(LogCustomEditor, Warning, TEXT("CustomGameplaySystemEditor module has been unloaded"));
}

void FCustomGameplaySystemEditor::CheckAbilitySystemGlobalsClass() const
{
	UE_LOG(LogCustomEditor, Verbose, TEXT("FCustomGameplaySystemEditor::CheckAbilitySystemGlobalsClass"));
	
	const UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	if (AbilitySystemGlobals.IsA(UCustomAbilitySystemGlobals::StaticClass()))
	{
		// We good
		return;
	}
	
	UE_LOG(LogCustomEditor, Error, TEXT("CustomGameplaySystemEditor module - REQUIRE CustomAbilitySystemGlobals (or subclass of it) to be AbilitySystemGlobals"));

	// Check UAbilitySystemGlobals config
	const FSoftClassPath AbilitySystemGlobalsClassName = GetDefault<UAbilitySystemGlobals>()->AbilitySystemGlobalsClassName;
	UE_LOG(LogCustomEditor, Verbose, TEXT("FCustomGameplaySystemEditor - AbilitySystemGlobalsClassName: %s"), *AbilitySystemGlobalsClassName.ToString());

	// Warn and add option for users to set it up to our custom subclass
	const FText MessageEntry = LOCTEXT("MissingRuleForAbilitySystemGlobals",
		"Custom Gameplay System: AbilitySystemGlobals is not a class of type {0}, which is required by this plugin");
	const FText TokenText = LOCTEXT("AddRuleForAbilitySystemGlobalsAction",
		"Set AbilitySystemGlobalsClassName to CustomAbilitySystemGlobals (REQUIRED)");

	FMessageLog("LoadErrors").Error()->AddToken(FTextToken::Create(FText::Format(
		MessageEntry,
		FText::FromName(UCustomAbilitySystemGlobals::StaticClass()->GetFName())
		)));

	FMessageLog("LoadErrors").Error()
		->AddToken(FTextToken::Create(LOCTEXT("AddRuleForAbilitySystemGlobalsLabel", "Click the link to set it to CustomAbilitySystemGlobals in DefaultGame.ini:")))
		->AddToken(FActionToken::Create(
			TokenText,
			FText(),
			FOnActionTokenExecuted::CreateRaw(this, &FCustomGameplaySystemEditor::AddAbilitySystemGlobalsClassName), true
		));
}

void FCustomGameplaySystemEditor::AddAbilitySystemGlobalsClassName() const
{
	UAbilitySystemGlobals* AbilitySystemGlobals = GetMutableDefault<UAbilitySystemGlobals>();
	if (!AbilitySystemGlobals)
	{
		return;
	}

	AbilitySystemGlobals->AbilitySystemGlobalsClassName = UCustomAbilitySystemGlobals::StaticClass();
	const FString DefaultConfigFilename = AbilitySystemGlobals->GetDefaultConfigFilename();
	UE_LOG(LogCustomEditor, Verbose, TEXT("Changing AbilitySystemGlobals in: %s"), *DefaultConfigFilename);
	
	AbilitySystemGlobals->UpdateSinglePropertyInConfigFile(
		AbilitySystemGlobals->GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UAbilitySystemGlobals, AbilitySystemGlobalsClassName)), DefaultConfigFilename);

	// Notify user
	FNotificationInfo Info(LOCTEXT(
		"SettingsAbilitySystemGlobalsClassUpdated",
		"AbilitySystemGlobals Class Name has been updated to use CustomAbilitySystemGlobals.\n\n"
		"You need to restart the editor for it to take effect."
	));

	Info.FadeInDuration = 0.2f;
	Info.ExpireDuration = 5.0f;
	Info.FadeOutDuration = 1.0f;
	Info.bUseThrobber = false;
	Info.bUseLargeFont = false;

	const TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
	if (Notification.IsValid())
	{
		Notification->SetCompletionState(SNotificationItem::CS_Success);
	}
}

void FCustomGameplaySystemEditor::CheckMaxRPC() const
{
	const IConsoleManager& ConsoleManager = IConsoleManager::Get();

	const IConsoleVariable* MaxRPCPerNetVar = ConsoleManager.FindConsoleVariable(TEXT("net.MaxRPCPerNetUpdate"));
	if (!MaxRPCPerNetVar) return;
	
	const int32 MaxRPC = MaxRPCPerNetVar->GetInt();
	if (MaxRPC >= 5) return;

	// Warn and add option for users to set it up to our custom subclass
	const FText MessageEntry = LOCTEXT("LowRPC",
		"net.MaxRPCPerNetUpdate (Maximum number of Unreliable Multicast RPC calls allowed per network update, additional ones are dropped) is currently {0}. \n\n"
		"That is too low and required to be at least 5! For reference, Lyra project sets it to 10. \n");
	const FText TokenText = LOCTEXT("ActionSetRPC",
		"Set net.MaxRPCPerNetUpdate to 5?");

	FMessageLog("LoadErrors").Error()->AddToken(FTextToken::Create(FText::Format(
		MessageEntry,	MaxRPC)));
	
	FMessageLog("LoadErrors").Error()
		->AddToken(FTextToken::Create(LOCTEXT("LabelSetRPC", "Click the link to set it to 5 (you can change later in DefaultEngine.ini). ")))
		->AddToken(FActionToken::Create(
			TokenText,
			FText(),
			FOnActionTokenExecuted::CreateRaw(this, &FCustomGameplaySystemEditor::SetMaxRPC), true
		));
}

void FCustomGameplaySystemEditor::SetMaxRPC() const
{
	const FString Section = TEXT("ConsoleVariables");
	const FString Key = TEXT("net.MaxRPCPerNetUpdate");
	
	// Get project-specific DefaultEngine.ini path
	const FString ConfigFile = FPaths::Combine(FPaths::ProjectConfigDir(), TEXT("DefaultEngine.ini"));
	
	// Update the config value
	GConfig->SetInt(*Section, *Key, 5, ConfigFile);

	// Ensure the changes are saved to DefaultEngine.ini
	GConfig->Flush(false, ConfigFile);

	// Notify user
	FNotificationInfo Info(LOCTEXT(
		"DoneSetRPC",
		"net.MaxRPCPerNetUpdate is set to 5 in DefaultEngine.ini file.\n\n"
		"You need to restart the editor for it to take effect."
	));

	Info.FadeInDuration = 0.2f;
	Info.ExpireDuration = 5.0f;
	Info.FadeOutDuration = 1.0f;
	Info.bUseThrobber = false;
	Info.bUseLargeFont = false;

	const TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
	if (Notification.IsValid())
	{
		Notification->SetCompletionState(SNotificationItem::CS_Success);
	}
}

void FCustomGameplaySystemEditor::CheckInputComponent() const
{
	UInputSettings* InputSettings = UInputSettings::GetInputSettings();
	if (!InputSettings) return;

	UClass* InputCompClass = InputSettings->GetDefaultInputComponentClass();
	if (InputCompClass->IsChildOf(UCustomInputComponent::StaticClass()))
	{
		// All good
		return;
	}

	// Warn and add option for users to set it up to our custom subclass
	const FText MessageEntry = LOCTEXT("WrongInputComp",
		"DefaultInputComponentClass is not a class of type {0}, which is required by this plugin");
	const FText TokenText = LOCTEXT("ActionSetInputComp",
		"Set DefaultInputComponentClass to use CustomInputComponent (REQUIRED)");

	FMessageLog("LoadErrors").Error()->AddToken(FTextToken::Create(FText::Format(
		MessageEntry,
		FText::FromName(UCustomInputComponent::StaticClass()->GetFName())
		)));

	FMessageLog("LoadErrors").Error()
		->AddToken(FTextToken::Create(LOCTEXT("LabelSetInputComp", "Click the link to set it to CustomInputComponent. ")))
		->AddToken(FActionToken::Create(
			TokenText,
			FText(),
			FOnActionTokenExecuted::CreateRaw(this, &FCustomGameplaySystemEditor::SetInputComponent, InputSettings), true
		));
}

void FCustomGameplaySystemEditor::SetInputComponent(UInputSettings* InputSettings) const
{
	if (!InputSettings) return;

	// Set to new class
	InputSettings->SetDefaultInputComponentClass(UCustomInputComponent::StaticClass());
	
	// We can write to the file if it is not read only. If it is read only, then we can write to it if we successfully check it out with source control
	const FString DefaultConfigFile = InputSettings->GetDefaultConfigFilename();
	const bool bCanWriteToFile = !IFileManager::Get().IsReadOnly(*DefaultConfigFile) ||
		(USourceControlHelpers::IsEnabled() && USourceControlHelpers::CheckOutFile(DefaultConfigFile));
	if (bCanWriteToFile)
	{
		InputSettings->TryUpdateDefaultConfigFile(DefaultConfigFile, /* bWarnIfFail */ false);
	}

	// Notify user
	FNotificationInfo Info(LOCTEXT(
		"DoneSetInputComp",
		"DefaultInputComponentClass is now using CustomInputComponent.\n\n"
		"You may need to restart the editor for it to take effect."
	));

	Info.FadeInDuration = 0.2f;
	Info.ExpireDuration = 5.0f;
	Info.FadeOutDuration = 1.0f;
	Info.bUseThrobber = false;
	Info.bUseLargeFont = false;

	const TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
	if (Notification.IsValid())
	{
		Notification->SetCompletionState(SNotificationItem::CS_Success);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCustomGameplaySystemEditor, CustomGameplaySystemEditor)