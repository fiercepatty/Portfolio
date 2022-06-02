// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralFoliageComponent.h"
#include "ProceduralTerrainComponent.h"
#include "ProceduralWaterComponent.h"
#include "GameFramework/Actor.h"
#include "TerrainStructInfo.h"
#include "Components/BoxComponent.h"
#include "ProceduralTerrainGen.generated.h"


UCLASS()
class PORTFOLIO_API AProceduralTerrainGen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralTerrainGen();

	
	//Boolean for when the manager is destroying all of the planes we dont want it to try to destroy something twice so this is used to prevent it from adding something in the array twice
	bool bGettingDestroyed=false;

	//Boolean for whether or not water is enable for this terrain
	bool bWaterEnabled = false;

	//Used to interact with the procedural player component to let the manager spawn in more map
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TerrainTriggerBox;

	//This is what actually creates the mesh that is attached to it
	UPROPERTY(VisibleAnywhere)
	UProceduralTerrainComponent* ProceduralTerrain;

	/**Draw the foliage squares using the Foliage Debug settings*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Foliage Debug Settings")
	void DrawNatureSquares();

	/**How long the squares are visible for*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Debug Settings")
	float DebugDrawTime = 25.0f;

	/**Debug Box Thickness*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Debug Settings")
	float DebugBoxThickness =5.0f;

	/**Debug Box Persistent Lines*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Debug Settings")
	bool bPersistentLines =false;

	/**Debug Box Depth Priority*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Debug Settings")
	int DepthPriority =0;

	/**Color for debug trace if the square is not in the shade or in use*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Debug Settings")
	FColor ColorNoFoliageOrShade =FColor::Red;

	/**Color for debug trace if the square is in the shade*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Debug Settings")
	FColor ColorInShade =FColor::Black;

	/**Color for debug trace if the square is in use*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Debug Settings")
	FColor ColorInUse =FColor::Blue;

	//This is what actually create the mesh for the water that is attached to it
	UPROPERTY(VisibleAnywhere)
	UProceduralWaterComponent* ProceduralWater;

	//Tell the Noise Wrapper where to start indexing into the mesh at
	FVector NoiseComponentStartLocation;

	//Function to actually generate the terrain
	void GenerateTerrain();

	/**
	 * Used to generate the nature for a specific terrain
	 */
	void GenerateNatureInternal();

	/**
	 * @param TerrainInfo - this is the Noise Parameter used to create the Mesh
	 * @param ConnectedTerrainInfos - the terrain in for all of the connected Terrains
	 * This function is used to setup all the fast noise wrapper settings that need to be set before we can generate the mesh
	 */
	void InitializeVariable(FTerrainInfo TerrainInfo, TArray<FTerrainInfo> ConnectedTerrainInfos) ;
	

	//LoadingTerrainIn
	void LoadTerrain() ;

	//Unloading Terrain
	void UnloadTerrain();

	//Function used to hide nature
	void HideNature();

	//Function used to load nature
	void LoadNature();

	//This was done in a way like a linked list
	
	/**The Connected Terrain in the East Direction*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	AProceduralTerrainGen* EastTerrainGenerated;

	/**The Connected Terrain in the West Direction*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	AProceduralTerrainGen* WestTerrainGenerated;

	/**The Connected Terrain in the North Direction*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	AProceduralTerrainGen* NorthTerrainGenerated;

	/**The Connected Terrain in the South Direction*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Terrain Settings")
	AProceduralTerrainGen* SouthTerrainGenerated;

	
private:
	//Helper function to create the terrain 
	void GenerateCurrentLandscape() const;
	
	//Helper function to do a line trace at a location
	/**
	 * @param StartLocation The Place to do the straight up and done line trace to try and find a terrain
	 * @param OutputScale How long the line trace is
	 * Line trace is a straight up and down line just used to find if there is a terrain that could be attached to this actor
	 */
	FHitResult LineTrace(FVector StartLocation, float OutputScale) const;

	/**
	 * Finds all the Connections that are available for the terrain passed in
	 * It will also establish the connection for the other terrain if it find that they are connected so that other terrain does not need to do this as well, we are only doing this once
	 * Uses the helper function line trace to complete this task
	 */
	void FindConnections(AProceduralTerrainGen* CurrentTerrain) const;

	/**The Foliage attached to the terrain*/
	UPROPERTY()
	TMap<UStaticMesh*, UHierarchicalInstancedStaticMeshComponent*> NatureStaticMeshHISM_Correspondence;

	/**
	 * Boolean controls whether or not we generate nature with our terrain
	 */
	bool bGenerateNature = false;

	//Saved off of all the Biome Foliage options for generating terrain
	TArray<FNatureInfo> BiomeFoliage;
	

	//All the Squares in the terrain mesh for the foliage object to spawn
	TArray<TArray<FNatureSquares>> NatureSquares;
	

	// Function pointers declaration
	typedef FQuat(AProceduralTerrainGen::* GetBiomeRotationFuncPtr)(const FRandomStream&, const FNatureSquares&, const FVector&) const;

	//The different functions used for the function pointers
	FQuat GetBiomeRandomRotation(const FRandomStream& RandomNumberGenerator, const FNatureSquares& Square, const FVector& Location) const;
	static FQuat GetRandomYawQuat(const FRandomStream& RandomNumberGenerator);
	FQuat GetBiomePlaneShapeRotation(const  FRandomStream& RandomNumberGenerator, const FNatureSquares& Square, const FVector& Location) const;
	FQuat GetBiomeMeshSurfaceRotation(const FRandomStream& RandomNumberGenerator, const FNatureSquares& Square, const FVector& Location) const;

	int NumberOfQuadsPerLine = 0;

    /**
    *Reference - https://www.redblobgames.com/grids/circle-drawing/
    */
	void ApplyMeshShade(const FNatureInfo& BiomeNature, int NatureSquareIndex);

	//Internal Function used to setup all the nature squares
	inline void InitNatureSquares()
	{
		NatureSquares = ProceduralTerrain->GetNatureSquares();
		NumberOfQuadsPerLine = ProceduralTerrain->NoiseSamplesPerLine-1;
	}

	inline FNatureSquares& GetNatureSquareAtIndex(const int Index)
	{
		const int OuterIndex = Index / NumberOfQuadsPerLine;
		const int InnerIndex = Index - (OuterIndex*NumberOfQuadsPerLine);
		return NatureSquares[OuterIndex][InnerIndex];
	}

	inline void CorrectedSquareIndex(const int Index, int& OuterIndex, int&InnerIndex) const
	{
		OuterIndex = Index / NumberOfQuadsPerLine;
		InnerIndex = Index - (OuterIndex*NumberOfQuadsPerLine);
	}
	

	void GetSquareCandidates(const FVector2D& HeightPercentageRangeToLocateElements,
		const int32 MaxCandidatesToGet,const ENatureBiomes Biome, const FRandomStream& RandomNumberGenerator,TArray<int32>& SquareCandidateIndexes,const bool bCanGrowInShade, const bool bOnlyGrowInShade);

};




