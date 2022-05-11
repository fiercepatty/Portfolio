// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	void InitializeWaterComponent(FWaterInfo Water);

	/**Turning off the mesh and collision for the mesh when we dont need them */
	void UnLoadMesh() const;

	/**The way we turn the mesh and collision back on after we turned it off*/
	void LoadMesh() const;

	//Todo Explain
	int NoiseResolution = 300;

	//Todo Explain
	int TotalSizeToGenerate = 1200;

	//Todo Explain
	int NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;

	//Todo Explain
	int VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;

	//The Height that the water will be at. The highest the terrain can be is 2x the NoiseOutputScale so we subtract this height from NoiseOutputScale to get the actual location for height
	int WaterHeight=300;

	//We save this so we can determine where the water should be in relation to the terrain
	int OutputScale=2000;

	/**This is the actual procedural mesh used when generating the map*/
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralWaterMesh;
	
private:
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
