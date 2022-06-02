// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/ProceduralInventory.h"

#include "ProceduralRockParent.h"
#include "ProceduralTreeParent.h"

// Sets default values for this component's properties
UProceduralInventory::UProceduralInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UProceduralInventory::AddResourceToInventory(AProceduralResourceParent* Resource, const float AmtToAdd)
{
	if(Cast<AProceduralRockParent>(Resource))
		AmtOfRocks+=AmtToAdd;
	else if(Cast<AProceduralTreeParent>(Resource))
		AmtOfTree+=AmtToAdd;
}


