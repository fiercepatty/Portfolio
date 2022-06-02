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
			"Portfolio/ProceduralTerrainGeneration/ActorGeneration",
			"Portfolio/ProceduralTerrainGeneration/ActorGeneration/ChildrenClasses",
			"Portfolio/ProceduralTerrainGeneration/ActorGeneration/ChildrenClasses/Resources",
			//"Portfolio/ProceduralTerrainGeneration/ActorGeneration/ChildrenClasses/Resources/Rocks",
			//"Portfolio/ProceduralTerrainGeneration/ActorGeneration/ChildrenClasses/Resources/Trees",
			"Portfolio/ProceduralTerrainGeneration/ActorGeneration/ChildrenClasses/Spawners",
			"Portfolio/ProceduralTerrainGeneration/TerrainGeneration"
		});
		
		
        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule",
	        "FastNoise","FastNoiseGenerator", "ProceduralMeshComponent", "MeshDescription" ,"StaticMeshDescription", "Foliage"

        });
        

    }
}
