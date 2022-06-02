// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/ActorGeneration/ChildrenClasses/ProceduralResourceParent.h"

AProceduralResourceParent::AProceduralResourceParent()
{
	ResourceStaticMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ResourceStaticMesh"));
}

float AProceduralResourceParent::HitResource(float DamageAmount, float MiningEffectiveness)
{
	return 0;
}


void AProceduralResourceParent::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(bNeedDestroy)
	{
		Destroy();
	}
}

void AProceduralResourceParent::SetVisibility(const bool NewVisibility) const
{
	ResourceStaticMesh->SetVisibility(NewVisibility);
}

void AProceduralResourceParent::UpdateHealthMesh()
{
	if(Health<=0)
		bNeedDestroy=true;
	else
	{
		const int AmtOfStaticMeshes = ResourceStaticMeshDestructionProgress.Num();
		if(AmtOfStaticMeshes > 1)
		{
			const int PerIndex =MaxHealth / AmtOfStaticMeshes;
			const int AmtFromFull =MaxHealth- Health;
			const int Index =( AmtFromFull / PerIndex);
			ResourceStaticMesh->SetStaticMesh(ResourceStaticMeshDestructionProgress[Index]);
			//#TODO Might need to add instance
			//ResourceStaticMesh->AddInstance();
		}
	}
}
