// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/ActorGeneration/ChildrenClasses/Resources/ProceduralTreeParent.h"

AProceduralTreeParent::AProceduralTreeParent()
{
	ResourceType = EResourceTypes::Tree;
}

float AProceduralTreeParent::HitResource(const float DamageAmount, const float MiningEffectiveness)
{
	Health-=DamageAmount;
	UpdateHealthMesh();
	return DamageAmount*DropAmount*MiningEffectiveness;
}
