// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralResourceParent.h"
#include "Components/ActorComponent.h"
#include "ProceduralInventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_API UProceduralInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralInventory();

	void AddResourceToInventory(AProceduralResourceParent* Resource, float AmtToAdd);

	

protected:

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Player Inventory",meta = (AllowProtectedAccess=true))
	float AmtOfRocks = 0;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Player Inventory",meta = (AllowProtectedAccess=true))
	float AmtOfTree =0;


		
};
