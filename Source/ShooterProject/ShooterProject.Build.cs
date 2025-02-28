// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShooterProject : ModuleRules
{
	public ShooterProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "GameplayTasks" });
        PublicDependencyModuleNames.AddRange(new string[] {
             "OnlineSubsystem",
             "OnlineSubsystemUtils"
         });
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

    }
}
