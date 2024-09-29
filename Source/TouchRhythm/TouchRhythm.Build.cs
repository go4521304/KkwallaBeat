// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TouchRhythm : ModuleRules
{
	public TouchRhythm(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "Paper2D", "FMODStudio", "UMG", "Slate", "SlateCore", "FMODStudio", "LevelSequence", "MovieScene" });
    }
}
