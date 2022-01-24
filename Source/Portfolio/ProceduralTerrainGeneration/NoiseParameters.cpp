// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/NoiseParameters.h"

// Sets default values for this component's properties
UNoiseParameters::UNoiseParameters()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UNoiseParameters::InitializeVariables(const UNoiseParameters* NoiseValues)
{
	 NoiseComponentStartLocation= NoiseValues->NoiseComponentStartLocation;
	
	 NoiseResolution = NoiseValues->NoiseResolution;

	 TotalSizeToGenerate = NoiseValues->TotalSizeToGenerate;
	
	 NoiseInputScale = NoiseValues->NoiseInputScale;

	 NoiseOutputScale = NoiseValues->NoiseOutputScale;

	 NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	
	 NoiseType = NoiseValues->NoiseType;

	 Seed = NoiseValues->Seed;

	 Frequency = NoiseValues->Frequency;

	 Interp = NoiseValues->Interp;

	 FractalType =NoiseValues->FractalType;

	 Octaves = NoiseValues->Octaves;

	 Lacunarity = NoiseValues->Lacunarity;

	 Gain = NoiseValues->Gain;

	 CellularJitter = NoiseValues->CellularJitter;

	 CellularDistanceFunction = NoiseValues->CellularDistanceFunction;

	 CellularReturnType = NoiseValues->CellularReturnType;
	
	 FastNoise =NoiseValues->FastNoise;
}
