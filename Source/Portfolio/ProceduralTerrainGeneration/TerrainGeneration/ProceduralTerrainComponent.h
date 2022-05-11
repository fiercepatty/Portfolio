// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "FastNoiseWrapper.h"
#include "ProceduralTerrainComponent.generated.h"


struct FTerrainInfo;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_API UProceduralTerrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralTerrainComponent();
	
	//Init the Fast Noise Or the noise values used for our terrain are init here
	void InitializeFastNoise(FTerrainInfo Terrain);

	//Init all the connection noise so that we can use them to make sure borders are flush up with one another
	void InitializeConnectionNoise(TArray<FTerrainInfo> Terrains) ;

	/**
	 * @param StartingLocation - This is where we will be indexing in the noise map to keep consistent with even map created
	 * Function used to created the Procedural Map Calls multiple helper functions
	 */
	UFUNCTION(BlueprintCallable)
	void GenerateMap(FVector StartingLocation);

	/**
	 * Used to unload the mesh and any collisions that are unnecessary when we can not see the map
	 */
	void UnLoadMesh() const;

	/**
	 * Used to load the mesh and collision back when we need to see the map
	 */
	void LoadMesh() const;

	/**This is the starting location saved off that is passed in Generate Map*/
	FVector NoiseComponentStartLocation;

	//Todo Explain
	int NoiseResolution = 300;

	//Todo Explain
	int TotalSizeToGenerate = 1200;

	//Todo Explain
	float NoiseInputScale = 0.5;

	//How large the output the Terrain will be. This will be doubled so there will be 4000 units used its just half will be in negative space
	float FastNoiseOutputScale = 2000;

	//Todo Explain
	float ConnectionNoiseOutputScale =2000;
	
	//Todo Explain
	int NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;

	/**The Wrapper that saves off the Noise that the terrain is using*/
	UPROPERTY(VisibleAnywhere, Category = "Terrian Generation | Noise Settings")
	UFastNoiseWrapper* TerrainNoise;

	/**All of the connections with this terrain so all the places that its edges touch corners count as well*/
	UPROPERTY()
	TArray<UFastNoiseWrapper*> ConnectionNoises;

	//Todo Explain
	int VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;

	/**This is the actual mesh that is created when generating meshes*/
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralTerrainMesh;

private:
	//Should we average between the edges of two terrains or not
	bool bAverage = true;

	//Todo Explain
	FVector ComponentLocation;

	//Todo Explain
	TArray<FVector> Vertices;

	//Todo Explain
	TArray<int> Triangles;

	

	//Todo Explain
	void GenerateVertices();
	
	//Todo Explain
	void GenerateTriangles();

	//Todo Explain
	void GenerateMesh() const;

	//Todo Explain
	float GetNoiseValueForGridCoordinates(int X, int Y) const;

	//Todo Explain
	int GetIndexForGridCoordinates(int X, int Y) const;

	//Todo Explain
	FVector2D GetPositionForGridCoordinates(int X, int Y) const;

	// Other things needed to generate the mesh
	//Todo Explain
	TArray<FVector> Normals;

	//Todo Explain
	TArray<FProcMeshTangent> Tangents;

	//Todo Explain
	TArray<FVector2D> UV;

	//Todo Explain
	TArray<FColor> VertexColors;
	
};
