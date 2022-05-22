// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Portfolio : ModuleRules
{
	public Portfolio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.AddRange(new string[] {
			"Portfolio/",
			"Portfolio/ProceduralTerrainGeneration",
			"Portfolio/ProceduralTerrainGeneration/WaterGeneration",
			"Portfolio/ProceduralTerrainGeneration/TerrainGeneration"
		});
		
		
        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule",
	        "FastNoise","FastNoiseGenerator", "ProceduralMeshComponent", "MeshDescription" ,"StaticMeshDescription", "Foliage"

        });
        

    }
}
