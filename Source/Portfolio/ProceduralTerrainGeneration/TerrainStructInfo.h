#pragma once
#include "FastNoiseWrapper.h"
#include "TerrainStructInfo.generated.h"

USTRUCT(BlueprintType)
struct FTerrainInfo
{
	GENERATED_USTRUCT_BODY()

	FTerrainInfo()        // this is your default constructor which is required for a USTRUCT definition
	{
		// initialize things here if desired or required
	}

public:
	int NoiseResolution = 300;

	int TotalSizeToGenerate = 1200;
	
	float NoiseInputScale = 0.5;

	/**How tall the highest point and smallest point will be from one another*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float NoiseOutputScale = 2000;

	/**How the noise is calculated the algorithm that is used*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::Simplex;

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