// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoiseParameters.h"
#include "ProceduralTerrainComponent.h"
#include "GameFramework/Actor.h"
#include "TerrainEnums.h"
#include "TerrainParameters.h"
#include "ProceduralTerrainGen.generated.h"

UCLASS()
class PORTFOLIO_API AProceduralTerrainGen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralTerrainGen();
	
	bool bNeedDestroyed = false;
	bool bNeverDestroy = false;

	UPROPERTY(VisibleAnywhere)
	UProceduralTerrainComponent* ProceduralTerrain;

	UPROPERTY(VisibleAnywhere)
	UNoiseParameters* NoiseParameters;

	UPROPERTY(VisibleAnywhere)
	UTerrainParameters* TerrainParameters;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void GenerateTerrain();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void DeleteTerrain();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void UpdateVisibleTerrain();

	void RecursiveActiveCheck(int IndexFromActiveLandscape, EDirectionException PreviousDirectionException, int ViewDistance);

	void GenerateSiblingLandscapes(int IndexFromActiveLandscape, EDirectionException PreviousDirectionException);
	
	void GenerateCurrentLandscape() const;

	void InitializeVariable(AProceduralTerrainGen* Proc) const;
	
	void RecursiveDelete();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	AProceduralTerrainGen* EastTerrainGenerated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	AProceduralTerrainGen* WestTerrainGenerated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	AProceduralTerrainGen* NorthTerrainGenerated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	AProceduralTerrainGen* SouthTerrainGenerated;
	
	void GenerateSphereInformation();
	
	static void MakeVisibleComponent(AProceduralTerrainGen* TerrainGen, int IndexFromActiveLandscape );
	AProceduralTerrainGen* GenerateVisibleComponent(EDirectionException PreviousDirectionException,int IndexFromActiveLandscape, EShapeSide TerrainSide= EShapeSide::Ve_Top, EClampingNeeds ClampingNeeds= EClampingNeeds::Ve_None);

	AProceduralTerrainGen* CreateProceduralTerrain(int PosX, int PosY,int PosZ, int CurrentLandscapeIndex, EDirectionException PreviousDirectionException, EShapeSide TerrainSide, EClampingNeeds ClampingNeeds);

	static int ConvertEnumSubSectionsToInteger(ESubSections AShapeSubSections);

	void SetNewClamps(const bool Top, const bool Bottom,const bool Right,const bool Left){TerrainParameters->bClampTop=Top;TerrainParameters->bClampBottom=Bottom;
		TerrainParameters->bClampRight=Right;TerrainParameters->bClampLeft=Left;}

	void SetCardinalDirectionVisibility(AProceduralTerrainGen* Direction, bool IsVisible,bool MakeInvisible, int IndexFromActive);

	static bool CardinalDirectionGeneratedVisible(AProceduralTerrainGen* Direction);
};



