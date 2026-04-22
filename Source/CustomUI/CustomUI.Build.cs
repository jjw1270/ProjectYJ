// Copyright (c) 2026 장윤제. All rights reserved.

using System.IO;
using UnrealBuildTool;

public class CustomUI : ModuleRules
{
	public CustomUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModuleDirectory, "Public/Widgets"),
				// ... add public include paths required here ...
			}
            );
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);

        PublicDefinitions.Add("FEATURE_PAGE=0");

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "UMG",
                "CommonLibrary",
                "DeveloperSettings",
            }
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Slate",
                "SlateCore",
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
