#include "ModularGameplayActors.h"

DEFINE_LOG_CATEGORY(ModularGameplayActors);

#define LOCTEXT_NAMESPACE "FModularGameplayActors"

void FModularGameplayActors::StartupModule()
{
	UE_LOG(ModularGameplayActors, Warning, TEXT("ModularGameplayActors module has been loaded"));
}

void FModularGameplayActors::ShutdownModule()
{
	UE_LOG(ModularGameplayActors, Warning, TEXT("ModularGameplayActors module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FModularGameplayActors, ModularGameplayActors)