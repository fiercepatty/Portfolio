// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/TerrainManager.h"


// Sets default values
ATerrainManager::ATerrainManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BiomeNoise=CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("Noise"));
	
}

void ATerrainManager::InitBiomeNoise() const
{
	BiomeNoise->SetupFastNoise(BiomeOptions.NoiseType, BiomeOptions.Seed, BiomeOptions.Frequency, BiomeOptions.Interp, BiomeOptions.FractalType,
				BiomeOptions.Octaves,BiomeOptions.Lacunarity,BiomeOptions.Gain,BiomeOptions.CellularJitter, BiomeOptions.CellularDistanceFunction, BiomeOptions.CellularReturnType);
}

int ATerrainManager::SelectedBiome(const FVector BiomeLocation) const
{
	//If we only have one option just return it
	if(AllTerrainOptions.Num() ==1)
		return 0;
	else
	{
		//Get the Noise Value for the Biome Location We need to add 1 to it because it is between -1, 1 so we make it between 0 and 2 and then we multiple by 0.5 because multiplication is faster than division
		float Noise = BiomeNoise->GetNoise2D(BiomeLocation.X,BiomeLocation.Y);
		Noise +=1;
		Noise = Noise * 0.5;

		//Here we have to divide because we have no way of know how many terrain options there will be. This will be how we separate when to use a certain terrain option over another
		//Basically if we have 3 options the range of 0-0.33 with give us index of 0 and then .34-.66 will give us index of 1 and .67 to 1 will give us index 2 allowing there to be a random chance but controlled change with seed picked by the user
		//And because we are using noise the noise tends to stay around the same values and has small increments so we could have a much more likely chance of having similar terrain options around the same terrain option
		const float Increment = 1.0 / AllTerrainOptions.Num();
		const int Value = Noise / Increment;
		int Index = AllTerrainOptions.Num()-1;
		Index -= Value;

		return Index;
	}
}

void ATerrainManager::GeneratePlane()
{
	InitBiomeNoise();
	for (int I=0;I<AllTerrainOptions.Num();I++)
	{
		AllTerrainOptions[I].NoiseResolution=NoiseResolution;
		AllTerrainOptions[I].TotalSizeToGenerate=TotalSizeToGenerate;
		AllTerrainOptions[I].bAverageTerrainConnections=bAverageConnection;
	}
	//Telling the water how much to generate
	WaterOptions.TotalSizeToGenerate=TotalSizeToGenerate;
	
	
	RemoveAllPlanes();
	//Creating The Starting Point
	Origin = SpawnTerrain(GetActorLocation());
	Origin->GenerateTerrain();
	Origin->NoiseComponentStartLocation=FVector(0);
	//Starting by creating the North and setting north in origin and south in North and Adding into the north square size
	Origin->NorthTerrainGenerated=SpawnTerrain(Origin->GetActorLocation(),Origin->NoiseComponentStartLocation,EDirection::VE_North);
	Origin->NorthTerrainGenerated->SouthTerrainGenerated=Origin;
	Origin->NorthTerrainGenerated->GenerateTerrain();
	North.Add(Origin->NorthTerrainGenerated);

	//Creating the East and setting East to the origin and west in the East and adding into east square size
	Origin->EastTerrainGenerated=SpawnTerrain(Origin->GetActorLocation(),Origin->NoiseComponentStartLocation,EDirection::VE_East);
	Origin->EastTerrainGenerated->WestTerrainGenerated=Origin;
	Origin->EastTerrainGenerated->GenerateTerrain();
	East.Add(Origin->EastTerrainGenerated);

	//Creating the South and then setting the South to the origin and the north in the south and adding into the south square size
	Origin->SouthTerrainGenerated=SpawnTerrain(Origin->GetActorLocation(),Origin->NoiseComponentStartLocation,EDirection::VE_South);
	Origin->SouthTerrainGenerated->NorthTerrainGenerated=Origin;
	Origin->SouthTerrainGenerated->GenerateTerrain();
	South.Add(Origin->SouthTerrainGenerated);

	//Creating the West and setting west to the origin and the east in the west and adding into the east square size
	Origin->WestTerrainGenerated=SpawnTerrain(Origin->GetActorLocation(),Origin->NoiseComponentStartLocation,EDirection::VE_West);
	Origin->WestTerrainGenerated->EastTerrainGenerated=Origin;
	Origin->WestTerrainGenerated->GenerateTerrain();
	West.Add(Origin->WestTerrainGenerated);

	GenerateSquareCorners();

	for(int I=1;I<VisibleRange;I++)
	{
		GenerateSquareLayer();
	}
}


void ATerrainManager::RemoveAllPlanes()
{
	//If we dont have a origin then there are no planes attached to this manager
	if(Origin)
	{
		//Creating an array of all the Terrain we need to delete
		TArray<AProceduralTerrainGen*> NeedDelete;
		//Adding all the origin's terrain in it is impossible for the origin not to have all four directions filled
		NeedDelete.Add(Origin->NorthTerrainGenerated);
		Origin->NorthTerrainGenerated->bGettingDestroyed=true;
		NeedDelete.Add(Origin->EastTerrainGenerated);
		Origin->EastTerrainGenerated->bGettingDestroyed=true;

		NeedDelete.Add(Origin->WestTerrainGenerated);
		Origin->WestTerrainGenerated->bGettingDestroyed=true;

		NeedDelete.Add(Origin->SouthTerrainGenerated);
		Origin->SouthTerrainGenerated->bGettingDestroyed=true;

		//Get rid of the origin we dont need it anymore
		Origin->Destroy();
		while(NeedDelete.Num()!=0)
		{
			//Take the last object out of the array and remove all instances of it from the array
			AProceduralTerrainGen* Current =NeedDelete.Top();
			NeedDelete.Remove(Current);
			//Get all the attached objects and add them to the array to be deleted only add an object if it hasn't been added already
			if(Current)
			{
				if(Current->NorthTerrainGenerated && !Current->NorthTerrainGenerated->bGettingDestroyed)
				{
					NeedDelete.Add(Current->NorthTerrainGenerated);
					Current->NorthTerrainGenerated->bGettingDestroyed=true;
				}
				if(Current->EastTerrainGenerated && !Current->EastTerrainGenerated->bGettingDestroyed)
				{
					NeedDelete.Add(Current->EastTerrainGenerated);
					Current->EastTerrainGenerated->bGettingDestroyed=true;
				}
				if(Current->SouthTerrainGenerated && !Current->SouthTerrainGenerated->bGettingDestroyed)
				{
					NeedDelete.Add(Current->SouthTerrainGenerated);
					Current->SouthTerrainGenerated->bGettingDestroyed=true;
				}
				if(Current->WestTerrainGenerated && !Current->WestTerrainGenerated->bGettingDestroyed)
				{
					NeedDelete.Add(Current->WestTerrainGenerated);
					Current->WestTerrainGenerated->bGettingDestroyed=true;
				}
				Current->Destroy();
			}
			NeedDelete.Shrink();
		}
		//Making sure that there are not any nullptr or anything in the arrays to break it later
		North.Empty();
		South.Empty();
		East.Empty();
		West.Empty();
		Origin=nullptr;
	}
}


void ATerrainManager::BeginPlay()
{
	Super::BeginPlay();
	//Just creating the world
	GeneratePlane();
}

FVector ATerrainManager::UpdateNoiseSamplingLocation(const FVector StartLocation, const EDirection Dir) const
{
	//used to tell the noise wrapper where to start indexing in the actual noise map
	FVector Result = StartLocation;
	const int NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	switch (Dir)
	{
	case EDirection::VE_North:
		{
			Result +=FVector(NoiseSamplesPerLine-1,0,0);
			break;
		}
	case EDirection::VE_South:
		{
			Result+=FVector(-(NoiseSamplesPerLine-1),0,0);
			break;
		}
	case EDirection::VE_East:
		{
			Result+=FVector(0,NoiseSamplesPerLine-1,0);
			break;
		}
	case EDirection::VE_West:
		{
			Result+=FVector(0,-(NoiseSamplesPerLine-1),0);
			break;
		}
	default: ;
	}

	return Result;
}

AProceduralTerrainGen* ATerrainManager::SpawnTerrain(const FVector Location,FVector SampleStart, const EDirection Dir)
{
	//Move the spawn of the terrain by the direction we are wanting to spawn it in. The terrain are TotalSize-NoiseResolution big so we are add or subtracting that on the correct axis
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	FVector SpawnLocation = Location;
	switch (Dir)
	{
	case EDirection::VE_North:
		{
			SpawnLocation+=FVector(TotalSizeToGenerate-NoiseResolution,0,0);
			break;
		}
	case EDirection::VE_South:
		{
			SpawnLocation-=FVector(TotalSizeToGenerate-NoiseResolution,0,0);
			break;
		}
	case EDirection::VE_East:
		{
			SpawnLocation+=FVector(0,TotalSizeToGenerate-NoiseResolution,0);
			break;
		}
	case EDirection::VE_West:
		{
			SpawnLocation-=FVector(0,TotalSizeToGenerate-NoiseResolution,0);
			break;
		}
	default: ;
	}

	
	//Spawn the terrain and update where we are sampling the noise from and initialize any variables needed
	AProceduralTerrainGen* TerrainGen = GetWorld()->SpawnActor<AProceduralTerrainGen>(SpawnLocation,GetActorRotation(),SpawnParams);
	TerrainGen->NoiseComponentStartLocation=UpdateNoiseSamplingLocation(SampleStart,Dir);
	const int Num =	SelectedBiome(TerrainGen->NoiseComponentStartLocation);
	const TArray<FTerrainInfo> Connections = FindConnectedTerrains(TerrainGen);

	if(WaterOptions.WaterHeight >= 0)
	{
		TerrainGen->InitializeVariable(AllTerrainOptions[Num],Connections,WaterOptions);
	}
	else
	{
		TerrainGen->InitializeVariable(AllTerrainOptions[Num],Connections);
	}
	return TerrainGen;
}

AProceduralTerrainGen* ATerrainManager::SpawnTerrain(FVector Location)
{
	//This is basically only for the origin map because it does not need to be offset at all when it was created
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	AProceduralTerrainGen* TerrainGen = GetWorld()->SpawnActor<AProceduralTerrainGen>(Location,GetActorRotation(),SpawnParams);
	const int Num =	SelectedBiome(TerrainGen->NoiseComponentStartLocation);
	const TArray<FTerrainInfo> Connections = FindConnectedTerrains(TerrainGen);

	if(WaterOptions.WaterHeight >= 0)
	{
		TerrainGen->InitializeVariable(AllTerrainOptions[Num],Connections,WaterOptions);
	}
	else
	{
		TerrainGen->InitializeVariable(AllTerrainOptions[Num],Connections);
	}
	return TerrainGen;
}
TArray<FTerrainInfo> ATerrainManager::FindConnectedTerrains(const AProceduralTerrainGen* CurrentTerrain)
{
	TArray<FTerrainInfo> TerrainInfos;
	const int NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	const FVector CurrentLocation = CurrentTerrain->NoiseComponentStartLocation;
	//North
	TerrainInfos.Add(AllTerrainOptions[SelectedBiome(CurrentLocation+FVector(NoiseSamplesPerLine-1,0,0))]);
	//NorthEast
	TerrainInfos.Add(AllTerrainOptions[SelectedBiome((CurrentLocation+FVector(NoiseSamplesPerLine-1,NoiseSamplesPerLine-1,0)))]);
	//East
	TerrainInfos.Add(AllTerrainOptions[SelectedBiome(CurrentLocation+FVector(0,NoiseSamplesPerLine-1,0))]);
	//SouthEast
	TerrainInfos.Add(AllTerrainOptions[SelectedBiome((CurrentLocation+FVector(-(NoiseSamplesPerLine-1),NoiseSamplesPerLine-1,0)))]);
	//South
	TerrainInfos.Add(AllTerrainOptions[SelectedBiome(CurrentLocation+FVector(-(NoiseSamplesPerLine-1),0,0))]);
	//SouthWest
	TerrainInfos.Add(AllTerrainOptions[SelectedBiome(CurrentLocation+FVector(-(NoiseSamplesPerLine-1),-(NoiseSamplesPerLine-1),0))]);
	//West
	TerrainInfos.Add(AllTerrainOptions[SelectedBiome(CurrentLocation+FVector(0,-(NoiseSamplesPerLine-1),0))]);
	//NorthWest
	TerrainInfos.Add(AllTerrainOptions[SelectedBiome(CurrentLocation+FVector(NoiseSamplesPerLine-1,-(NoiseSamplesPerLine-1),0))]);
	
	return TerrainInfos;
}


void ATerrainManager::GenerateSquareLayer()
{
	//For each layer on the square we will want to generate one more deep for the Map also know as if we are on the north side we go another one north and set that as our new most north points
	for(int Index =0;Index < North.Num();Index++)
	{
		AProceduralTerrainGen* Current = North[Index];
		Current->NorthTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_North);
		Current->NorthTerrainGenerated->SouthTerrainGenerated=Current;
		Current->NorthTerrainGenerated->GenerateTerrain();
		North[Index] = Current->NorthTerrainGenerated;
	}
	for(int Index =0;Index < East.Num();Index++)
	{
		AProceduralTerrainGen* Current = East[Index];
		Current->EastTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_East);
		Current->EastTerrainGenerated->WestTerrainGenerated=Current;
		Current->EastTerrainGenerated->GenerateTerrain();
		East[Index] = Current->EastTerrainGenerated;
	}
	for(int Index =0;Index < South.Num();Index++)
	{
		AProceduralTerrainGen* Current = South[Index];
		Current->SouthTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_South);
		Current->SouthTerrainGenerated->NorthTerrainGenerated=Current;
		Current->SouthTerrainGenerated->GenerateTerrain();
		South[Index] = Current->SouthTerrainGenerated;
	}
	for(int Index =0;Index < West.Num();Index++)
	{
		AProceduralTerrainGen* Current = West[Index];
		Current->WestTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_West);
		Current->WestTerrainGenerated->EastTerrainGenerated=Current;
		Current->WestTerrainGenerated->GenerateTerrain();
		West[Index] = Current->WestTerrainGenerated;
	}
	//Wee go clockwise around the square and get the corner that need to be generated and the arrays are in order so we made sure to keep them in order
	GenerateSquareCorners();
	
}

void ATerrainManager::GenerateSquareCorners()
{
	//Travelling Clockwise around the square and keeping order of the Square directions
	
	//Get the last item in the North Array and Create another terrain east and then add that item into the east of the current and then also add that item at the beginning of the East Square also add at the end of the North Square
	AProceduralTerrainGen* Current = North[North.Num()-1];
	Current->EastTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_East);
	Current->EastTerrainGenerated->WestTerrainGenerated=Current;
	Current->EastTerrainGenerated->GenerateTerrain();
	North.Add(Current->EastTerrainGenerated);
	East.Insert(Current->EastTerrainGenerated,0);

	//Current is changed to the last item in East and then we generate its south and then add that into the end of the East Square and the End of the South square
	Current = East[East.Num()-1];
	Current->SouthTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_South);
	Current->SouthTerrainGenerated->NorthTerrainGenerated=Current;
	Current->SouthTerrainGenerated->GenerateTerrain();
	East.Add(Current->SouthTerrainGenerated);
	South.Add(Current->SouthTerrainGenerated);

	//Current changed to the first idem in South and then generate west for it and then add into the beginning of the South Square and the end of the West square
	Current = South[0];
	Current->WestTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_West);
	Current->WestTerrainGenerated->EastTerrainGenerated=Current;
	Current->WestTerrainGenerated->GenerateTerrain();
	South.Insert(Current->WestTerrainGenerated,0);
	West.Add(Current->WestTerrainGenerated);

	//Current Becomes the first item in west and then generate north for it and then add into the beginning of West and the beginning of North
	Current = West[0];
	Current->NorthTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_North);
	Current->NorthTerrainGenerated->SouthTerrainGenerated=Current;
	Current->NorthTerrainGenerated->GenerateTerrain();
	West.Insert(Current->NorthTerrainGenerated,0);
	North.Insert(Current->NorthTerrainGenerated,0);

}



void ATerrainManager::MoveSquareNorth()
{
	//To move north all we need to do is access each terrain in the outer edge of the square and tell it to move north. If there is an attached piece just go north if not then we need to create it
	//Make sure to set all our meshes that we are moving to visible so we can actually see what we are going to. Also Make the layer on the south side not visible
	for(int Index =0;Index < North.Num();Index++)
	{
		AProceduralTerrainGen* Current = North[Index];
		if(Current->NorthTerrainGenerated)
		{
			North[Index] = Current->NorthTerrainGenerated;
			Current->NorthTerrainGenerated->LoadTerrain();
		}
		else
		{
			Current->NorthTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_North);
			Current->NorthTerrainGenerated->SouthTerrainGenerated=Current;
			Current->NorthTerrainGenerated->GenerateTerrain();
			North[Index] = Current->NorthTerrainGenerated;
		}
	}

	//East does not need to check to see if it exist because it is going in territory that was already made by north so it will always be there and the same can be said about West
	for(int Index =0;Index < East.Num();Index++)
	{
		const AProceduralTerrainGen* Current = East[Index];
		if(Current->NorthTerrainGenerated)
		{
			East[Index] = Current->NorthTerrainGenerated;
			Current->NorthTerrainGenerated->LoadTerrain();
		}
	}
	
	for(int Index =0;Index < West.Num();Index++)
	{
		const AProceduralTerrainGen* Current = West[Index];
		if(Current->NorthTerrainGenerated)
		{
			West[Index] = Current->NorthTerrainGenerated;
			Current->NorthTerrainGenerated->LoadTerrain();
		}
	}

	//North doesnt really need to check if it needs  to spawn anything in but we do need to unload any meshes that are currently on the south layer
	for(int Index =0;Index < South.Num();Index++)
	{
		AProceduralTerrainGen* Current = South[Index];
		if(Current->NorthTerrainGenerated)
		{
			South[Index] = Current->NorthTerrainGenerated;
			Current->NorthTerrainGenerated->LoadTerrain();
		}
		else
		{
			Current->NorthTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_North);
			Current->NorthTerrainGenerated->SouthTerrainGenerated=Current;
			Current->NorthTerrainGenerated->GenerateTerrain();
			South[Index] = Current->NorthTerrainGenerated;
		}
		Current->UnloadTerrain();
	}
	
}

void ATerrainManager::MoveSquareEast()
{
	//East we are travelling in new lands that could be undiscovered so we need to make sure we can create any mesh we need and just make the rest visible
	for(int Index =0;Index < East.Num();Index++)
	{
		AProceduralTerrainGen* Current = East[Index];
		if(Current->EastTerrainGenerated)
		{
			East[Index] = Current->EastTerrainGenerated;
			Current->EastTerrainGenerated->LoadTerrain();
		}
		else
		{
			Current->EastTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_East);
			Current->EastTerrainGenerated->WestTerrainGenerated=Current;
			Current->EastTerrainGenerated->GenerateTerrain();
			East[Index] = Current->EastTerrainGenerated;
		}
	}

	//Just making the north and south arrays being up to date with what meshes are there so when we move in those directions it does not get weird and glitch
	for(int Index =0;Index < North.Num();Index++)
	{
		const AProceduralTerrainGen* Current = North[Index];
		if(Current->EastTerrainGenerated)
		{
			North[Index] = Current->EastTerrainGenerated;
			Current->EastTerrainGenerated->LoadTerrain();
		}
	}

	for(int Index =0;Index < South.Num();Index++)
	{
		const AProceduralTerrainGen* Current = South[Index];
		if(Current->EastTerrainGenerated)
		{
			South[Index] = Current->EastTerrainGenerated;
			Current->EastTerrainGenerated->LoadTerrain();
		}
	}

	//Unloading the meshes we need to but also dont really need to check if we are spawning anything in because it is not going in any new lands
	for(int Index =0;Index < West.Num();Index++)
	{
		AProceduralTerrainGen* Current = West[Index];
		if(Current->EastTerrainGenerated)
		{
			West[Index] = Current->EastTerrainGenerated;
			Current->EastTerrainGenerated->LoadTerrain();
		}
		else
		{
			Current->EastTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_East);
			Current->EastTerrainGenerated->WestTerrainGenerated=Current;
			Current->EastTerrainGenerated->GenerateTerrain();
			West[Index] = Current->EastTerrainGenerated;
		}
		Current->UnloadTerrain();
	}
}

void ATerrainManager::MoveSquareSouth()
{
	//Same idea as north just make visible or spawn what is need and update the rest of the meshes where the north link is being unloaded
	for(int Index =0;Index < South.Num();Index++)
	{
		AProceduralTerrainGen* Current = South[Index];
		if(Current->SouthTerrainGenerated)
		{
			South[Index] = Current->SouthTerrainGenerated;
			Current->SouthTerrainGenerated->LoadTerrain();
		}
		else
		{
			Current->SouthTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_South);
			Current->SouthTerrainGenerated->NorthTerrainGenerated=Current;
			Current->SouthTerrainGenerated->GenerateTerrain();
			South[Index] = Current->SouthTerrainGenerated;
		}
	}

	for(int Index =0;Index < East.Num();Index++)
	{
		const AProceduralTerrainGen* Current = East[Index];
		if(Current->SouthTerrainGenerated)
		{
			East[Index] = Current->SouthTerrainGenerated;
			Current->SouthTerrainGenerated->LoadTerrain();
		}
	}
	
	for(int Index =0;Index < West.Num();Index++)
	{
		const AProceduralTerrainGen* Current = West[Index];
		if(Current->SouthTerrainGenerated)
		{
			West[Index] = Current->SouthTerrainGenerated;
			Current->SouthTerrainGenerated->LoadTerrain();
		}
	}

	for(int Index =0;Index < North.Num();Index++)
	{
		AProceduralTerrainGen* Current = North[Index];
		if(Current->SouthTerrainGenerated)
		{
			North[Index] = Current->SouthTerrainGenerated;
			Current->SouthTerrainGenerated->LoadTerrain();
		}
		else
		{
			Current->SouthTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_South);
			Current->SouthTerrainGenerated->NorthTerrainGenerated=Current;
			Current->SouthTerrainGenerated->GenerateTerrain();
			North[Index] = Current->SouthTerrainGenerated;
		}
		Current->UnloadTerrain();
	}
}

void ATerrainManager::MoveSquareWest()
{
	//Same idea as east just make visible or spawn what is need and update the rest of the meshes where the east link is being unloaded

	for(int Index =0;Index < West.Num();Index++)
	{
		AProceduralTerrainGen* Current = West[Index];
		if(Current->WestTerrainGenerated)
		{
			West[Index] = Current->WestTerrainGenerated;
			Current->WestTerrainGenerated->LoadTerrain();
		}
		else
		{
			Current->WestTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_West);
			Current->WestTerrainGenerated->EastTerrainGenerated=Current;
			Current->WestTerrainGenerated->GenerateTerrain();
			West[Index] = Current->WestTerrainGenerated;
		}
	}

	for(int Index =0;Index < North.Num();Index++)
	{
		const AProceduralTerrainGen* Current = North[Index];
		if(Current->WestTerrainGenerated)
		{
			North[Index] = Current->WestTerrainGenerated;
			Current->WestTerrainGenerated->LoadTerrain();
		}
	}

	for(int Index =0;Index < South.Num();Index++)
	{
		const AProceduralTerrainGen* Current = South[Index];
		if(Current->WestTerrainGenerated)
		{
			South[Index] = Current->WestTerrainGenerated;
			Current->WestTerrainGenerated->LoadTerrain();
		}
	}

	for(int Index =0;Index < East.Num();Index++)
	{
		AProceduralTerrainGen* Current = East[Index];
		if(Current->WestTerrainGenerated)
		{
			East[Index] = Current->WestTerrainGenerated;
			Current->WestTerrainGenerated->LoadTerrain();
		}
		else
		{
			Current->WestTerrainGenerated=SpawnTerrain(Current->GetActorLocation(),Current->NoiseComponentStartLocation,EDirection::VE_West);
			Current->WestTerrainGenerated->EastTerrainGenerated=Current;
			Current->WestTerrainGenerated->GenerateTerrain();
			East[Index] = Current->WestTerrainGenerated;
		}
		Current->UnloadTerrain();
	}
}




