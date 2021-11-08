// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FastNoiseWrapper.h"
#include "NoiseParameters.h"
#include "Components/RuntimeMeshComponentStatic.h"
#include "TerrainEnums.h"
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
	void GenerateMap(FVector StartingLocation, EShapeSide SideOfShape,ESubSections ShapeSection);
	void GenerateMapSphere(FVector StartingLocation,EShapeSide SideOfShape,ESubSections ShapeSection);
	bool IsGenerated() const;

	bool IsVisible() const;

	void UnLoadMesh();

	void LoadMesh();

	bool bClampTop =false;
	bool bClampBottom = false;
	bool bClampLeft = false;
	bool bClampRight =false;

	void InitializeVariables(UNoiseParameters* NoiseParameters);

	

	void SetClamping(const bool Top, const bool Bottom, const bool Right,const bool Left) {bClampTop=Top;bClampLeft=Left;bClampRight=Right;bClampBottom=Bottom;};

private:
	EComponentShapes Shapes;
	EShapeSide ShapeSide;
	ESubSections ShapeSubSections;
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
	URuntimeMeshComponentStatic* ProceduralMesh;

	void GenerateVertices();
	void GenerateTriangles();
	void GenerateMesh() const;
	float GetNoiseValueForGridCoordinates(int X, int Y, int Z) const;
	int GetIndexForGridCoordinates(int X, int Y) const;
	FVector2D GetPositionForGridCoordinates(int X, int Y) const;

	// Other things needed to generate the mesh
	TArray<FVector> Normals;
	TArray<FRuntimeMeshTangent> Tangents;
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
