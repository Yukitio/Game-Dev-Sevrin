using UnrealBuildTool;

public class LivingWorldRPG : ModuleRules
{
	public LivingWorldRPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Paper2D"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
