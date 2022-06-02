// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/ActorGeneration/ChildrenClasses/ProceduralSpawnerParent.h"

AProceduralSpawnerParent::AProceduralSpawnerParent()
{
}

void AProceduralSpawnerParent::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bDisableSpawner)
	{
		if(!bIgnoreLifetime)
		{
			SpawningLifeTime-=DeltaSeconds;
			if(SpawningLifeTime<0)
			{
				Destroy();
			}
		}
		SpawningTimeRemaining-=DeltaSeconds;
		if(SpawningTimeRemaining<0)
		{
			SpawnInActor(ChooseActorToSpawn());
			SpawningTimeRemaining=SpawningInterval;
		}
	}
	
}

TSubclassOf<AActor> AProceduralSpawnerParent::ChooseActorToSpawn()
{
	//Could put weights for spawning in actors
	const int RandomNum = FMath::RandRange(0,SpawningActors.Num()-1);
	return SpawningActors[RandomNum];
}

void AProceduralSpawnerParent::SpawnInActor(const TSubclassOf<AActor> ActorClass)
{
	const FVector Location = GetActorLocation();
	const FRotator Rotation = GetActorRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner =this;
	
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClass,Location,Rotation,SpawnParams);
	
}
