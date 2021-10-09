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
	CurrentIndexFromActiveLandscape=0;
	GenerateCurrentLandscape();
	NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
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
	RecursiveActiveCheck(EDirectionException::VE_None);
}

void AProceduralTerrainGen::RecursiveActiveCheck(EDirectionException DirectionException)
{
	if(DirectionException == EDirectionException::VE_North)
	{
		if(NorthTerrainGenerated)
		{
			if(NorthTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(NorthTerrainGenerated);
				NorthTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_North);
			}
			else
			{
				if(NorthTerrainGenerated->ProceduralTerrain->IsGenerated() && NorthTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					NorthTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					NorthTerrainGenerated->bVisibleTerrain=false;
					NorthTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_North);
				}
			}
		}
		if(EastTerrainGenerated)
		{
			if(EastTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(EastTerrainGenerated);
				EastTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_East);
			}
			else
			{
				if(EastTerrainGenerated->ProceduralTerrain->IsGenerated() && EastTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					EastTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					EastTerrainGenerated->bVisibleTerrain=false;
					EastTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_East);
				}
			}
		}
		if(WestTerrainGenerated)
		{
			if(WestTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(WestTerrainGenerated);
				WestTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_West);
			}
			else
			{
				if(WestTerrainGenerated->ProceduralTerrain->IsGenerated() && WestTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					WestTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					WestTerrainGenerated->bVisibleTerrain=false;
					WestTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_West);
				}
			}
		}
	}
	else if(DirectionException == EDirectionException::VE_South)
	{
		if(SouthTerrainGenerated)
		{
			if(SouthTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(SouthTerrainGenerated);
				SouthTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_South);
			}
			else
			{
				if(SouthTerrainGenerated->ProceduralTerrain->IsGenerated() && SouthTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					SouthTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					SouthTerrainGenerated->bVisibleTerrain=false;
					SouthTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_South);
				}
			}
		}
		if(EastTerrainGenerated)
		{
			if(EastTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(EastTerrainGenerated);
				EastTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_East);
			}
			else
			{
				if(EastTerrainGenerated->ProceduralTerrain->IsGenerated() && EastTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					EastTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					EastTerrainGenerated->bVisibleTerrain=false;
					EastTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_East);
				}
			}
		}
		if(WestTerrainGenerated)
		{
			if(WestTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(WestTerrainGenerated);
				WestTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_West);
			}
			else
			{
				if(WestTerrainGenerated->ProceduralTerrain->IsGenerated() && WestTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					WestTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					WestTerrainGenerated->bVisibleTerrain=false;
					WestTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_West);
				}
			}
		}
	}
	else if(DirectionException == EDirectionException::VE_East)
	{
		if(EastTerrainGenerated)
		{
			if(EastTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(EastTerrainGenerated);
				EastTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_East);
			}
			else
			{
				if(EastTerrainGenerated->ProceduralTerrain->IsGenerated() && EastTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					EastTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					EastTerrainGenerated->bVisibleTerrain=false;
					EastTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_East);
				}
			}
		}
	}
	else if(DirectionException== EDirectionException::VE_West)
	{
		if(WestTerrainGenerated)
		{
			if(WestTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(WestTerrainGenerated);
				WestTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_West);
			}
			else
			{
				if(WestTerrainGenerated->ProceduralTerrain->IsGenerated() && WestTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					WestTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					WestTerrainGenerated->bVisibleTerrain=false;
					WestTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_West);
				}
			}
		}
	}
	else if(DirectionException==EDirectionException::VE_None)
	{
		if(NorthTerrainGenerated)
		{
			if(NorthTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(NorthTerrainGenerated);
				NorthTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_North);
			}
			else
			{
				if(NorthTerrainGenerated->ProceduralTerrain->IsGenerated() && NorthTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					NorthTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					NorthTerrainGenerated->bVisibleTerrain=false;
					NorthTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_North);
				}
			}
		}
		if(SouthTerrainGenerated)
		{
			if(SouthTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(SouthTerrainGenerated);
				SouthTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_South);
			}
			else
			{
				if(SouthTerrainGenerated->ProceduralTerrain->IsGenerated() && SouthTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					SouthTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					SouthTerrainGenerated->bVisibleTerrain=false;
					SouthTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_South);
				}
			}
		}
		if(EastTerrainGenerated)
		{
			if(EastTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(EastTerrainGenerated);
				EastTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_East);
			}
			else
			{
				if(EastTerrainGenerated->ProceduralTerrain->IsGenerated() && EastTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					EastTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					EastTerrainGenerated->bVisibleTerrain=false;
					EastTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_East);
				}
			}
		}
		if(WestTerrainGenerated)
		{
			if(WestTerrainGenerated->GetCurrentIndexFromActiveLandscape()<ChunkViewDistance)
			{
				MakeVisibleOrGenerateComponent(WestTerrainGenerated);
				WestTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_West);
			}
			else
			{
				if(WestTerrainGenerated->ProceduralTerrain->IsGenerated() && WestTerrainGenerated->ProceduralTerrain->IsVisible())
				{
					WestTerrainGenerated->ProceduralTerrain->UnLoadMesh();
					WestTerrainGenerated->bVisibleTerrain=false;
					WestTerrainGenerated->RecursiveActiveCheck(EDirectionException::VE_West);
				}
			}
		}
	}
}



void AProceduralTerrainGen::GenerateSiblingLandscapes(int IndexFromActiveLandscape, EDirectionException PreviousDirectionException)
{
	if(IndexFromActiveLandscape<LandscapeAmount)
	{
		if(PreviousDirectionException == EDirectionException::VE_North)
		{
			NorthTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X + TotalSizeToGenerate-NoiseResolution, GetActorLocation().Y,IndexFromActiveLandscape);
			EastTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y+ TotalSizeToGenerate-NoiseResolution,IndexFromActiveLandscape);
			WestTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y- TotalSizeToGenerate+NoiseResolution,IndexFromActiveLandscape);

			NorthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_North);
			EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_East);
			WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::VE_West);
		}
		if(PreviousDirectionException == EDirectionException::VE_South)
		{
			SouthTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X - TotalSizeToGenerate+NoiseResolution, GetActorLocation().Y,IndexFromActiveLandscape);
			EastTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y+ TotalSizeToGenerate-NoiseResolution,IndexFromActiveLandscape);
			WestTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y- TotalSizeToGenerate+NoiseResolution,IndexFromActiveLandscape);

			SouthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_South);
			EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_East);
			WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::VE_West);
		
		}
		if(PreviousDirectionException == EDirectionException::VE_East)
		{
			EastTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y+ TotalSizeToGenerate-NoiseResolution,IndexFromActiveLandscape);

			EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::VE_East);
		}
		if(PreviousDirectionException== EDirectionException::VE_West)
		{
			WestTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y- TotalSizeToGenerate+NoiseResolution,IndexFromActiveLandscape);

			WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::VE_West);
		}
		if(PreviousDirectionException==EDirectionException::VE_None)
		{
			NorthTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X + TotalSizeToGenerate-NoiseResolution, GetActorLocation().Y,IndexFromActiveLandscape);
			SouthTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X - TotalSizeToGenerate+NoiseResolution, GetActorLocation().Y,IndexFromActiveLandscape);
			EastTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y+ TotalSizeToGenerate-NoiseResolution,IndexFromActiveLandscape);
			WestTerrainGenerated =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y- TotalSizeToGenerate+NoiseResolution,IndexFromActiveLandscape);

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
	ProceduralTerrain->GenerateMap();
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

void AProceduralTerrainGen::MakeVisibleOrGenerateComponent(AProceduralTerrainGen* TerrainGen)
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
}

AProceduralTerrainGen* AProceduralTerrainGen::CreateProceduralTerrain(int PosX, int PosY, int CurrentLandscapeIndex)
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
	
}
