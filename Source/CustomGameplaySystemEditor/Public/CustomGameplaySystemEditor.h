#pragma once

#include "Modules/ModuleManager.h"

class UGameMapsSettings;
class UInputSettings;

DECLARE_LOG_CATEGORY_EXTERN(LogCustomEditor, All, All);

class FCustomGameplaySystemEditor : public IModuleInterface
{
	
public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:

	void CheckGameInstance();
	void SetGameInstance(UGameMapsSettings* GameMapsSettings) const;

	void CheckAbilitySystemGlobalsClass() const;
	void SetCustomAbilitySystemGlobalsClass() const;
	
	void CheckMaxRPC() const;
	void SetMaxRPC() const;

	void CheckInputComponent() const;
	void SetInputComponent(UInputSettings* InputSettings) const;

	void CheckGameViewport();
	void SetGameViewport() const;

	void CheckLocalPlayer();
	void SetLocalPlayer() const;

	// Helpers
	bool CanWriteToFile(const FString& ConfigFile) const;
	void NotifyUserSuccess(FNotificationInfo& Info) const;
};