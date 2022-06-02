// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralResourceParent.h"
#include "ProceduralRockParent.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AProceduralRockParent : public AProceduralResourceParent
{
	GENERATED_BODY()

public:
	AProceduralRockParent();

	/**Override the Hit resource to implement what happens when the rock is Hit*/
	virtual float HitResource(float DamageAmount, float MiningEffectiveness) override;

protected:
	/**Multiplied in the Hit resource function to make the gathering less efficient when desired*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Rock", meta=(AllowProtectedAccess =true))
	float RockSturdiness= 0.3;
	
	
	
};
