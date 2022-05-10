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

	void InitializeConnectionNoise(FTerrainInfo Terrain) ;
	
	UFUNCTION(BlueprintCallable)
	void GenerateMap(FVector StartingLocation);
	
	bool IsGenerated() const;

	bool IsVisible() const;

	void UnLoadMesh();

	void LoadMesh();

	FVector NoiseComponentStartLocation;
	
	int NoiseResolution = 300;

	int TotalSizeToGenerate = 1200;
	
	float NoiseInputScale = 0.5;

	float FastNoiseOutputScale = 2000;

	float ConnectionNoiseOutputScale =2000;
	

	int NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	
	UPROPERTY(VisibleAnywhere, Category = "Terrian Generation | Noise Settings")
	UFastNoiseWrapper* FastNoise;

	UPROPERTY(VisibleAnywhere, Category = "Terrian Generation | Noise Settings")
	UFastNoiseWrapper* ConnectionNoise;

	int VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralMesh;

private:
	
	FVector ComponentLocation;
	bool bGenerated =false;
	bool bIsVisible = false;
	TArray<FVector> Vertices;
	TArray<int> Triangles;

	


	void GenerateVertices();
	void GenerateTriangles();
	void GenerateMesh() const;

	float GetNoiseValueForGridCoordinates(int X, int Y) const;
	int GetIndexForGridCoordinates(int X, int Y) const;
	FVector2D GetPositionForGridCoordinates(int X, int Y) const;

	// Other things needed to generate the mesh
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	
};
