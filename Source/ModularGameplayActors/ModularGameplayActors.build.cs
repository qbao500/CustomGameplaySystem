using UnrealBuildTool;
 
public class ModularGameplayActors : ModuleRules
{
	public ModularGameplayActors(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine",
			"ModularGameplay", "AIModule",
		});
	}
}