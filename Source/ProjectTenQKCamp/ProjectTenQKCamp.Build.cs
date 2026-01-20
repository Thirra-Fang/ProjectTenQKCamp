// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectTenQKCamp : ModuleRules
{
	public ProjectTenQKCamp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ProjectTenQKCamp",
			"ProjectTenQKCamp/Variant_Horror",
			"ProjectTenQKCamp/Variant_Horror/UI",
			"ProjectTenQKCamp/Variant_Shooter",
			"ProjectTenQKCamp/Variant_Shooter/AI",
			"ProjectTenQKCamp/Variant_Shooter/UI",
			"ProjectTenQKCamp/Variant_Shooter/Weapons",
			"ProjectTenQKCamp/GameModes",
			"ProjectTenQKCamp/GameModes/TestFPS",
			"ProjectTenQKCamp/GameModes/TestFPS/AI",
			"ProjectTenQKCamp/GameModes/TestFPS/AI/Common",
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
