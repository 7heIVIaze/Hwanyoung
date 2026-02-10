// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class hwanyoung2 : ModuleRules
{
	public hwanyoung2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Niagara" });

		PrivateDependencyModuleNames.AddRange(new string[] { "FMODStudio" });

		PublicIncludePaths.AddRange(new string[]
		{
			"hwanyoung2",
			"hwanyoung2/",
			"hwanyoung2/System/"
        });
	}
}
