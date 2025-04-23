#include "CommonGame.h"

DEFINE_LOG_CATEGORY(CommonGame);

#define LOCTEXT_NAMESPACE "FCommonGame"

void FCommonGame::StartupModule()
{
	UE_LOG(CommonGame, Warning, TEXT("CommonGame module has been loaded"));
}

void FCommonGame::ShutdownModule()
{
	UE_LOG(CommonGame, Warning, TEXT("CommonGame module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCommonGame, CommonGame)