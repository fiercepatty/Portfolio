// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGen.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AProceduralTerrainGen::AProceduralTerrainGen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	if(!ProceduralTerrain)
	{
		ProceduralTerrain = CreateDefaultSubobject<UProceduralTerrainComponent>(TEXT("ProceduralTerrain"));
	}
}


void AProceduralTerrainGen::GenerateTerrain()
{
	bNeverDestroy=true;
	RecursiveDelete();
	NoiseComponentStartLocation = FVector(0,0,0);
	NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	CurrentIndexFromActiveLandscape=0;
	GenerateCurrentLandscape();
	GenerateSiblingLandscapes(1, EDirectionException::VE_None);
}

void AProceduralTerrainGen::DeleteTerrain()
{
	bNeverDestroy=true;
	RecursiveDelete();
}

void AProceduralTerrainGen::UpdateVisibleTerrain()
{
	bActiveTerrain=true;
	bVisibleTerrain=true;
	RecursiveActiveCheck(1,EDirectionException::VE_None,ChunkViewDistance);
}

void AProceduralTerrainGen::RecursiveActiveCheck(const int IndexFromActiveLandscape, EDirectionException PreviousDirectionException, int ViewDistance)
{
	bool bStartNorthRecursion=false;
	bool bStartSouthRecursion=false;
	bool bStartEastRecursion=false;
	bool bStartWestRecursion=false;
	
	bool bMakeNorthVisible=false;
	bool bMakeSouthVisible=false;
	bool bMakeEastVisible=false;
	bool bMakeWestVisible=false;

	bool bMakeNorthInvisible=false;
	bool bMakeEastInvisible=false;
	bool bMakeSouthInvisible=false;
	bool bMakeWestInvisible=false;



	if(IndexFromActiveLandscape<ViewDistance)
	{
		switch (PreviousDirectionException)
		{
		case EDirectionException::VE_None:
			{
				bMakeNorthVisible=true;
				bMakeEastVisible=true;
				bMakeSouthVisible=true;
				bMakeWestVisible=true;

				bStartNorthRecursion=true;
				bStartSouthRecursion=true;
				bStartEastRecursion=true;
				bStartWestRecursion=true;
				break;
			}
		case EDirectionException::VE_North:
			{
				bMakeNorthVisible=true;
				bMakeEastVisible=true;
				bMakeWestVisible=true;

				bStartNorthRecursion=true;
				bStartEastRecursion=true;
				bStartWestRecursion=true;
				break;
			}
		case EDirectionException::VE_South:
			{
				bMakeEastVisible=true;
				bMakeSouthVisible=true;
				bMakeWestVisible=true;

				bStartSouthRecursion=true;
				bStartEastRecursion=true;
				bStartWestRecursion=true;
				break;
			}
		case EDirectionException::VE_East:
			{
				bMakeEastVisible=true;

				bStartEastRecursion=true;
				break;
			}
		case EDirectionException::VE_West:
			{
				bMakeWestVisible=true;

				bStartWestRecursion=true;
				break;
			}
		default: ;
		}
	}
	else
	{
		switch (PreviousDirectionException)
		{
		case EDirectionException::VE_None:
			{
				break;
			}
		case EDirectionException::VE_North:
			{
				if(NorthTerrainGenerated)
				{
					if(NorthTerrainGenerated->ProceduralTerrain->IsGenerated() && NorthTerrainGenerated->ProceduralTerrain->IsVisible())
					{
						bMakeNorthInvisible=true;
						
						bStartNorthRecursion=true;
					}
				}
				if(EastTerrainGenerated)
				{
					if(EastTerrainGenerated->ProceduralTerrain->IsGenerated() && EastTerrainGenerated->ProceduralTerrain->IsVisible())
					{
						bMakeEastInvisible=true;
						
						bStartEastRecursion=true;
					}
				}
				if(WestTerrainGenerated)
				{
					if(WestTerrainGenerated->ProceduralTerrain->IsGenerated() && WestTerrainGenerated->ProceduralTerrain->IsVisible())
					{
						bMakeWestInvisible=true;
						
						bStartWestRecursion=true;
					}
				}
				break;
			}
		case EDirectionException::VE_South:
			{
				if(SouthTerrainGenerated)
				{
					if(SouthTerrainGenerated->ProceduralTerrain->IsGenerated() && SouthTerrainGenerated->ProceduralTerrain->IsVisible())
					{
						bMakeSouthInvisible=true;
						
						bStartSouthRecursion=true;
					}
				}
				if(EastTerrainGenerated)
				{
					if(EastTerrainGenerated->ProceduralTerrain->IsGenerated() && EastTerrainGenerated->ProceduralTerrain->IsVisible())
					{
						bMakeEastInvisible=true;
						
						bStartEastRecursion=true;
					}
				}
				if(WestTerrainGenerated)
				{
					if(WestTerrainGenerated->ProceduralTerrain->IsGenerated() && WestTerrainGenerated->ProceduralTerrain->IsVisible())
					{
						bMakeWestInvisible=true;
						
						bStartWestRecursion=true;
					}
				}
				break;
			}
		case EDirectionException::VE_East:
			{
				if(EastTerrainGenerated)
				{
					if(EastTerrainGenerated->ProceduralTerrain->IsGenerated() && EastTerrainGenerated->ProceduralTerrain->IsVisible())
					{
						bMakeEastInvisible=true;
						
						bStartEastRecursion=true;
					}
				}
				break;
			}
		case EDirectionException::VE_West:
			{
				if(WestTerrainGenerated)
				{
					if(WestTerrainGenerated->ProceduralTerrain->IsGenerated() && WestTerrainGenerated->ProceduralTerrain->IsVisible())
					{
						bMakeWestInvisible=true;
						
						bStartWestRecursion=true;
					}
				}
				break;
			}
		default: ;
		}
		
	}
	if(bMakeNorthVisible)
	{
		if(NorthTerrainGenerated)
		{
			MakeVisibleComponent(NorthTerrainGenerated,IndexFromActiveLandscape+1);
		}
		else
		{
			NorthTerrainGenerated=GenerateVisibleComponent(EDirectionException::VE_North,IndexFromActiveLandscape+1);
		}
	}
	else
	{
		if(bMakeNorthInvisible)
		{
			NorthTerrainGenerated->ProceduralTerrain->UnLoadMesh();
			NorthTerrainGenerated->bVisibleTerrain=false;
		}
	}
	if(bMakeSouthVisible)
	{
		if(SouthTerrainGenerated)
		{
			MakeVisibleComponent(SouthTerrainGenerated,IndexFromActiveLandscape+1);
		}
		else
		{
			SouthTerrainGenerated=GenerateVisibleComponent(EDirectionException::VE_South,IndexFromActiveLandscape+1);
		}
	}
	else
	{
		if(bMakeSouthInvisible)
		{
			SouthTerrainGenerated->ProceduralTerrain->UnLoadMesh();
			SouthTerrainGenerated->bVisibleTerrain=false;
		}
	}
	if(bMakeEastVisible)
	{
		if(EastTerrainGenerated)
		{
			MakeVisibleComponent(EastTerrainGenerated,IndexFromActiveLandscape+1);
		}
		else
		{
			EastTerrainGenerated=GenerateVisibleComponent(EDirectionException::VE_East,IndexFromActiveLandscape+1);
		}
	}
	else
	{
		if(bMakeEastInvisible)
		{
			EastTerrainGenerated->ProceduralTerrain->UnLoadMesh();
			EastTerrainGenerated->bVisibleTerrain=false;
		}
	}
	if(bMakeWestVisible)
	{
		if(WestTerrainGenerated)
		{
			MakeVisibleComponent(WestTerrainGenerated,IndexFromActiveLandscape+1);
		}
		else
		{
			WestTerrainGenerated=GenerateVisibleComponent(EDirectionException::VE_West,IndexFromActiveLandscape+1);
		}

	}
	else
	{
		if(bMakeWestInvisible)
		{
			WestTerrainGenerated->ProceduralTerrain->UnLoadMesh();
			WestTerrainGenerated->bVisibleTerrain=false;
		}
	}
	
	if(bStartNorthRecursion)
	{
		NorthTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::VE_North,ViewDistance);
	}
	if(bStartEastRecursion)
	{
		EastTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::VE_East,ViewDistance);
	}
	if( bStartSouthRecursion)
	{
		SouthTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::VE_South,ViewDistance);
	}
	if(bStartWestRecursion)
	{
		WestTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::VE_West,ViewDistance);
	}
}



void AProceduralTerrainGen::GenerateSiblingLandscapes(int IndexFromActiveLandscape, EDirectionException PreviousDirectionException)
{
	if(IndexFromActiveLandscape<LandscapeAmount)
	{
		if(PreviousDirectionException == EDirectionException::VE_North)
		{
			NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_North,IndexFromActiveLandscape);
			EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_East,IndexFromActiveLandscape);
			WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_West,IndexFromActiveLandscape);
			
			NorthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_North);
			EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_East);
			WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::VE_West);
		}
		if(PreviousDirectionException == EDirectionException::VE_South)
		{
			SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_South,IndexFromActiveLandscape);
			EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_East,IndexFromActiveLandscape);
			WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_West,IndexFromActiveLandscape);
			
			SouthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_South);
			EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_East);
			WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::VE_West);
		
		}
		if(PreviousDirectionException == EDirectionException::VE_East)
		{
			EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_East,IndexFromActiveLandscape);

			EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_East);
		}
		if(PreviousDirectionException== EDirectionException::VE_West)
		{
			WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_West,IndexFromActiveLandscape);

			WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::VE_West);
		}
		if(PreviousDirectionException==EDirectionException::VE_None)
		{
			NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_North,IndexFromActiveLandscape);
			SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_South,IndexFromActiveLandscape);
			EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_East,IndexFromActiveLandscape);
			WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::VE_West,IndexFromActiveLandscape);
			
			NorthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_North);
			SouthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_South);
			EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_East);
			WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::VE_West);
		}
	}
}

void AProceduralTerrainGen::GenerateCurrentLandscape()
{
	/**Set Noise Resolution*/
	ProceduralTerrain->SetNoiseResolution(NoiseResolution);
	/**Set Noise Input Scale*/
	ProceduralTerrain->SetNoiseInputScale(NoiseInputScale);
	/**Set noise Output Scale*/
	ProceduralTerrain->SetNoiseOutputScale(NoiseOutputScale);
	/**Set Total Size to Generate*/
	ProceduralTerrain->SetTotalSizeToGenerate(TotalSizeToGenerate);
	/** Set Noise Type*/
	ProceduralTerrain->SetNoiseTypeComponent(NoiseType);
	/** Set seed. */
	ProceduralTerrain->SetSeed(Seed);
	/** Set frequency. */
	ProceduralTerrain->SetFrequency(Frequency);
	/** Set interpolation type. */
	ProceduralTerrain->SetInterpolation(Interp);
	/** Set fractal type. */
	ProceduralTerrain->SetFractalType(FractalType);
	/** Set fractal octaves. */
	ProceduralTerrain->SetOctaves(Octaves);
	/** Set fractal lacunarity. */
	ProceduralTerrain->SetLacunarity(Lacunarity);
	/** Set fractal gain. */
	ProceduralTerrain->SetGain(Gain);
	/** Set cellular jitter. */
	ProceduralTerrain->SetCellularJitter(CellularJitter);
	/** Set cellular distance function. */
	ProceduralTerrain->SetDistanceFunction(CellularDistanceFunction);
	/** Set cellular return type. */
	ProceduralTerrain->SetReturnType(CellularReturnType);
	ProceduralTerrain->GenerateMap(NoiseComponentStartLocation);
}

// Called every frame
void AProceduralTerrainGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AProceduralTerrainGen::RecursiveDelete()
{
	if(EastTerrainGenerated)
	{
		if(!EastTerrainGenerated->bNeedDestroyed)
		{
			EastTerrainGenerated->bNeedDestroyed=true;
			EastTerrainGenerated->RecursiveDelete();
		}
	}
	if(WestTerrainGenerated)
	{
		if(!WestTerrainGenerated->bNeedDestroyed)
		{
			WestTerrainGenerated->bNeedDestroyed=true;
			WestTerrainGenerated->RecursiveDelete();
		}
	}
	if(NorthTerrainGenerated)
	{
		if(!NorthTerrainGenerated->bNeedDestroyed)
		{
			NorthTerrainGenerated->bNeedDestroyed=true;
			NorthTerrainGenerated->RecursiveDelete();
		}
	}
	if(SouthTerrainGenerated)
	{
		if(!SouthTerrainGenerated->bNeedDestroyed)
		{
			SouthTerrainGenerated->bNeedDestroyed=true;
			SouthTerrainGenerated->RecursiveDelete();
		}
	}
	if(!bNeverDestroy)
	{
		if(bNeedDestroyed)
		{
			Destroy();
		}
	}
	else
	{
		NorthTerrainGenerated= nullptr;
		SouthTerrainGenerated= nullptr;
		WestTerrainGenerated= nullptr;
		EastTerrainGenerated = nullptr;
	}
}

void AProceduralTerrainGen::SetChunkViewDistance(int ChunkDistance)
{
	ChunkViewDistance=ChunkDistance;
}

void AProceduralTerrainGen::SetNoiseSamplingPerLine(int NoiseSampling)
{
	NoiseSamplesPerLine=NoiseSampling;
}

void AProceduralTerrainGen::MakeVisibleComponent(AProceduralTerrainGen* TerrainGen,int IndexFromActiveLandscape)
{
	if(TerrainGen->ProceduralTerrain->IsGenerated())
	{
		if(!TerrainGen->ProceduralTerrain->IsVisible())
		{
			TerrainGen->ProceduralTerrain->LoadMesh();
		}
	}
	else
	{
		TerrainGen->GenerateCurrentLandscape();
	}
	TerrainGen->bVisibleTerrain=true;
	TerrainGen->CurrentIndexFromActiveLandscape=IndexFromActiveLandscape;
}

AProceduralTerrainGen* AProceduralTerrainGen::GenerateVisibleComponent(EDirectionException PreviousDirectionException,
	int IndexFromActiveLandscape)
{
	AProceduralTerrainGen* TerrainGen;
	switch (PreviousDirectionException)
	{
	case EDirectionException::VE_North:
		{
			TerrainGen =CreateProceduralTerrain(GetActorLocation().X + TotalSizeToGenerate-NoiseResolution, GetActorLocation().Y,IndexFromActiveLandscape, PreviousDirectionException);
			break;
		}
	case EDirectionException::VE_South:
		{
			TerrainGen =CreateProceduralTerrain(GetActorLocation().X - TotalSizeToGenerate+NoiseResolution, GetActorLocation().Y,IndexFromActiveLandscape,PreviousDirectionException);
			break;
		}
	case EDirectionException::VE_East:
		{
			TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y+ TotalSizeToGenerate-NoiseResolution,IndexFromActiveLandscape,PreviousDirectionException);
			break;
		}
	case EDirectionException::VE_West:
		{
			TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y- TotalSizeToGenerate+NoiseResolution,IndexFromActiveLandscape,PreviousDirectionException);
			break;
		}
	default:
		{
			TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,IndexFromActiveLandscape,PreviousDirectionException);
		}
	}
	if(TerrainGen)
	{
		TerrainGen->bVisibleTerrain=true;
		TerrainGen->CurrentIndexFromActiveLandscape=IndexFromActiveLandscape;
	}
	return TerrainGen;
}

AProceduralTerrainGen* AProceduralTerrainGen::CreateProceduralTerrain(int PosX, int PosY, int CurrentLandscapeIndex, EDirectionException PreviousDirectionException)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	AProceduralTerrainGen* ProceduralTerrainGen = GetWorld()->SpawnActor<AProceduralTerrainGen>(
		FVector(PosX,PosY, GetActorLocation().Z),GetActorRotation(),SpawnParams);
	
	ProceduralTerrainGen->SetLandscapeAmount(LandscapeAmount);
	if(ProceduralTerrainGen->GetCurrentIndexFromActiveLandscape()!= 0 && ProceduralTerrainGen->GetCurrentIndexFromActiveLandscape()>CurrentLandscapeIndex)
	{
		ProceduralTerrainGen->SetCurrentIndexFromActiveLandscape(CurrentLandscapeIndex);
	}
	ProceduralTerrainGen->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
	InitializeVariable(ProceduralTerrainGen);
	switch (PreviousDirectionException)
	{
	case EDirectionException::VE_North:
		{
			ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(NoiseSamplesPerLine-1,0,0);
			break;
		}
	case EDirectionException::VE_South:
		{
			ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(-(NoiseSamplesPerLine-1),0,0);
			break;
		}
	case EDirectionException::VE_East:
		{
			ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(0,NoiseSamplesPerLine-1,0);
			break;
		}
	case EDirectionException::VE_West:
		{
			ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(0,-(NoiseSamplesPerLine-1),0);
			break;
		}
	default:
		{
			ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation;
		}
	}
	ProceduralTerrainGen->GenerateCurrentLandscape();

	return ProceduralTerrainGen;
}

void AProceduralTerrainGen::SetCurrentIndexFromActiveLandscape(int Index)
{
	CurrentIndexFromActiveLandscape=Index;
}

int AProceduralTerrainGen::GetCurrentIndexFromActiveLandscape()
{
	return CurrentIndexFromActiveLandscape;
}

void AProceduralTerrainGen::SetLandscapeAmount(int LandscapeViewAmount)
{
	LandscapeAmount=LandscapeViewAmount;
}

void AProceduralTerrainGen::SetNoiseTypeComponent(const EFastNoise_NoiseType NewNoiseType)
{
	NoiseType=NewNoiseType;
}

void AProceduralTerrainGen::SetSeed(const int32 NewSeed)
{
	Seed=NewSeed;
}

void AProceduralTerrainGen::SetFrequency(const float NewFrequency)
{
	Frequency=NewFrequency;
}

void AProceduralTerrainGen::SetInterpolation(const EFastNoise_Interp NewInterpolation)
{
	Interp=NewInterpolation;
}

void AProceduralTerrainGen::SetFractalType(const EFastNoise_FractalType NewFractalType)
{
	FractalType=NewFractalType;
}

void AProceduralTerrainGen::SetOctaves(const int32 NewOctaves)
{
	Octaves=NewOctaves;
}

void AProceduralTerrainGen::SetLacunarity(const float NewLacunarity)
{
	Lacunarity=NewLacunarity;
}

void AProceduralTerrainGen::SetGain(const float NewGain)
{
	Gain=NewGain;
}

void AProceduralTerrainGen::SetCellularJitter(const float NewCellularJitter)
{
	CellularJitter=NewCellularJitter;
}

void AProceduralTerrainGen::SetDistanceFunction(const EFastNoise_CellularDistanceFunction NewDistanceFunction)
{
	CellularDistanceFunction=NewDistanceFunction;
}

void AProceduralTerrainGen::SetReturnType(const EFastNoise_CellularReturnType NewCellularReturnType)
{
	CellularReturnType=NewCellularReturnType;
}

void AProceduralTerrainGen::SetNoiseResolution(int NewNoiseResolution)
{
	NoiseResolution=NewNoiseResolution;
}

void AProceduralTerrainGen::SetTotalSizeToGenerate(int NewTotalSizeToGenerate)
{
	TotalSizeToGenerate=NewTotalSizeToGenerate;
}

void AProceduralTerrainGen::SetNoiseInputScale(float NewNoiseInputScale)
{
	NoiseInputScale=NewNoiseInputScale;
}

void AProceduralTerrainGen::SetNoiseOutputScale(float NewNoiseOutputScale)
{
	NoiseOutputScale=NewNoiseOutputScale;
}

void AProceduralTerrainGen::InitializeVariable(AProceduralTerrainGen* Proc)
{
	/**Set Noise Resolution*/
	Proc->SetNoiseResolution(NoiseResolution);
	/**Set Noise Input Scale*/
	Proc->SetNoiseInputScale(NoiseInputScale);
	/**Set noise Output Scale*/
	Proc->SetNoiseOutputScale(NoiseOutputScale);
	/**Set Total Size to Generate*/
	Proc->SetTotalSizeToGenerate(TotalSizeToGenerate);
	/** Set Noise Type*/
	Proc->SetNoiseTypeComponent(NoiseType);
	/** Set seed. */
	Proc->SetSeed(Seed);
	/** Set frequency. */
	Proc->SetFrequency(Frequency);
	/** Set interpolation type. */
	Proc->SetInterpolation(Interp);
	/** Set fractal type. */
	Proc->SetFractalType(FractalType);
	/** Set fractal octaves. */
	Proc->SetOctaves(Octaves);
	/** Set fractal lacunarity. */
	Proc->SetLacunarity(Lacunarity);
	/** Set fractal gain. */
	Proc->SetGain(Gain);
	/** Set cellular jitter. */
	Proc->SetCellularJitter(CellularJitter);
	/** Set cellular distance function. */
	Proc->SetDistanceFunction(CellularDistanceFunction);
	/** Set cellular return type. */
	Proc->SetReturnType(CellularReturnType);
	/**Set chunk view distance*/
	Proc->SetChunkViewDistance(ChunkViewDistance);
	/**Set the Noise Sampling Per Line*/
	Proc->SetNoiseSamplingPerLine(NoiseSamplesPerLine);
	
}
