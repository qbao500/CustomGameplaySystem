#include "CommonUser.h"

DEFINE_LOG_CATEGORY(CommonUser);

#define LOCTEXT_NAMESPACE "FCommonUser"

void FCommonUser::StartupModule()
{
	UE_LOG(CommonUser, Warning, TEXT("CommonUser module has been loaded"));
}

void FCommonUser::ShutdownModule()
{
	UE_LOG(CommonUser, Warning, TEXT("CommonUser module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCommonUser, CommonUser)