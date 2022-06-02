// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavModifierComponent.h"
#include "ProceduralMeshComponent.h"
#include "TerrainStructInfo.h"
#include "Components/ActorComponent.h"
#include "ProceduralWaterComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_API UProceduralWaterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralWaterComponent();

	UFUNCTION(BlueprintCallable)
	void GenerateMap(FVector StartingLocation);

	//Init the Fast Noise Or the noise values used for our terrain are init here
	void InitializeWaterComponent(FTerrainInfo TerrainInfo);

	/**Turning off the mesh and collision for the mesh when we dont need them */
	void UnLoadMesh() const;

	/**The way we turn the mesh and collision back on after we turned it off*/
	void LoadMesh() const;

	//The Height that the water will be at. The highest the terrain can be is 2x the NoiseOutputScale so we subtract this height from NoiseOutputScale to get the actual location for height
	int WaterHeight=300;

	//We save this so we can determine where the water should be in relation to the terrain
	int OutputScale=2000;

	/**This is the actual procedural mesh used when generating the map*/
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralWaterMesh;


	
private:
	//This is where the Noise starts indexing in the Noise Texture. This is the starting point to index in our texture. 
	FVector ComponentLocation;
	
	//All the vertices of the mesh
	TArray<FVector> Vertices;

	//All the triangles of the mesh
	TArray<int> Triangles;

	//Saved off value for the Noise Resolution for calculations
	int NoiseResolution = 300;

	//Saved off value for the total size to generate for calculations
	int TotalSizeToGenerate = 1200;

	//Saved off Samples per line for calculations
	int NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;

	//Saved off size for calculations
	int VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;

	//Internal Function used to generate all the vertices of the mesh and the UVs for the mesh
	void GenerateVertices();

	//Internal function for generating all the triangles giving the procedural mesh a way of knowing what vertices indexes are in reference to what triangles
	void GenerateTriangles();

	//Internal function that just passes in the parameter needed to generate the mesh for the procedural mesh
	void GenerateMesh() const;

	//Internal Function that is used to tell what noise value is represented at every location.
	int GetIndexForGridCoordinates(int X, int Y) const;

	//Used to get the index for a specific coordinate in the array of vertices
	FVector2D GetPositionForGridCoordinates(int X, int Y) const;

	// Other things needed to generate the mesh
	//Array of all the Normals for the Vertices
	TArray<FVector> Normals;

	//Array of all the tangents for the vertices
	TArray<FProcMeshTangent> Tangents;

	//Array of all the UV for the terrain
	TArray<FVector2D> UV;

	//Colors for the actual vertexes
	TArray<FColor> VertexColors;

	UMaterialInstance* Mat=nullptr;

	//Whether Collision is enabled
	bool bEnableCollision;

};
