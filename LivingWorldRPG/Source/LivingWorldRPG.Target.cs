using UnrealBuildTool;
using System.Collections.Generic;

public class LivingWorldRPGTarget : TargetRules
{
	public LivingWorldRPGTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("LivingWorldRPG");
	}
}
