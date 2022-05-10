// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralTerrainGen.h"
#include "TerrainManager.h"
#include "Components/ActorComponent.h"
#include "ProceduralPlayerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_API UProceduralPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralPlayerComponent();

	/**The terrain piece that the player is currently on. So the center of the square that the terrain generates*/
	AProceduralTerrainGen* CurrentActiveTerrain;

	/**Terrain Manager Map Used to update the Map on direction the player is heading*/
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	ATerrainManager* Manager;

	/**The Actual box that is handling the hit detection for creating more map*/
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UBoxComponent* PlayerTriggerBox;

	//Using this to find the manager in the world so we can tell it to create more map
	virtual void BeginPlay() override;

	/**This is where we are find out which direction we need to extend the map in. We just check to see if we are not in the same block as we were before and then we check the block's neighbors that we are in and see which direction we traveled in*/
	UFUNCTION()
	void PlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComponent, int32 OtherBoxIndex, bool FromSweep, const FHitResult& SweepResult);

	
	
};
