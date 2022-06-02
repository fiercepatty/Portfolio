// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralTerrainGeneration/ActorGeneration/ProceduralActorParent.h"
#include "ActorStructInfo.h"
#include "ProceduralResourceParent.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AProceduralResourceParent : public AProceduralActorParent
{
	GENERATED_BODY()

public:
	AProceduralResourceParent();

	/**
	 * Whenever a resource is hit this tells how much of that resource this dropped
	 * @param DamageAmount How much Damage to do to the Resource
	 * @param MiningEffectiveness Percentage to increase or decrease the dropped resources by
	 * @returns How many pieces of resource dropped
	 */
	virtual float HitResource(float DamageAmount, float MiningEffectiveness);

	/**Every Tick check to see if we need to be destroyed*/
	virtual void Tick(float DeltaSeconds) override;

	/**Set the visiblity of the static mesh*/
	void SetVisibility(bool NewVisibility) const;
	
	

protected:
	/**
	 * How much Health an Actor Spawned in Will Have
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Resource", meta=(AllowProtectedAccess =true))
	float Health = 0;

	/**
	 * The Most Health an Actor Spawned in can Have
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Resource", meta=(AllowProtectedAccess =true))
	float MaxHealth = 0;
	
	/**
	 * How many Resources are dropped when Hit
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Resource", meta=(AllowProtectedAccess =true))
	float DropAmount = 0;

	/**Type of resource the node is*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Resource", meta=(AllowProtectedAccess =true))
	EResourceTypes ResourceType; 

	/**
	 * How many Resources are dropped when Hit
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Resource", meta=(AllowProtectedAccess =true))
	bool bNeedDestroy = false;

	/**
	 * Starting Static Mesh when Spawned in
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Resource", meta=(AllowProtectedAccess =true))
	UStaticMeshComponent* ResourceStaticMesh = nullptr;	
	

	/**
	 * StaticMeshes to swap in when actor is damaged in progression
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Actor | Resource", meta=(AllowProtectedAccess =true))
	TArray<UStaticMesh*> ResourceStaticMeshDestructionProgress = TArray<UStaticMesh*>();

	//The current Static Mesh the resource is using, if 0 then the Mesh is at full health
	int CurrentMeshActive = 0;

	/**Update the Health Mesh based on how much health the Mesh has*/
	void UpdateHealthMesh();
	
};
