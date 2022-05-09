// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGen.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AProceduralTerrainGen::AProceduralTerrainGen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	if(!ProceduralTerrain)
	{
		ProceduralTerrain = CreateDefaultSubobject<UProceduralTerrainComponent>(TEXT("ProceduralTerrain"));
	}
	
}

void AProceduralTerrainGen::GenerateTerrain()
{
	GenerateCurrentLandscape();
	FindConnections(this);
}


void AProceduralTerrainGen::FindConnections(AProceduralTerrainGen* CurrentTerrain) const
{
	FVector StartLocation =CurrentTerrain->GetActorLocation() +FVector(ProceduralTerrain->NoiseResolution,ProceduralTerrain->NoiseResolution,0);
	if(!CurrentTerrain->NorthTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation+FVector(ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0,0),ProceduralTerrain->NoiseOutputScale);
		if(Out.IsValidBlockingHit())
		{
			AProceduralTerrainGen* Gen =Cast<AProceduralTerrainGen>(Out.Actor);
			if(Gen)
			{
				CurrentTerrain->NorthTerrainGenerated=Gen;
				if(!Gen->SouthTerrainGenerated)
					Gen->SouthTerrainGenerated=CurrentTerrain;
			}
		}
	}
	if(!CurrentTerrain->EastTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation+FVector(0,ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0),ProceduralTerrain->NoiseOutputScale);
		if(Out.IsValidBlockingHit())
		{
			AProceduralTerrainGen* Gen =Cast<AProceduralTerrainGen>(Out.Actor);
			if(Gen)
			{
				CurrentTerrain->EastTerrainGenerated=Gen;
				if(!Gen->WestTerrainGenerated)
					Gen->WestTerrainGenerated=CurrentTerrain;
			}
		}
	}
	if(!CurrentTerrain->SouthTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation-FVector(ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0,0),ProceduralTerrain->NoiseOutputScale);
		if(Out.IsValidBlockingHit())
		{
			AProceduralTerrainGen* Gen =Cast<AProceduralTerrainGen>(Out.Actor);
			if(Gen)
			{
				CurrentTerrain->SouthTerrainGenerated=Gen;
				if(!Gen->NorthTerrainGenerated)
					Gen->NorthTerrainGenerated=CurrentTerrain;
			}
		}
	}
	if(!CurrentTerrain->WestTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation-FVector(0,ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0),ProceduralTerrain->NoiseOutputScale);
		if(Out.IsValidBlockingHit())
		{
			AProceduralTerrainGen* Gen =Cast<AProceduralTerrainGen>(Out.Actor);
			if(Gen)
			{
				CurrentTerrain->WestTerrainGenerated=Gen;
				if(!Gen->EastTerrainGenerated)
					Gen->EastTerrainGenerated=CurrentTerrain;
			}
		}
	}
}


 FHitResult AProceduralTerrainGen::LineTrace(FVector StartLocation, float OutputScale) const
 {
	FHitResult OutHit;

	FVector Start = FVector(StartLocation.X,StartLocation.Y,StartLocation.Z-OutputScale);
	FVector End =  FVector(StartLocation.X,StartLocation.Y,StartLocation.Z +OutputScale);
	//GEngine->AddOnScreenDebugMessage(0,15,FColor::Green,Start.ToString());
	//GEngine->AddOnScreenDebugMessage(1,15,FColor::Red,End.ToString());

	FCollisionQueryParams CollisionParams;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 25, 0, 5);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);
	return OutHit;
}


void AProceduralTerrainGen::GenerateCurrentLandscape() const
{
	ProceduralTerrain->GenerateMap(NoiseComponentStartLocation);
}

void AProceduralTerrainGen::InitializeVariable(const FTerrainInfo TerrainInfo) const
{
	ProceduralTerrain->NoiseResolution=TerrainInfo.NoiseResolution;
	ProceduralTerrain->NoiseInputScale=TerrainInfo.NoiseInputScale;
	ProceduralTerrain->NoiseOutputScale=TerrainInfo.NoiseOutputScale;
	ProceduralTerrain->TotalSizeToGenerate=TerrainInfo.TotalSizeToGenerate;
	ProceduralTerrain->NoiseType=TerrainInfo.NoiseType;
	ProceduralTerrain->Seed=TerrainInfo.Seed;
	ProceduralTerrain->Frequency=TerrainInfo.Frequency;
	ProceduralTerrain->Interp=TerrainInfo.Interp;
	ProceduralTerrain->FractalType=TerrainInfo.FractalType;
	ProceduralTerrain->Octaves=TerrainInfo.Octaves;
	ProceduralTerrain->Lacunarity=TerrainInfo.Lacunarity;
	ProceduralTerrain->Gain=TerrainInfo.Gain;
	ProceduralTerrain->CellularJitter=TerrainInfo.CellularJitter;
	ProceduralTerrain->CellularDistanceFunction=TerrainInfo.CellularDistanceFunction;
	ProceduralTerrain->CellularReturnType=TerrainInfo.CellularReturnType;
}
