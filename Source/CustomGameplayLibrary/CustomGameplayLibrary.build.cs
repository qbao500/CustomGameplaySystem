using UnrealBuildTool;
 
public class CustomGameplayLibrary : ModuleRules
{
	public CustomGameplayLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "Niagara",
			"GameplayAbilities", "GameplayTags"
		});
		
	}
}