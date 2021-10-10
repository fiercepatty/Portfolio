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

	bool IsGenerated();

	bool IsVisible();

	void UnLoadMesh();

	void LoadMesh();

	/** Set Noise Type*/
	void SetNoiseTypeComponent(const EFastNoise_NoiseType NewNoiseType);

	/** Set seed. */
	void SetSeed(const int32 NewSeed);

	/** Set frequency. */
	void SetFrequency(const float NewFrequency);

	/** Set interpolation type. */
	void SetInterpolation(const EFastNoise_Interp NewInterpolation);

	/** Set fractal type. */
	void SetFractalType(const EFastNoise_FractalType NewFractalType);

	/** Set fractal octaves. */
	void SetOctaves(const int32 NewOctaves);

	/** Set fractal lacunarity. */
	void SetLacunarity(const float NewLacunarity);

	/** Set fractal gain. */
	void SetGain(const float NewGain);

	/** Set cellular jitter. */
	void SetCellularJitter(const float NewCellularJitter);

	/** Set cellular distance function. */
	void SetDistanceFunction(const EFastNoise_CellularDistanceFunction NewDistanceFunction);

	/** Set cellular return type. */
	void SetReturnType(const EFastNoise_CellularReturnType NewCellularReturnType);

	void SetNoiseResolution(int NewNoiseResolution);

	void SetTotalSizeToGenerate(int NewTotalSizeToGenerate);

	void SetNoiseInputScale(float NewNoiseInputScale);

	void SetNoiseOutputScale(float NewNoiseOutputScale);

private:
	FVector ComponentLocation;
	bool bGenerated =false;
	bool bIsVisible = false;
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	
	int NoiseResolution = 300;

	int TotalSizeToGenerate = 1200;
	
	int NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	int VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;

	float NoiseInputScale = 0.5; // Making this smaller will "stretch" the perlin noise terrain

	float NoiseOutputScale = 2000; // Making this bigger will scale the terrain's height

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralMesh;

	void GenerateVertices();
	void GenerateTriangles();
	void GenerateMesh();

	float GetNoiseValueForGridCoordinates(int x, int y);
	int GetIndexForGridCoordinates(int x, int y);
	FVector2D GetPositionForGridCoordinates(int x, int y);

	// Other things needed to generate the mesh
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	
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
	
	UPROPERTY(VisibleAnywhere)
	UFastNoiseWrapper* FastNoise;
};
