// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/ProceduralPlayerComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UProceduralPlayerComponent::UProceduralPlayerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//Initizling the actual component. I tried to go as much in C++ so that all you would need to do is attach this component on a character and it will be able to walk around
	PlayerTriggerBox=CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerBoxComponent"));
	PlayerTriggerBox->SetCollisionProfileName(FName(TEXT("Pawn")));
	PlayerTriggerBox->OnComponentBeginOverlap.AddDynamic(this,&UProceduralPlayerComponent::PlayerBeginOverlap);
	PlayerTriggerBox->SetGenerateOverlapEvents(true);
	PlayerTriggerBox->SetBoxExtent(FVector(25));
}

void UProceduralPlayerComponent::BeginPlay()
{
	Super::BeginPlay();
	//Doing a hard find for the Manager but there should only ever be one in the world and we only have to do this once so it shouldnt be too bad
	Manager = Cast<ATerrainManager>(UGameplayStatics::GetActorOfClass(GetWorld(),ATerrainManager::StaticClass()));
	if(Manager)
	{
		GEngine->AddOnScreenDebugMessage(1,10,FColor::Red,TEXT("Successful Found the Manager"));
		CurrentActiveTerrain=Manager->GetOrigin();
	}
	
}

void UProceduralPlayerComponent::PlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComponent, int32 OtherBoxIndex, bool FromSweep, const FHitResult& SweepResult)
{
	//If the actor is a terrain then check if it is our current active terrain
	AProceduralTerrainGen* ATerrain = Cast<AProceduralTerrainGen>(OtherActor);
	if(ATerrain)
	{
		//If it is NOT the current terrain we check to see which of the blocks neighbors it was so we can move the square accordingly
		if(ATerrain != CurrentActiveTerrain)
		{
			if(CurrentActiveTerrain->NorthTerrainGenerated == ATerrain)
			{
				Manager->MoveSquareNorth();
				CurrentActiveTerrain=ATerrain;

			}
			else if(CurrentActiveTerrain->EastTerrainGenerated==ATerrain)
			{
				Manager->MoveSquareEast();
				CurrentActiveTerrain=ATerrain;

			}
			else if(CurrentActiveTerrain->SouthTerrainGenerated==ATerrain)
			{
				Manager->MoveSquareSouth();
				CurrentActiveTerrain=ATerrain;

			}
			else if(CurrentActiveTerrain->WestTerrainGenerated==ATerrain)
			{
				Manager->MoveSquareWest();
				CurrentActiveTerrain=ATerrain;
			}
		}
	}
}

