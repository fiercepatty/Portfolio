// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "FastNoiseWrapper.h"
#include "ProceduralTerrainComponent.generated.h"

UENUM(BlueprintType)
enum class EComponentShapes: uint8
{
	Ve_Plain UMETA(DisplayName="Plain"),
	
	Ve_Cube UMETA(DisplayName="Cube"),
	
	Ve_Sphere	UMETA(DisplayName = "Sphere")
	
};
UENUM(BlueprintType)
enum class EShapeSide: uint8
{
	/**Positive Z Axis*/
	Ve_Top		UMETA(DisplayName="Top"),

	/**Negative Z Axis*/
	Ve_Bottom	UMETA(DisplayName="Bottom"),

	/**Negative Y Axis*/
	Ve_Left		UMETA(DisplayName = "Left"),

	/**Positive Y Axis*/
	Ve_Right	UMETA(DisplayName = "Right"),

	/**Positive X Axis*/
	Ve_Front	UMETA(DisplayName = "Front"),

	/**Negative X Axis*/
	Ve_Back		UMETA(DisplayName = "Back")
};
UENUM(BlueprintType)
enum class ESubSections: uint8
{
	Ve_One			UMETA(DisplayName="1"),

	Ve_Two			UMETA(DisplayName="2"),
	
	Ve_Four			UMETA(DisplayName="4"),

	Ve_Eight		UMETA(DisplayName="8"),
	
	Ve_Sixteen		UMETA(DisplayName="16"),

	Ve_ThirtyTwo	UMETA(DisplayName="32"),
	
	Ve_SixtyFour	UMETA(DisplayName="64")
};

UENUM(BlueprintType)
enum class EEdgeOfGeometricObject: uint8
{
	Ve_NorthStart		UMETA(DisplayName="North Start"),

	Ve_NorthMiddle		UMETA(DisplayName="North Middle"),
	
	Ve_NorthEnd			UMETA(DisplayName="North End"),

	Ve_SouthStart		UMETA(DisplayName="South Start"),

	Ve_SouthMiddle		UMETA(DisplayName="South Middle"),
	
	Ve_SouthEnd			UMETA(DisplayName="South End"),
	
	Ve_EastBottom		UMETA(DisplayName="East Bottom"),

	Ve_EastTop			UMETA(DisplayName="East Top"),
	
	Ve_EastEndTop		UMETA(DisplayName="East End Top"),

	Ve_EastEndMiddle	UMETA(DisplayName="East End Middle"),

	Ve_EastEndBottom	UMETA(DisplayName="East End Bottom"),

	Ve_WestBottom		UMETA(DisplayName="West Bottom"),

	Ve_WestTop			UMETA(DisplayName="West Top"),
	
	Ve_WestEndTop		UMETA(DisplayName="West End Top"),

	Ve_WestEndMiddle	UMETA(DisplayName="West End Middle"),

	Ve_WestEndBottom	UMETA(DisplayName="West End Bottom"),

	Ve_AllEnds			UMETA(DisplayName="All Ends"),

	Ve_None				UMETA(DisplayName= "None")
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_API UProceduralTerrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralTerrainComponent();
	
	UFUNCTION(BlueprintCallable)
	void GenerateMap(FVector StartingLocation,EComponentShapes ComponentShape = EComponentShapes::Ve_Plain, EShapeSide SideOfShape= EShapeSide::Ve_Top,ESubSections ShapeSection = ESubSections::Ve_One);
	bool IsGenerated() const;

	bool IsVisible() const;

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

	void SetEdgeOfGeometricObject(EEdgeOfGeometricObject GeometricObject);

private:
	EEdgeOfGeometricObject EdgeOfGeometricObject;
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
	UProceduralMeshComponent* ProceduralMesh;

	void GenerateVertices();
	void GenerateTriangles();
	void GenerateMesh() const;

	float GetNoiseValueForGridCoordinates(int X, int Y, int Z) const;
	int GetIndexForGridCoordinates(int X, int Y) const;
	FVector2D GetPositionForGridCoordinates(int X, int Y) const;

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
