#pragma once
#include "FastNoiseWrapper.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActorStructInfo.generated.h"


UENUM(BlueprintType)
enum class EActorTypes : uint8
{
	Resource		UMETA(DisplayName = "Resource"),
	Spawner			UMETA(DisplayName = "Spawner"),
};


UENUM(BlueprintType)
enum class EResourceTypes : uint8
{
	Tree			UMETA(DisplayName = "Tree"),
	Rock			UMETA(DisplayName = "Rock"),
};


USTRUCT(BlueprintType)
struct FResourceInfo
{
	GENERATED_USTRUCT_BODY()

	FResourceInfo()
	{
	}
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	EResourceTypes Resource = EResourceTypes::Tree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	UStaticMesh* TreeMesh =nullptr;

	
};

USTRUCT(BlueprintType)
struct FSpawnerInfo
{
	GENERATED_USTRUCT_BODY()

	FSpawnerInfo()
	{
	}
	
public:
	
};