// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorStructInfo.h"
#include "ProceduralTerrainGeneration/ActorGeneration/ChildrenClasses/ProceduralResourceParent.h"
#include "ProceduralTreeParent.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AProceduralTreeParent : public AProceduralResourceParent
{
	GENERATED_BODY()
	

public:
	AProceduralTreeParent();

	/**Override the Hit resource to implement what happens when the Tree is Hit*/
	virtual float HitResource(float DamageAmount, float MiningEffectiveness) override;

	
	
};
