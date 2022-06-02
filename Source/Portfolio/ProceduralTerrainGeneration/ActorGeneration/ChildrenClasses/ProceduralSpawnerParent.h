// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralTerrainGeneration/ActorGeneration/ProceduralActorParent.h"
#include "ProceduralSpawnerParent.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AProceduralSpawnerParent : public AProceduralActorParent
{
	GENERATED_BODY()

	
public:
	AProceduralSpawnerParent();

	/**Handle Spawning in a Wave*/
	virtual void Tick(float DeltaSeconds) override;

protected:
	/**How often a wave will spawn in*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Spawner", meta=(AllowProtectedAccess =true))
	float SpawningInterval =5.0f;

	/**How much time is remaining to spawn in a wave*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Spawner", meta=(AllowProtectedAccess =true))
	float SpawningTimeRemaining = 10.0f;

	/**Whether or not to ignore the lifetime for the spawner*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Spawner", meta=(AllowProtectedAccess =true))
	bool bIgnoreLifetime = false;
	
	/**How long a spawner will be alive for*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Spawner", meta=(AllowProtectedAccess =true))
	float SpawningLifeTime = 100;

	/**What actors to spawn in*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Spawner", meta=(AllowProtectedAccess =true))
	TArray<TSubclassOf<AActor>> SpawningActors;

	/**If the spawner is disable ignore any spawning calls and this actor will do nothing*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Spawner", meta=(AllowProtectedAccess =true))
	bool bDisableSpawner = false;

	//Helper function to pick the actor to spawn in the world
	TSubclassOf<AActor> ChooseActorToSpawn();

	//Helper function to spawn in an actor
	void SpawnInActor(TSubclassOf<AActor> ActorClass);
};
