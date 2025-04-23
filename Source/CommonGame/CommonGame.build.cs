using UnrealBuildTool;
 
public class CommonGame : ModuleRules
{
	public CommonGame(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(
	        new string[]
	        {
		        "Core", "CoreUObject", "InputCore", "Engine",
		        "Slate", "SlateCore", "UMG",
		        "CommonInput", "CommonUI",
		        "GameplayTags",
		        "ModularGameplayActors", "CommonUser",
	        }
        );
	}
}