// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralTerrainComponent.h"
#include "GameFramework/Actor.h"
#include "TerrainStructInfo.h"
#include "ProceduralTerrainGen.generated.h"


UCLASS()
class PORTFOLIO_API AProceduralTerrainGen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralTerrainGen();

	bool bGettingDestroyed=false;
	

	UPROPERTY(VisibleAnywhere)
	UProceduralTerrainComponent* ProceduralTerrain;

	FVector NoiseComponentStartLocation;

	
	void GenerateTerrain();
	
	void GenerateCurrentLandscape() const;

	/**
	 * 
	 */
	void InitializeVariable(FTerrainInfo TerrainInfo) const;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	AProceduralTerrainGen* EastTerrainGenerated;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	AProceduralTerrainGen* WestTerrainGenerated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	AProceduralTerrainGen* NorthTerrainGenerated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	AProceduralTerrainGen* SouthTerrainGenerated;

private:
	
	FHitResult LineTrace(FVector StartLocation, float OutputScale) const;

	void FindConnections(AProceduralTerrainGen* CurrentTerrain) const;
	

	
	

};



