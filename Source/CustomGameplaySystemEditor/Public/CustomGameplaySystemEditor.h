#pragma once

#include "Modules/ModuleManager.h"

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

	void CheckAbilitySystemGlobalsClass() const;
	void AddAbilitySystemGlobalsClassName() const;
	
	void CheckMaxRPC() const;
	void SetMaxRPC() const;

	void CheckInputComponent() const;
	void SetInputComponent(UInputSettings* InputSettings) const;
};