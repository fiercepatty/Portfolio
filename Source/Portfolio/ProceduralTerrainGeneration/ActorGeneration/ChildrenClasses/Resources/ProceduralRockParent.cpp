// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralRockParent.h"

AProceduralRockParent::AProceduralRockParent()
{
	ResourceType = EResourceTypes::Rock;
}

float AProceduralRockParent::HitResource(const float DamageAmount, const float MiningEffectiveness)
{
	Health-=DamageAmount*RockSturdiness;
	UpdateHealthMesh();
	return DamageAmount*DropAmount*MiningEffectiveness * RockSturdiness;
}
