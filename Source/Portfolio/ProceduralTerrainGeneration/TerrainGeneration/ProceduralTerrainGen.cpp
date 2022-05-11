// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGen.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AProceduralTerrainGen::AProceduralTerrainGen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultScene"));
	if(!ProceduralTerrain)
	{
		ProceduralTerrain = CreateDefaultSubobject<UProceduralTerrainComponent>(TEXT("ProceduralTerrain"));
		ProceduralTerrain->ProceduralTerrainMesh->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	}
	if(!ProceduralWater)
	{
		ProceduralWater=CreateDefaultSubobject<UProceduralWaterComponent>(TEXT("ProceduralWater"));
		ProceduralWater->ProceduralWaterMesh->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	}
	//Create and attach the trigger box to the root component that is the procedural mesh
	TerrainTriggerBox=CreateDefaultSubobject<UBoxComponent>(TEXT("TerrainTriggerBox"));
	TerrainTriggerBox->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	TerrainTriggerBox->SetCollisionProfileName(FName(TEXT("Trigger")));
	TerrainTriggerBox->SetGenerateOverlapEvents(true);	
	


	
}

void AProceduralTerrainGen::GenerateTerrain()
{
	GenerateCurrentLandscape();
	//After we generate the landscape we want to find any and all connected landscapes
	FindConnections(this);
}


void AProceduralTerrainGen::FindConnections(AProceduralTerrainGen* CurrentTerrain) const
{
	//We want to look in more of a center location instead of the bottom left corner so that if we overlap with something we know that it is the correct mesh
	FVector StartLocation =CurrentTerrain->GetActorLocation() +FVector(ProceduralTerrain->NoiseResolution,ProceduralTerrain->NoiseResolution,0);
	//If we dont have a north then we look for one
	if(!CurrentTerrain->NorthTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation+FVector(ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0,0),ProceduralTerrain->FastNoiseOutputScale);
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
	//If we dont have a east then we look for one
	if(!CurrentTerrain->EastTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation+FVector(0,ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0),ProceduralTerrain->FastNoiseOutputScale);
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
	//If we dont have a south we look for one
	if(!CurrentTerrain->SouthTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation-FVector(ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0,0),ProceduralTerrain->FastNoiseOutputScale);
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
	//If we dont have a west we look for one
	if(!CurrentTerrain->WestTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation-FVector(0,ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0),ProceduralTerrain->FastNoiseOutputScale);
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

	//Make the start and end where the desired location is but with a negative z for the output scale
	//and positive still we use the start location because if we generated the map at a location that is
	//not 0,0,0 then we would not be able to reliably hit it with a line trace so we just take what the max height
	//that the terrain could be and we subtract that from the z and we also add it to the z to keep our line trace relative
	FVector Start = FVector(StartLocation.X,StartLocation.Y,StartLocation.Z-OutputScale);
	FVector End =  FVector(StartLocation.X,StartLocation.Y,StartLocation.Z +OutputScale);
	

	FCollisionQueryParams CollisionParams;

	//Uncomment to get a debug trace for where we are doing the line traces
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 25, 0, 5);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);
	return OutHit;
}


void AProceduralTerrainGen::GenerateCurrentLandscape() const
{
	ProceduralTerrain->GenerateMap(NoiseComponentStartLocation);
	if(bWaterEnabled)
		ProceduralWater->GenerateMap(NoiseComponentStartLocation);
	else
	{
		ProceduralWater->DestroyComponent();
	}
}

void AProceduralTerrainGen::InitializeVariable(const FTerrainInfo TerrainInfo, TArray<FTerrainInfo> ConnectedTerrainInfos) const
{
	//Init the Fast Noise or the terrain map and the Connection noise until I get a better solution for the connection
	ProceduralTerrain->InitializeFastNoise(TerrainInfo);
	ProceduralTerrain->InitializeConnectionNoise(ConnectedTerrainInfos);

	//Creating the trigger box the box extent is just the same size as the map generated. 
	TerrainTriggerBox->SetBoxExtent(FVector((TerrainInfo.TotalSizeToGenerate-TerrainInfo.NoiseResolution)*0.5 ,(TerrainInfo.TotalSizeToGenerate-TerrainInfo.NoiseResolution)*0.5 ,TerrainInfo.NoiseOutputScale*2));

	//And we need to make it so the box is in the center of the terrain so we off set it by half of the size of the map on x and y
	TerrainTriggerBox->SetRelativeLocation(FVector((TerrainInfo.TotalSizeToGenerate)*0.5,(TerrainInfo.TotalSizeToGenerate)*0.5,0));
}

void AProceduralTerrainGen::InitializeVariable(const FTerrainInfo TerrainInfo, TArray<FTerrainInfo> ConnectedTerrainInfos, FWaterInfo WaterInfo)
{
	//Init the Fast Noise or the terrain map and the Connection noise until I get a better solution for the connection
	ProceduralTerrain->InitializeFastNoise(TerrainInfo);
	ProceduralTerrain->InitializeConnectionNoise(ConnectedTerrainInfos);

	//Creating the trigger box the box extent is just the same size as the map generated. 
	TerrainTriggerBox->SetBoxExtent(FVector((TerrainInfo.TotalSizeToGenerate-TerrainInfo.NoiseResolution)*0.5 ,(TerrainInfo.TotalSizeToGenerate-TerrainInfo.NoiseResolution)*0.5 ,TerrainInfo.NoiseOutputScale*2));

	//And we need to make it so the box is in the center of the terrain so we off set it by half of the size of the map on x and y
	TerrainTriggerBox->SetRelativeLocation(FVector((TerrainInfo.TotalSizeToGenerate)*0.5,(TerrainInfo.TotalSizeToGenerate)*0.5,0));

	//Setup the water level
	WaterInfo.NoiseOutputScale=TerrainInfo.NoiseOutputScale;
	
	ProceduralWater->InitializeWaterComponent(WaterInfo);
	bWaterEnabled=true;
}

void AProceduralTerrainGen::LoadTerrain() const
{
	if(bWaterEnabled)
		ProceduralWater->LoadMesh();
	ProceduralTerrain->LoadMesh();
	TerrainTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AProceduralTerrainGen::UnloadTerrain() const
{
	if(bWaterEnabled)
		ProceduralWater->UnLoadMesh();
	ProceduralTerrain->UnLoadMesh();
	TerrainTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
