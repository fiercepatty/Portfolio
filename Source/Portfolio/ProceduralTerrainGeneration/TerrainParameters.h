// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TerrainEnums.h"
#include "TerrainParameters.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_API UTerrainParameters : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTerrainParameters();

	void InitializeVariables(const UTerrainParameters* TerrainValues);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	EComponentShapes TerrainShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	EShapeSide TerrainShapeSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	ESubSections TerrainSubSections;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	int CurrentIndexFromActiveLandscape = 999;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	int LandscapeAmount=3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings", meta = (AllowPrivateAccess = "true"))
	int ChunkViewDistance=3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	bool bActiveTerrain = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	bool bVisibleTerrain =true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	bool bClampTop =false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	bool bClampBottom = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	bool bClampLeft = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	bool bClampRight =false;
};
