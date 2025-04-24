using UnrealBuildTool;
 
public class CustomGameplaySystemEditor : ModuleRules
{
	public CustomGameplaySystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "UnrealEd", "EngineSettings",
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate", "SourceControl",
			"GameplayAbilities",
			"CustomGameplaySystem",
		});
		
	}
}