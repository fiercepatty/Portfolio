// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "FastNoiseWrapper.h"
#include "ProceduralTerrainComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_API UProceduralTerrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralTerrainComponent();

	
	
	UFUNCTION(BlueprintCallable)
	void GenerateMap(FVector StartingLocation);


	bool IsGenerated() const;

	bool IsVisible() const;

	void UnLoadMesh();

	void LoadMesh();

	FVector NoiseComponentStartLocation;

	int CurrentIndexFromActiveLandscape = 999;
	
	int LandscapeAmount=3;
	
	int NoiseResolution = 300;

	int TotalSizeToGenerate = 1200;
	
	float NoiseInputScale = 0.5;

	float NoiseOutputScale = 2000;

	int NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	
	int ChunkViewDistance=3;
	
	EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::Simplex;

	int32 Seed = 1337;

	float Frequency = 0.01f;

	EFastNoise_Interp Interp = EFastNoise_Interp::Quintic;

	EFastNoise_FractalType FractalType = EFastNoise_FractalType::FBM;

	int32 Octaves = 3;

	float Lacunarity = 2.0f;

	float Gain = 0.5f;

	float CellularJitter = 0.45f;

	EFastNoise_CellularDistanceFunction CellularDistanceFunction = EFastNoise_CellularDistanceFunction::Euclidean;

	EFastNoise_CellularReturnType CellularReturnType = EFastNoise_CellularReturnType::CellValue;
	
	UPROPERTY(VisibleAnywhere, Category = "Terrian Generation | Noise Settings")
	UFastNoiseWrapper* FastNoise;

	int VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;

private:
	
	FVector ComponentLocation;
	bool bGenerated =false;
	bool bIsVisible = false;
	TArray<FVector> Vertices;
	TArray<int> Triangles;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralMesh;


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
