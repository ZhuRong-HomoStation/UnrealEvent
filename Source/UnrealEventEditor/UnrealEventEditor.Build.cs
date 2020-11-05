// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealEventEditor : ModuleRules
{
	public UnrealEventEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"UnrealEvent" ,
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"UnrealEd" ,		// editor function 
				"DesktopPlatform" ,	// dialog tool 
				"EditorStyle" ,		// for use editor styles 
				"PropertyEditor" ,	// for custom detail 
				"EditorWidgets" ,	// for use editor widgets 
				"ActorPickerMode" ,	// for pick actor 
				"SceneOutliner" ,	// for use scene outline 
				"ApplicationCore" ,	// for use application core 
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
