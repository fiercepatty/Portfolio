#pragma once
#include "FastNoiseWrapper.h"
#include "TerrainStructInfo.generated.h"

USTRUCT(BlueprintType)
struct FTerrainInfo
{
	GENERATED_USTRUCT_BODY()

	FTerrainInfo()       
	{
		
	}

public:
	bool bAverageTerrainConnections=true;
	
	int NoiseResolution = 300;

	int TotalSizeToGenerate = 1200;
	
	float NoiseInputScale = 0.5;

	/**How tall the highest point and smallest point will be from one another*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float NoiseOutputScale = 2000;

	/**How the noise is calculated the algorithm that is used*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::Simplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	int32 Seed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float Frequency = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_Interp Interp = EFastNoise_Interp::Quintic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_FractalType FractalType = EFastNoise_FractalType::FBM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	int32 Octaves = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float Lacunarity = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float Gain = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float CellularJitter = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_CellularDistanceFunction CellularDistanceFunction = EFastNoise_CellularDistanceFunction::Euclidean;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_CellularReturnType CellularReturnType = EFastNoise_CellularReturnType::CellValue;
};


USTRUCT(BlueprintType)
struct FWaterInfo
{
	GENERATED_USTRUCT_BODY()

	FWaterInfo()        
	{
		
	}

public:
	/**The waters resolution so the larger this number is the less triangles it will be to create the water mesh but make sure this number is divisible by the TotalSizeToGenerate in the Manager*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	int NoiseResolution = 300;

	int TotalSizeToGenerate = 1200;
	
	/**At this Height Water will be generated at. So everything below this would be underwater. Keep this value lower than the NoiseOutputHeight so that there is land*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float WaterHeight = 300;

	/**How tall the highest point and smallest point will be from one another*/
	float NoiseOutputScale = 2000;


	
	
};


USTRUCT(BlueprintType)
struct FBiomeInfo
{
	GENERATED_USTRUCT_BODY()

	FBiomeInfo()        
	{
		
	}

public:
	/**How the noise is calculated the algorithm that is used*/
	EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::Simplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
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
	
};