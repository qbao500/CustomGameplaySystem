using UnrealBuildTool;
 
public class CommonUser : ModuleRules
{
	public CommonUser(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreOnline",
			"GameplayTags",
			"OnlineSubsystemUtils",
		});

		bool bUseOnlineSubsystemV1 = true;
		if (bUseOnlineSubsystemV1)
		{
			PublicDependencyModuleNames.Add("OnlineSubsystem");
		}
		else
		{
			PublicDependencyModuleNames.Add("OnlineServicesInterface");
		}

		PrivateDependencyModuleNames.Add("OnlineSubsystemUtils");

		PublicDefinitions.Add("COMMONUSER_OSSV1=" + (bUseOnlineSubsystemV1 ? "1" : "0"));

		PrivateDependencyModuleNames.AddRange(new string[]
		{
				"CoreOnline",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"ApplicationCore",
				"InputCore",
		});
	}
}