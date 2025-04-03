#include "CustomGameplayLibrary.h"

DEFINE_LOG_CATEGORY(CustomGameplayLibrary);

#define LOCTEXT_NAMESPACE "FCustomGameplayLibrary"

void FCustomGameplayLibrary::StartupModule()
{
	UE_LOG(CustomGameplayLibrary, Warning, TEXT("CustomGameplayLibrary module has been loaded"));
}

void FCustomGameplayLibrary::ShutdownModule()
{
	UE_LOG(CustomGameplayLibrary, Warning, TEXT("CustomGameplayLibrary module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCustomGameplayLibrary, CustomGameplayLibrary)