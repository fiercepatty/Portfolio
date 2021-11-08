// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGen.h"

#include <stdbool.h>

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
	if(ProceduralTerrain)
	{
		RootComponent= Cast<USceneComponent>(ProceduralTerrain);
	}
	if(!NoiseParameters)
	{
		NoiseParameters = CreateDefaultSubobject<UNoiseParameters>(TEXT("NoiseParameters"));
	}
	if(!TerrainParameters)
	{
		TerrainParameters= CreateDefaultSubobject<UTerrainParameters>(TEXT("TerrainParameters"));
	}
}

void AProceduralTerrainGen::GenerateTerrain()
{
	bNeverDestroy=true;
	RecursiveDelete();
	NoiseParameters->NoiseComponentStartLocation = FVector(0,0,0);
	NoiseParameters->NoiseSamplesPerLine = NoiseParameters->TotalSizeToGenerate / NoiseParameters->NoiseResolution;
	TerrainParameters->CurrentIndexFromActiveLandscape=0;
	if(TerrainParameters->TerrainSubSections==ESubSections::Ve_One)
	{
		SetNewClamps(true,true,true,true);
	}
	else
	{
		SetNewClamps(false,true,true,false);
	}
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
	TerrainParameters->bActiveTerrain=true;
	TerrainParameters->bVisibleTerrain=true;
	RecursiveActiveCheck(1,EDirectionException::Ve_None,TerrainParameters->ChunkViewDistance);
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
				bMakeNorthVisible=true;bMakeEastVisible=true;bMakeSouthVisible=true;bMakeWestVisible=true;

				bStartNorthRecursion=true;bStartSouthRecursion=true;bStartEastRecursion=true;bStartWestRecursion=true;
				break;
			}
		case EDirectionException::Ve_North:
			{
				bMakeNorthVisible=true;bMakeEastVisible=true;bMakeWestVisible=true;

				bStartNorthRecursion=true;bStartEastRecursion=true;bStartWestRecursion=true;
				break;
			}
		case EDirectionException::Ve_South:
			{
				bMakeEastVisible=true;bMakeSouthVisible=true;bMakeWestVisible=true;

				bStartSouthRecursion=true;bStartEastRecursion=true;bStartWestRecursion=true;
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
				bMakeNorthInvisible=bStartNorthRecursion=CardinalDirectionGeneratedVisible(NorthTerrainGenerated);
				bMakeEastInvisible=bStartEastRecursion=CardinalDirectionGeneratedVisible(EastTerrainGenerated);
				bMakeWestInvisible=bStartWestRecursion=CardinalDirectionGeneratedVisible(WestTerrainGenerated);
				break;
			}
		case EDirectionException::Ve_South:
			{
				bMakeSouthInvisible=bStartSouthRecursion=CardinalDirectionGeneratedVisible(SouthTerrainGenerated);
				bMakeEastInvisible=bStartEastRecursion=CardinalDirectionGeneratedVisible(EastTerrainGenerated);
				bMakeWestInvisible=bStartWestRecursion=CardinalDirectionGeneratedVisible(WestTerrainGenerated);
				break;
			}
		case EDirectionException::Ve_East:
			{
				bMakeEastInvisible=bStartEastRecursion=CardinalDirectionGeneratedVisible(EastTerrainGenerated);
				break;
			}
		case EDirectionException::Ve_West:
			{
				bMakeWestInvisible=bStartWestRecursion=CardinalDirectionGeneratedVisible(WestTerrainGenerated);
				break;
			}
		default: ;
		}
		
	}
	SetCardinalDirectionVisibility(NorthTerrainGenerated,bMakeNorthVisible,bMakeNorthInvisible,IndexFromActiveLandscape);
	SetCardinalDirectionVisibility(SouthTerrainGenerated,bMakeSouthVisible,bMakeSouthInvisible,IndexFromActiveLandscape);
	SetCardinalDirectionVisibility(EastTerrainGenerated,bMakeEastVisible,bMakeEastInvisible,IndexFromActiveLandscape);
	SetCardinalDirectionVisibility(WestTerrainGenerated,bMakeWestVisible,bMakeWestInvisible,IndexFromActiveLandscape);
	
	if(bStartNorthRecursion)
		NorthTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::Ve_North,ViewDistance);
	if(bStartEastRecursion)
		EastTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::Ve_East,ViewDistance);
	if( bStartSouthRecursion)
		SouthTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::Ve_South,ViewDistance);
	if(bStartWestRecursion)
		WestTerrainGenerated->RecursiveActiveCheck(IndexFromActiveLandscape+1,EDirectionException::Ve_West,ViewDistance);
}
void AProceduralTerrainGen::SetCardinalDirectionVisibility(AProceduralTerrainGen* Direction, const bool IsVisible, const bool MakeInvisible, const int IndexFromActive)
{
	if(IsVisible)
	{
		if(Direction)
		{
			MakeVisibleComponent(Direction,IndexFromActive+1);
		}
		else
		{
			Direction=GenerateVisibleComponent(EDirectionException::Ve_North,IndexFromActive+1);
		}
	}
	else
	{
		if(MakeInvisible)
		{
			Direction->ProceduralTerrain->UnLoadMesh();
			Direction->TerrainParameters->bVisibleTerrain=false;
		}
	}
	
}
bool AProceduralTerrainGen::CardinalDirectionGeneratedVisible(AProceduralTerrainGen* Direction)
{
	if(Direction)
	{
		if(Direction->ProceduralTerrain->IsGenerated() && Direction->ProceduralTerrain->IsVisible())
		{
			return true;
		}
	}
	return false;
}

void AProceduralTerrainGen::GenerateSiblingLandscapes(const int IndexFromActiveLandscape, const EDirectionException PreviousDirectionException)
{
	switch (TerrainParameters->TerrainShape)
	{
	case EComponentShapes::Ve_Plain:
		{
			if(IndexFromActiveLandscape<TerrainParameters->LandscapeAmount)
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
			if(IndexFromActiveLandscape<ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
			{
				if(PreviousDirectionException == EDirectionException::Ve_North)
				{
					if(TerrainParameters->TerrainShapeSide==EShapeSide::Ve_Top)
					{
						EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right, EClampingNeeds::Ve_Left);
						EastTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_East);
					}
					if(IndexFromActiveLandscape+1==ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopRight);
					}
					else
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Right);
					}
					if(IndexFromActiveLandscape==1)
					{
						if(2 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomLeft);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Bottom);
						}
					}
					else
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_None);
					}
					NorthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_North);
					WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
				}
				if(PreviousDirectionException == EDirectionException::Ve_South)
				{
					if(IndexFromActiveLandscape+1==ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
					{
						SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomRight);
					}
					else
					{
						SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Right);
					}
					if(IndexFromActiveLandscape==1)
					{
						if(IndexFromActiveLandscape+1 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopLeft);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Top);
						}
					}
					else
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_None);
					}
					SouthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_South);
					WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
		
				}
				if(PreviousDirectionException == EDirectionException::Ve_East)
				{
					if(IndexFromActiveLandscape+1==ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
					{
						if(TerrainParameters->bClampTop)
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopRight);
						}
						else if(TerrainParameters->bClampBottom)
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomRight);
						}
						else
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Right);
						}
					}
					else
					{
						if(TerrainParameters->bClampTop)
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Top);
						}
						else if(TerrainParameters->bClampBottom)
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Bottom);
						}
						else
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_None);
						}
					}
					EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_East);
				}
				if(PreviousDirectionException== EDirectionException::Ve_West)
				{
					if(IndexFromActiveLandscape+1==ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
					{
						if(TerrainParameters->bClampTop)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopLeft);
						}
						else if(TerrainParameters->bClampBottom)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomLeft);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Left);
						}
					}
					else
					{
						if(TerrainParameters->bClampTop)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Top);
						}
						else if(TerrainParameters->bClampBottom)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Bottom);
						}
						else if(TerrainParameters->bClampRight)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_None);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_None);
						}
					}
					WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::Ve_West);
				}
				if(PreviousDirectionException==EDirectionException::Ve_None)
				{
					if(ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections)==2)
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,2,EShapeSide::Ve_Top,EClampingNeeds::Ve_TopRight);
					}
					else
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,2,EShapeSide::Ve_Top,EClampingNeeds::Ve_Right);
					}
					SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Front,EClampingNeeds::Ve_TopRight);
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right,EClampingNeeds::Ve_BottomLeft);
					if(2==ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Top,EClampingNeeds::Ve_BottomLeft);

					}
					else
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Top,EClampingNeeds::Ve_Bottom);
					}
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
					if(TerrainParameters->TerrainShapeSide == EShapeSide::Ve_Top)
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,1,EShapeSide::Ve_Back,EClampingNeeds::Ve_BottomRight);
						if(2 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopLeft);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Top);
						}
						EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right,EClampingNeeds::Ve_TopLeft);

						EastTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_East);
						NorthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_North);
						WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
					}
					else if(TerrainParameters->TerrainSubSections !=ESubSections::Ve_One)
					{
						if(2 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopLeft);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Top);
						}
						WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
					}
				}
				if(PreviousDirectionException == EDirectionException::Ve_South)
				{
					if(TerrainParameters->TerrainShapeSide==EShapeSide::Ve_Front)
					{
						if(TerrainParameters->TerrainSubSections!= ESubSections::Ve_One)
						{
							if(2 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
							{
								WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomLeft);
							}
							else
							{
								WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Front,EClampingNeeds::Ve_Bottom);
							}
							WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
							SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Bottom,EClampingNeeds::Ve_TopRight);
							SouthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_South);
						}
						else
						{
							SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Bottom,EClampingNeeds::Ve_All);
							SouthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_South);
						}
					}
					else if(TerrainParameters->TerrainSubSections !=ESubSections::Ve_One)
					{
						if(TerrainParameters->TerrainShapeSide==EShapeSide::Ve_Bottom)
						{
							if(2 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
							{
								WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomLeft);
							}
							else
							{
								WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Bottom,EClampingNeeds::Ve_Bottom);
							}
						}
						WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
					}
				}
				if(PreviousDirectionException == EDirectionException::Ve_West)
				{
					if(TerrainParameters->TerrainShapeSide==EShapeSide::Ve_Top)
					{
						if(TerrainParameters->bClampBottom)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,1,EShapeSide::Ve_Left,EClampingNeeds::Ve_BottomRight);
						}
						else if (TerrainParameters->bClampTop)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,1,EShapeSide::Ve_Left,EClampingNeeds::Ve_TopRight);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,1,EShapeSide::Ve_Left,EClampingNeeds::Ve_Right);
						}
						WestTerrainGenerated->GenerateSiblingLandscapes(1,EDirectionException::Ve_West);
					}
				}
				if(PreviousDirectionException==EDirectionException::Ve_None)
				{
					NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,1,EShapeSide::Ve_Back,EClampingNeeds::Ve_All);
					SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Front,EClampingNeeds::Ve_All);
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right,EClampingNeeds::Ve_All);
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,1,EShapeSide::Ve_Left,EClampingNeeds::Ve_All);

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
			if(IndexFromActiveLandscape<ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
			{
				if(PreviousDirectionException == EDirectionException::Ve_North)
				{
					if(TerrainParameters->TerrainShapeSide==EShapeSide::Ve_Top)
					{
						EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right, EClampingNeeds::Ve_Left);
						EastTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_East);
					}
					if(IndexFromActiveLandscape+1==ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopRight);
					}
					else
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Right);
					}
					if(IndexFromActiveLandscape==1)
					{
						if(2 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomLeft);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Bottom);
						}
					}
					else
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_None);
					}
					NorthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_North);
					WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
				}
				if(PreviousDirectionException == EDirectionException::Ve_South)
				{
					if(IndexFromActiveLandscape+1==ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
					{
						SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomRight);
					}
					else
					{
						SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Right);
					}
					if(IndexFromActiveLandscape==1)
					{
						if(IndexFromActiveLandscape+1 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopLeft);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Top);
						}
					}
					else
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_None);
					}
					SouthTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_South);
					WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
		
				}
				if(PreviousDirectionException == EDirectionException::Ve_East)
				{
					if(IndexFromActiveLandscape+1==ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
					{
						if(TerrainParameters->bClampTop)
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopRight);
						}
						else if(TerrainParameters->bClampBottom)
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomRight);
						}
						else
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Right);
						}
					}
					else
					{
						if(TerrainParameters->bClampTop)
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Top);
						}
						else if(TerrainParameters->bClampBottom)
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Bottom);
						}
						else
						{
							EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_None);
						}
					}
					EastTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1, EDirectionException::Ve_East);
				}
				if(PreviousDirectionException== EDirectionException::Ve_West)
				{
					if(IndexFromActiveLandscape+1==ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
					{
						if(TerrainParameters->bClampTop)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopLeft);
						}
						else if(TerrainParameters->bClampBottom)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomLeft);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Left);
						}
					}
					else
					{
						if(TerrainParameters->bClampTop)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Top);
						}
						else if(TerrainParameters->bClampBottom)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Bottom);
						}
						else if(TerrainParameters->bClampRight)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_None);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,IndexFromActiveLandscape+1,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_None);
						}
					}
					WestTerrainGenerated->GenerateSiblingLandscapes(IndexFromActiveLandscape+1,EDirectionException::Ve_West);
				}
				if(PreviousDirectionException==EDirectionException::Ve_None)
				{
					if(ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections)==2)
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,2,EShapeSide::Ve_Top,EClampingNeeds::Ve_TopRight);
					}
					else
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,2,EShapeSide::Ve_Top,EClampingNeeds::Ve_Right);
					}
					SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Front,EClampingNeeds::Ve_TopRight);
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right,EClampingNeeds::Ve_BottomLeft);
					if(2==ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Top,EClampingNeeds::Ve_BottomLeft);

					}
					else
					{
						WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Top,EClampingNeeds::Ve_Bottom);
					}
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
					if(TerrainParameters->TerrainShapeSide == EShapeSide::Ve_Top)
					{
						NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,1,EShapeSide::Ve_Back,EClampingNeeds::Ve_BottomRight);
						if(2 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopLeft);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Top);
						}
						EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right,EClampingNeeds::Ve_TopLeft);

						EastTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_East);
						NorthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_North);
						WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
					}
					else if(TerrainParameters->TerrainSubSections !=ESubSections::Ve_One)
					{
						if(2 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_TopLeft);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_Top);
						}
						WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
					}
				}
				if(PreviousDirectionException == EDirectionException::Ve_South)
				{
					if(TerrainParameters->TerrainShapeSide==EShapeSide::Ve_Front)
					{
						if(TerrainParameters->TerrainSubSections!= ESubSections::Ve_One)
						{
							if(2 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
							{
								WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomLeft);
							}
							else
							{
								WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Front,EClampingNeeds::Ve_Bottom);
							}
							WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
							SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Bottom,EClampingNeeds::Ve_TopRight);
							SouthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_South);
						}
						else
						{
							SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Bottom,EClampingNeeds::Ve_All);
							SouthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_South);
						}
					}
					else if(TerrainParameters->TerrainSubSections !=ESubSections::Ve_One)
					{
						if(TerrainParameters->TerrainShapeSide==EShapeSide::Ve_Bottom)
						{
							if(2 == ConvertEnumSubSectionsToInteger(TerrainParameters->TerrainSubSections))
							{
								WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,TerrainParameters->TerrainShapeSide,EClampingNeeds::Ve_BottomLeft);
							}
							else
							{
								WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,2,EShapeSide::Ve_Bottom,EClampingNeeds::Ve_Bottom);
							}
						}
						WestTerrainGenerated->GenerateSiblingLandscapes(2,EDirectionException::Ve_West);
					}
				}
				if(PreviousDirectionException == EDirectionException::Ve_West)
				{
					if(TerrainParameters->TerrainShapeSide==EShapeSide::Ve_Top)
					{
						if(TerrainParameters->bClampBottom)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,1,EShapeSide::Ve_Left,EClampingNeeds::Ve_BottomRight);
						}
						else if (TerrainParameters->bClampTop)
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,1,EShapeSide::Ve_Left,EClampingNeeds::Ve_TopRight);
						}
						else
						{
							WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,1,EShapeSide::Ve_Left,EClampingNeeds::Ve_Right);
						}
						WestTerrainGenerated->GenerateSiblingLandscapes(1,EDirectionException::Ve_West);
					}
				}
				if(PreviousDirectionException==EDirectionException::Ve_None)
				{
					NorthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_North,1,EShapeSide::Ve_Back,EClampingNeeds::Ve_All);
					SouthTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_South,1,EShapeSide::Ve_Front,EClampingNeeds::Ve_All);
					EastTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_East,1,EShapeSide::Ve_Right,EClampingNeeds::Ve_All);
					WestTerrainGenerated =GenerateVisibleComponent(EDirectionException::Ve_West,1,EShapeSide::Ve_Left,EClampingNeeds::Ve_All);

					NorthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_North);
					SouthTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_South);
					EastTerrainGenerated->GenerateSiblingLandscapes(1, EDirectionException::Ve_East);
					WestTerrainGenerated->GenerateSiblingLandscapes(1,EDirectionException::Ve_West);
			
				}
			}
			break;
		}
	default: ;
	}
	
}

void AProceduralTerrainGen::GenerateCurrentLandscape() const
{
	/**Set Noise Resolution*/
	ProceduralTerrain->InitializeVariables(NoiseParameters);
	ProceduralTerrain->SetClamping(TerrainParameters->bClampTop,TerrainParameters->bClampBottom
		,TerrainParameters->bClampRight,TerrainParameters->bClampLeft);
	switch (TerrainParameters->TerrainShape)
	{
	case EComponentShapes::Ve_Plain:
		{
			ProceduralTerrain->GenerateMap(NoiseParameters->NoiseComponentStartLocation);
			break;
		}
	case EComponentShapes::Ve_Cube:
		{
			ProceduralTerrain->GenerateMap(NoiseParameters->NoiseComponentStartLocation,TerrainParameters->TerrainShapeSide,TerrainParameters->TerrainSubSections);
			break;
		}
	case EComponentShapes::Ve_Sphere:
		{
			ProceduralTerrain->GenerateMapSphere(NoiseParameters->NoiseComponentStartLocation,TerrainParameters->TerrainShapeSide,TerrainParameters->TerrainSubSections);
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
	TerrainGen->TerrainParameters->bVisibleTerrain=true;
	TerrainGen->TerrainParameters->CurrentIndexFromActiveLandscape=IndexFromActiveLandscape;
}

AProceduralTerrainGen* AProceduralTerrainGen::GenerateVisibleComponent(const EDirectionException PreviousDirectionException,const int IndexFromActiveLandscape, const EShapeSide TerrainSide,EClampingNeeds ClampingNeeds)
{
	AProceduralTerrainGen* TerrainGen;
	switch (PreviousDirectionException)
	{
	case EDirectionException::Ve_North:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X + NoiseParameters->TotalSizeToGenerate-NoiseParameters->NoiseResolution, GetActorLocation().Y, GetActorLocation().Z,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide,ClampingNeeds);
			}
			else
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,GetActorLocation().Z - NoiseParameters->TotalSizeToGenerate+NoiseParameters->NoiseResolution,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide,ClampingNeeds);
			}
			break;
		}
	case EDirectionException::Ve_South:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X - NoiseParameters->TotalSizeToGenerate+NoiseParameters->NoiseResolution, GetActorLocation().Y,GetActorLocation().Z,IndexFromActiveLandscape,PreviousDirectionException,TerrainSide,ClampingNeeds);
			}
			else if(TerrainSide==EShapeSide::Ve_Front)
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,GetActorLocation().Z - NoiseParameters->TotalSizeToGenerate+NoiseParameters->NoiseResolution,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide,ClampingNeeds);
			}
			else
			{
				if(IndexFromActiveLandscape==1)
				{
					TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide,ClampingNeeds);
				}
				else
				{
					TerrainGen =CreateProceduralTerrain(GetActorLocation().X + NoiseParameters->TotalSizeToGenerate-NoiseParameters->NoiseResolution, GetActorLocation().Y, GetActorLocation().Z,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide,ClampingNeeds);
				}
			}
			break;
		}
	case EDirectionException::Ve_East:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y+ NoiseParameters->TotalSizeToGenerate-NoiseParameters->NoiseResolution,GetActorLocation().Z,IndexFromActiveLandscape,PreviousDirectionException,TerrainSide,ClampingNeeds);
			}
			else
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,GetActorLocation().Z - NoiseParameters->TotalSizeToGenerate+NoiseParameters->NoiseResolution,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide,ClampingNeeds);
				
			}
			break;
		}
	case EDirectionException::Ve_West:
		{
			if(TerrainSide!=EShapeSide::Ve_Left)
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y- NoiseParameters->TotalSizeToGenerate+NoiseParameters->NoiseResolution,GetActorLocation().Z,IndexFromActiveLandscape,PreviousDirectionException,TerrainSide,ClampingNeeds);
			}
			else
			{
				TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,GetActorLocation().Z - NoiseParameters->TotalSizeToGenerate+NoiseParameters->NoiseResolution,IndexFromActiveLandscape, PreviousDirectionException,TerrainSide,ClampingNeeds);
			}
			break;
		}
	default:
		{
			TerrainGen =CreateProceduralTerrain(GetActorLocation().X, GetActorLocation().Y,GetActorLocation().Z,IndexFromActiveLandscape,PreviousDirectionException,TerrainSide,ClampingNeeds);
		}
	}
	if(TerrainGen)
	{
		TerrainGen->TerrainParameters->bVisibleTerrain=true;
		TerrainGen->TerrainParameters->CurrentIndexFromActiveLandscape=IndexFromActiveLandscape;
	}
	return TerrainGen;
}

AProceduralTerrainGen* AProceduralTerrainGen::CreateProceduralTerrain(const int PosX, const int PosY, const int PosZ, const int CurrentLandscapeIndex, const EDirectionException PreviousDirectionException, const EShapeSide TerrainSide,EClampingNeeds ClampingNeeds)
{
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	AProceduralTerrainGen* ProceduralTerrainGen = GetWorld()->SpawnActor<AProceduralTerrainGen>(
		FVector(PosX,PosY,PosZ),GetActorRotation(),SpawnParams);
	
	ProceduralTerrainGen->TerrainParameters->LandscapeAmount =TerrainParameters->LandscapeAmount;
	if(ProceduralTerrainGen->TerrainParameters->CurrentIndexFromActiveLandscape!= 0 && ProceduralTerrainGen->TerrainParameters->CurrentIndexFromActiveLandscape>CurrentLandscapeIndex)
	{
		ProceduralTerrainGen->TerrainParameters->CurrentIndexFromActiveLandscape =CurrentLandscapeIndex;
		switch (ClampingNeeds)
		{
		case EClampingNeeds::Ve_None:
			{
				ProceduralTerrainGen->SetNewClamps(false,false,false,false);
				break;
			}
		case EClampingNeeds::Ve_All:
			{
				ProceduralTerrainGen->SetNewClamps(true,true,true,true);
				break;
			}
		case EClampingNeeds::Ve_SameAsPrevious:
			{
				ProceduralTerrainGen->SetNewClamps(TerrainParameters->bClampTop,TerrainParameters->bClampBottom,TerrainParameters->bClampRight,TerrainParameters->bClampLeft);
				break;
			}
		case EClampingNeeds::Ve_Top:
			{
				ProceduralTerrainGen->SetNewClamps(true,false,false,false);
				break;
			}
		case EClampingNeeds::Ve_Bottom:
			{
				ProceduralTerrainGen->SetNewClamps(false,true,false,false);
				break;
			}
		case EClampingNeeds::Ve_Left:
			{
				ProceduralTerrainGen->SetNewClamps(false,false,false,true);
				break;
			}
		case EClampingNeeds::Ve_Right:
			{
				ProceduralTerrainGen->SetNewClamps(false,false,true,false);
				break;
			}
		case EClampingNeeds::Ve_TopRight:
			{
				ProceduralTerrainGen->SetNewClamps(true,false,true,false);
				break;
			}
		case EClampingNeeds::Ve_TopLeft:
			{
				ProceduralTerrainGen->SetNewClamps(true,false,false,true);
				break;
			}
		case EClampingNeeds::Ve_BottomLeft:
			{
				ProceduralTerrainGen->SetNewClamps(false,true,false,true);
				break;
			}
		case EClampingNeeds::Ve_BottomRight:
			{
				ProceduralTerrainGen->SetNewClamps(false,true,true,false);
				break;
			}
		default: ;
		}
	}
	
	ProceduralTerrainGen->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
	InitializeVariable(ProceduralTerrainGen);
	ProceduralTerrainGen->TerrainParameters->TerrainShapeSide =TerrainSide;
	switch (PreviousDirectionException)
	{
	case EDirectionException::Ve_North:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				ProceduralTerrainGen->NoiseParameters->NoiseComponentStartLocation = NoiseParameters->NoiseComponentStartLocation+ FVector(NoiseParameters->NoiseSamplesPerLine-1,0,0);
			}
			else
			{
				ProceduralTerrainGen->NoiseParameters->NoiseComponentStartLocation = NoiseParameters->NoiseComponentStartLocation+ FVector(0,0,-(NoiseParameters->NoiseSamplesPerLine-1));
			}
			break;
		}
	case EDirectionException::Ve_South:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				ProceduralTerrainGen->NoiseParameters->NoiseComponentStartLocation = NoiseParameters->NoiseComponentStartLocation+ FVector(-(NoiseParameters->NoiseSamplesPerLine-1),0,0);
			}
			else if(TerrainSide==EShapeSide::Ve_Front)
			{
				ProceduralTerrainGen->NoiseParameters->NoiseComponentStartLocation = NoiseParameters->NoiseComponentStartLocation+ FVector(0,0,-(NoiseParameters->NoiseSamplesPerLine-1));
			}
			else
			{
				ProceduralTerrainGen->NoiseParameters->NoiseComponentStartLocation = NoiseParameters->NoiseComponentStartLocation+  FVector(NoiseParameters->NoiseSamplesPerLine-1,0,0);
			}
			break;
		}
	case EDirectionException::Ve_East:
		{
			if(TerrainSide==EShapeSide::Ve_Top)
			{
				ProceduralTerrainGen->NoiseParameters->NoiseComponentStartLocation = NoiseParameters->NoiseComponentStartLocation+ FVector(0,NoiseParameters->NoiseSamplesPerLine-1,0);
			}
			else
			{
				ProceduralTerrainGen->NoiseParameters->NoiseComponentStartLocation = NoiseParameters->NoiseComponentStartLocation+ FVector(0,0,-(NoiseParameters->NoiseSamplesPerLine-1));
			}
			break;
		}
	case EDirectionException::Ve_West:
		{
			if(TerrainSide!=EShapeSide::Ve_Left)
			{
				ProceduralTerrainGen->NoiseParameters->NoiseComponentStartLocation = NoiseParameters->NoiseComponentStartLocation+ FVector(0,-(NoiseParameters->NoiseSamplesPerLine-1),0);
			}
			else
			{
				ProceduralTerrainGen->NoiseParameters->NoiseComponentStartLocation = NoiseParameters->NoiseComponentStartLocation+ FVector(0,0,-(NoiseParameters->NoiseSamplesPerLine-1));
			}
			break;
		}
	default:
		{
			ProceduralTerrainGen->NoiseParameters->NoiseComponentStartLocation = NoiseParameters->NoiseComponentStartLocation;
		}
	}
	if(TerrainParameters->TerrainShape==EComponentShapes::Ve_Sphere)
	{
		ProceduralTerrainGen->GenerateSphereInformation();
	}
	ProceduralTerrainGen->GenerateCurrentLandscape();

	return ProceduralTerrainGen;
}
void AProceduralTerrainGen::GenerateSphereInformation()
{
	if(TerrainParameters->TerrainSubSections==ESubSections::Ve_One)
	{
		switch (TerrainParameters->TerrainShapeSide)
		{
		case EShapeSide::Ve_Bottom:
			{
				break;
			}
		case EShapeSide::Ve_Left:
			{
				break;
			}
		case EShapeSide::Ve_Right:
			{
				break;
			}
		case EShapeSide::Ve_Front:
			{
				break;
			}
		case EShapeSide::Ve_Back:
			{
				break;
			}
		default: ;
		}
	}
	else
	{
		//#TODO Implement subsections of the sphere
	}
}
void AProceduralTerrainGen::InitializeVariable(AProceduralTerrainGen* Proc) const
{
	Proc->NoiseParameters->InitializeVariables(NoiseParameters);
	Proc->TerrainParameters->InitializeVariables(TerrainParameters);
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