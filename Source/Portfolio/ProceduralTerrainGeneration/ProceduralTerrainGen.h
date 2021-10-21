// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralTerrainComponent.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrainGen.generated.h"
UENUM(BlueprintType)
enum class EDirectionException: uint8
{
	Ve_None UMETA(DisplayName="None"),
	
	Ve_North UMETA(DisplayName="North"),
	
	Ve_South	UMETA(DisplayName = "South"),
	
	Ve_East		UMETA(DisplayName = "East"),
	
	Ve_West	UMETA(DisplayName = "West")

};

UCLASS()
class PORTFOLIO_API AProceduralTerrainGen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralTerrainGen();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	bool bActiveTerrain = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	bool bVisibleTerrain =true;
	
	bool bNeedDestroyed = false;
	bool bNeverDestroy = false;

	UPROPERTY(VisibleAnywhere)
	UProceduralTerrainComponent* ProceduralTerrain;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void GenerateTerrain();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void DeleteTerrain();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void UpdateVisibleTerrain();

	void RecursiveActiveCheck(int IndexFromActiveLandscape, EDirectionException PreviousDirectionException, int ViewDistance);

	void GenerateSiblingLandscapes(int IndexFromActiveLandscape, EDirectionException PreviousDirectionException);
	
	void GenerateCurrentLandscape() const;

	void SetCurrentIndexFromActiveLandscape(int Index);

	int GetCurrentIndexFromActiveLandscape() const;

	void SetLandscapeAmount(int LandscapeViewAmount);

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

	void InitializeVariable(AProceduralTerrainGen* Proc) const;
	
	void RecursiveDelete();

	void SetChunkViewDistance(int ChunkDistance);

	void SetNoiseSamplingPerLine(int NoiseSampling);

	void SetTerrainShape(EComponentShapes const ComponentShapes){TerrainShape=ComponentShapes;}

	void SetTerrainShapeSide(EShapeSide const ShapeSide){TerrainShapeSide=ShapeSide;}

	void SetTerrainSubSections(ESubSections const SubSections) {TerrainSubSections=SubSections;}


private:
	static void MakeVisibleComponent(AProceduralTerrainGen* TerrainGen, int IndexFromActiveLandscape );

	AProceduralTerrainGen* GenerateVisibleComponent(EDirectionException PreviousDirectionException,int IndexFromActiveLandscape, EShapeSide TerrainSide= EShapeSide::Ve_Top);

	AProceduralTerrainGen* CreateProceduralTerrain(int PosX, int PosY,int PosZ, int CurrentLandscapeIndex, EDirectionException PreviousDirectionException, EShapeSide TerrainSide);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	AProceduralTerrainGen* EastTerrainGenerated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	AProceduralTerrainGen* WestTerrainGenerated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	AProceduralTerrainGen* NorthTerrainGenerated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	AProceduralTerrainGen* SouthTerrainGenerated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	FVector NoiseComponentStartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	EComponentShapes TerrainShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	EShapeSide TerrainShapeSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	ESubSections TerrainSubSections;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	int CurrentIndexFromActiveLandscape = 999;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	int LandscapeAmount=3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	int NoiseResolution = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	int TotalSizeToGenerate = 1200;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	float NoiseInputScale = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	float NoiseOutputScale = 2000;

	int NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	int ChunkViewDistance=3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::Simplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	int32 Seed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	float Frequency = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	EFastNoise_Interp Interp = EFastNoise_Interp::Quintic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	EFastNoise_FractalType FractalType = EFastNoise_FractalType::FBM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	int32 Octaves = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	float Lacunarity = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	float Gain = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	float CellularJitter = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	EFastNoise_CellularDistanceFunction CellularDistanceFunction = EFastNoise_CellularDistanceFunction::Euclidean;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (AllowPrivateAccess = "true"))
	EFastNoise_CellularReturnType CellularReturnType = EFastNoise_CellularReturnType::CellValue;
	
	UPROPERTY(VisibleAnywhere, Category = "Terrian Generation | Noise Settings")
	UFastNoiseWrapper* FastNoise;

	static int ConvertEnumSubSectionsToInteger(ESubSections AShapeSubSections);

};


