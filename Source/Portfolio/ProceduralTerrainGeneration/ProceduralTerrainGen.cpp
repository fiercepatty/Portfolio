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
	GenerateSiblingLandscapes(1, EDirectionException::Ve_None);
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
	RecursiveActiveCheck(1,EDirectionException::Ve_None,ChunkViewDistance);
}

void AProceduralTerrainGen::RecursiveActiveCheck(const int IndexFromActiveLandscape, const EDirectionException PreviousDirectionException, int ViewDistance)
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
		case EDirectionException::Ve_None:
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
		case EDirectionException::Ve_North:
			{
				bMakeNorthVisible=true;
				bMakeEastVisible=true;
				bMakeWestVisible=true;

				bStartNorthRecursion=true;
				bStartEastRecursion=true;
				bStartWestRecursion=true;
				break;
			}
		case EDirectionException::Ve_South:
			{
				bMakeEastVisible=true;
				bMakeSouthVisible=true;
				bMakeWestVisible=true;

				bStartSouthRecursion=true;
				bStartEastRecursion=true;
				bStartWestRecursion=true;
				break;
			}
		case EDirectionException::Ve_East:
			{
				bMakeEastVisible=true;

				bStartEastRecursion=true;
				break;
			}
		case EDirectionException::Ve_West:
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
		case EDirectionException::Ve_None:
			{
				break;
			}
		case EDirectionException::Ve_North:
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
		case EDirectionException::Ve_South:
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
		case EDirectionException::Ve_East:
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
		case EDirectionException::Ve_West:
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
			NorthTerrainGenerated=GenerateVisibleComponent(EDirectionException::Ve_North,IndexFromActiveLandscape+1);
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
			SouthTerrainGenerated=GenerateVisibleComponent(EDirectionException::Ve_South,IndexFromActiveLandscape+1);
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
			EastTerrainGenerated=GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1);
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
			WestTerrainGenerated=GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1);
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
		NorthTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::Ve_North,ViewDistance);
	}
	if(bStartEastRecursion)
	{
		EastTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::Ve_East,ViewDistance);
	}
	if( bStartSouthRecursion)
	{
		SouthTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::Ve_South,ViewDistance);
	}
	if(bStartWestRecursion)
	{
		WestTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::Ve_West,ViewDistance);
	}
}

void AProceduralTerrainGen::GenerateSiblingLandscapes(const int IndexFromActiveLandscape, const EDirectionException PreviousDirectionException)
{
	switch (TerrainShape)
	{
	case EComponentShapes::Ve_Plain:
		{
			if(IndexFromActiveLandscape<LandscapeAmount)
			{
				if(PreviousDirectionException == EDirectionException::Ve_North)
				{
					NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,IndexFromActiveLandscape);
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape);
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape);
			
					NorthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_North);
					EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_East);
					WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::Ve_West);
				}
				if(PreviousDirectionException == EDirectionException::Ve_South)
				{
					SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,IndexFromActiveLandscape);
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape);
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape);
			
					SouthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_South);
					EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_East);
					WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::Ve_West);
		
				}
				if(PreviousDirectionException == EDirectionException::Ve_East)
				{
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape);

					EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_East);
				}
				if(PreviousDirectionException== EDirectionException::Ve_West)
				{
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape);

					WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::Ve_West);
				}
				if(PreviousDirectionException==EDirectionException::Ve_None)
				{
					NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,IndexFromActiveLandscape);
					SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,IndexFromActiveLandscape);
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape);
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape);
			
					NorthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_North);
					SouthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_South);
					EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_East);
					WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::Ve_West);
				}
			}
			break;
		}
	case EComponentShapes::Ve_Cube:
		{
			if(IndexFromActiveLandscape<ConvertEnumSubSectionsToInteger(TerrainSubSections))
			{
				if(PreviousDirectionException == EDirectionException::Ve_North)
				{
					if(TerrainShapeSide==EShapeSide::Ve_Top)
					{
						EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right);

						EastTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_East);
					}
					NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,IndexFromActiveLandscape+1,TerrainShapeSide);
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainShapeSide);
			
					NorthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_North);
					WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
				}
				if(PreviousDirectionException == EDirectionException::Ve_South)
				{
					SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,IndexFromActiveLandscape+1,TerrainShapeSide);
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainShapeSide);
			
					SouthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_South);
					WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
		
				}
				if(PreviousDirectionException == EDirectionException::Ve_East)
				{
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainShapeSide);

					EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_East);
				}
				if(PreviousDirectionException== EDirectionException::Ve_West)
				{
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainShapeSide);

					WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::Ve_West);
				}
				if(PreviousDirectionException==EDirectionException::Ve_None)
				{
					NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,2,EShapeSide::Ve_Top);
					SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Front);
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right);
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Top);
			
					NorthTerrainGenerated->GenerateSiblingLandscapes(2, EDirectionException::Ve_North);
					SouthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_South);
					EastTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_East);
					WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
				
				}
			}
			else
			{
				if(PreviousDirectionException == EDirectionException::Ve_North)
				{
					if(TerrainShapeSide == EShapeSide::Ve_Top)
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,1,EShapeSide::Ve_Back);
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Top);
						EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right);

						EastTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_East);
						NorthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_North);
						WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
					}
					else if(TerrainSubSections !=ESubSections::Ve_One)
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Back);

						WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
					}
				}
				if(PreviousDirectionException == EDirectionException::Ve_South)
				{
					if(TerrainShapeSide==EShapeSide::Ve_Front)
					{
						if(TerrainSubSections !=ESubSections::Ve_One)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Front);

							WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
						}
						SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Bottom);
			
						SouthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_South);
					}
					else if(TerrainSubSections !=ESubSections::Ve_One)
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Bottom);

						WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
					}
				}
				if(PreviousDirectionException == EDirectionException::Ve_West)
				{
					if(TerrainShapeSide==EShapeSide::Ve_Top)
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,1,EShapeSide::Ve_Left);

						WestTerrainGenerated->GenerateSiblingLandscapes(1,EDirectionException::Ve_West);
					}
				}
				if(PreviousDirectionException==EDirectionException::Ve_None)
				{
					NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,1,EShapeSide::Ve_Back);
					SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Front);
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right);
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,1,EShapeSide::Ve_Left);
			
					NorthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_North);
					SouthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_South);
					EastTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_East);
					WestTerrainGenerated->GenerateSiblingLandscapes(1,EDirectionException::Ve_West);
			
					}
			}
			break;
		}
	case EComponentShapes::Ve_Sphere:
		{
			break;
		}
	default: ;
	}
	
}

void AProceduralTerrainGen::GenerateCurrentLandscape() const
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
	switch (TerrainShape)
	{
	case EComponentShapes::Ve_Plain:
		{
			ProceduralTerrain->GenerateMap(NoiseComponentStartLocation);
			break;
		}
	case EComponentShapes::Ve_Cube:
		{
			ProceduralTerrain->GenerateMap(NoiseComponentStartLocation,TerrainShape,TerrainShapeSide,TerrainSubSections);
			break;
		}
	case EComponentShapes::Ve_Sphere:
		{
			break;
		}
	default: ;
	}
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

void AProceduralTerrainGen::MakeVisibleComponent(AProceduralTerrainGen* TerrainGen, const int IndexFromActiveLandscape)
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

AProceduralTerrainGen* AProceduralTerrainGen::GenerateVisibleComponent(const EDirectionException PreviousDirectionException,const int IndexFromActiveLandscape, const EShapeSide TerrainSide)
{
	AProceduralTerrainGen* TerrainGen;
	
	switch (PreviousDirectionException)
	{
	case EDirectionException::Ve_North:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X + TotalSizeToGenerate-NoiseResolution, GetActorLocation().Y, GetActorLocation().Z,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide);
			}
			else
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,GetActorLocation().Z - TotalSizeToGenerate+NoiseResolution,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide);
			}
			if(IndexFromActiveLandscape==1)
			{
				TerrainGen->ProceduralTerrain->SetEdgeOfGeometricObject(EEdgeOfGeometricObject::Ve_NorthStart);
			}
			else if(IndexFromActiveLandscape==ConvertEnumSubSectionsToInteger(TerrainSubSections))
			{
				TerrainGen->ProceduralTerrain->SetEdgeOfGeometricObject(EEdgeOfGeometricObject::Ve_NorthEnd);
			}
			else
			{
				TerrainGen->ProceduralTerrain->SetEdgeOfGeometricObject(EEdgeOfGeometricObject::Ve_NorthMiddle);
			}
			break;
		}
	case EDirectionException::Ve_South:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X - TotalSizeToGenerate+NoiseResolution, GetActorLocation().Y,GetActorLocation().Z,IndexFromActiveLandscape,PreviousDirectionException,TerrainSide);
			}
			else if(TerrainSide==EShapeSide::Ve_Front)
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,GetActorLocation().Z - TotalSizeToGenerate+NoiseResolution,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide);
			}
			else
			{
				if(IndexFromActiveLandscape==1)
				{
					TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide);
				}
				else
				{
					TerrainGen =CreateProceduralTerrain(GetActorLocation().X + TotalSizeToGenerate-NoiseResolution, GetActorLocation().Y, GetActorLocation().Z,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide);
				}
			}
			if(IndexFromActiveLandscape==1)
			{
				TerrainGen->ProceduralTerrain->SetEdgeOfGeometricObject(EEdgeOfGeometricObject::Ve_SouthStart);
			}
			else if(IndexFromActiveLandscape==ConvertEnumSubSectionsToInteger(TerrainSubSections))
			{
				TerrainGen->ProceduralTerrain->SetEdgeOfGeometricObject(EEdgeOfGeometricObject::Ve_SouthEnd);
			}
			else
			{
				TerrainGen->ProceduralTerrain->SetEdgeOfGeometricObject(EEdgeOfGeometricObject::Ve_SouthMiddle);
			}
			break;
		}
	case EDirectionException::Ve_East:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y+ TotalSizeToGenerate-NoiseResolution,GetActorLocation().Z,IndexFromActiveLandscape,PreviousDirectionException,TerrainSide);
			}
			else
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,GetActorLocation().Z - TotalSizeToGenerate+NoiseResolution,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide);
				
			}
			break;
		}
	case EDirectionException::Ve_West:
		{
			if(TerrainSide!=EShapeSide::Ve_Left)
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y- TotalSizeToGenerate+NoiseResolution,GetActorLocation().Z,IndexFromActiveLandscape,PreviousDirectionException,TerrainSide);
			}
			else
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,GetActorLocation().Z - TotalSizeToGenerate+NoiseResolution,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide);
			}
			break;
		}
	default:
		{
			TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,GetActorLocation().Z,IndexFromActiveLandscape,PreviousDirectionException,TerrainSide);
		}
	}
	if(TerrainGen)
	{
		TerrainGen->bVisibleTerrain=true;
		TerrainGen->CurrentIndexFromActiveLandscape=IndexFromActiveLandscape;
	}
	return TerrainGen;
}

AProceduralTerrainGen* AProceduralTerrainGen::CreateProceduralTerrain(const int PosX, const int PosY, const int PosZ, const int CurrentLandscapeIndex, const EDirectionException PreviousDirectionException, const EShapeSide TerrainSide)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	AProceduralTerrainGen* ProceduralTerrainGen = GetWorld()->SpawnActor<AProceduralTerrainGen>(
		FVector(PosX,PosY,PosZ),GetActorRotation(),SpawnParams);
	
	ProceduralTerrainGen->SetLandscapeAmount(LandscapeAmount);
	if(ProceduralTerrainGen->GetCurrentIndexFromActiveLandscape()!= 0 && ProceduralTerrainGen->GetCurrentIndexFromActiveLandscape()>CurrentLandscapeIndex)
	{
		ProceduralTerrainGen->SetCurrentIndexFromActiveLandscape(CurrentLandscapeIndex);
	}
	ProceduralTerrainGen->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
	InitializeVariable(ProceduralTerrainGen);
	ProceduralTerrainGen->SetTerrainShapeSide(TerrainSide);
	switch (PreviousDirectionException)
	{
	case EDirectionException::Ve_North:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(NoiseSamplesPerLine-1,0,0);
			}
			else
			{
				ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(0,0,-(NoiseSamplesPerLine-1));
			}
			break;
		}
	case EDirectionException::Ve_South:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(-(NoiseSamplesPerLine-1),0,0);
			}
			else if(TerrainSide==EShapeSide::Ve_Front)
			{
				ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(0,0,-(NoiseSamplesPerLine-1));
			}
			else
			{
				ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+  FVector(NoiseSamplesPerLine-1,0,0);
			}
			break;
		}
	case EDirectionException::Ve_East:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(0,NoiseSamplesPerLine-1,0);
			}
			else
			{
				ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(0,0,-(NoiseSamplesPerLine-1));
			}
			break;
		}
	case EDirectionException::Ve_West:
		{
			if(TerrainSide!=EShapeSide::Ve_Left)
			{
				ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(0,-(NoiseSamplesPerLine-1),0);
			}
			else
			{
				ProceduralTerrainGen->NoiseComponentStartLocation = NoiseComponentStartLocation+ FVector(0,0,-(NoiseSamplesPerLine-1));
			}
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

void AProceduralTerrainGen::SetChunkViewDistance(const int ChunkDistance)
{
	ChunkViewDistance=ChunkDistance;
}

void AProceduralTerrainGen::SetNoiseSamplingPerLine(const int NoiseSampling)
{
	NoiseSamplesPerLine=NoiseSampling;
}

void AProceduralTerrainGen::SetCurrentIndexFromActiveLandscape(const int Index)
{
	CurrentIndexFromActiveLandscape=Index;
}

int AProceduralTerrainGen::GetCurrentIndexFromActiveLandscape() const
{
	return CurrentIndexFromActiveLandscape;
}

void AProceduralTerrainGen::SetLandscapeAmount(const int LandscapeViewAmount)
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

void AProceduralTerrainGen::SetNoiseResolution(const int NewNoiseResolution)
{
	NoiseResolution=NewNoiseResolution;
}

void AProceduralTerrainGen::SetTotalSizeToGenerate(const int NewTotalSizeToGenerate)
{
	TotalSizeToGenerate=NewTotalSizeToGenerate;
}

void AProceduralTerrainGen::SetNoiseInputScale(const float NewNoiseInputScale)
{
	NoiseInputScale=NewNoiseInputScale;
}

void AProceduralTerrainGen::SetNoiseOutputScale(const float NewNoiseOutputScale)
{
	NoiseOutputScale=NewNoiseOutputScale;
}

void AProceduralTerrainGen::InitializeVariable(AProceduralTerrainGen* Proc) const
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
	/**Set the Shape Side*/
	Proc->SetTerrainShape(TerrainShape);
	/**Set the Component shape*/
	Proc->SetTerrainSubSections(TerrainSubSections);
	
}

int AProceduralTerrainGen::ConvertEnumSubSectionsToInteger(const ESubSections AShapeSubSections)
{
	switch (AShapeSubSections)
	{
	case ESubSections::Ve_One: return 1;
	case ESubSections::Ve_Two: return 2;
	case ESubSections::Ve_Four: return 4;
	case ESubSections::Ve_Eight: return 8;
	case ESubSections::Ve_Sixteen: return 16;
	case ESubSections::Ve_ThirtyTwo: return 32;
	case ESubSections::Ve_SixtyFour: return 64;
	default: return 1;
	}
}