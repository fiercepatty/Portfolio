// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/TerrainParameters.h"

// Sets default values for this component's properties
UTerrainParameters::UTerrainParameters()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UTerrainParameters::InitializeVariables(const UTerrainParameters* TerrainValues)
{
	TerrainShape=TerrainValues->TerrainShape;

	TerrainShapeSide=TerrainValues->TerrainShapeSide;

	TerrainSubSections=TerrainValues->TerrainSubSections;
	
	CurrentIndexFromActiveLandscape = TerrainValues->CurrentIndexFromActiveLandscape;
	
	LandscapeAmount= TerrainValues->LandscapeAmount;

	ChunkViewDistance= TerrainValues->ChunkViewDistance;

	bActiveTerrain = TerrainValues->bActiveTerrain;
	
	bVisibleTerrain =TerrainValues->bVisibleTerrain;

	bClampTop =TerrainValues->bClampTop;
	bClampBottom = TerrainValues->bClampBottom;
	bClampLeft = TerrainValues->bClampLeft;
	bClampRight =TerrainValues->bClampRight;
}
