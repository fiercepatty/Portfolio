// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralTerrainGen.h"
#include "GameFramework/Actor.h"
#include "TerrainStructInfo.h"
#include "TerrainManager.generated.h"
UENUM(BlueprintType)
enum class EDirection: uint8
{
	VE_North UMETA(DisplayName="North"),
	
	VE_South	UMETA(DisplayName = "South"),
	
	VE_East		UMETA(DisplayName = "East"),
	
	VE_West	UMETA(DisplayName = "West")

};
UCLASS()
class PORTFOLIO_API ATerrainManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrainManager();

	/**Need at least one of these to generate terrain
	 * If you have more than one when it is creating terrain it will pick one of these values randomly when creating the terrain
	 * On the edge of two terrain locations it take the noise value of the terrain that was already there. It has a higher chance
	 * Of keeping the noise value of the neighbor that told it to create itself
	 * So if its south neighbor is what told it to create itself there will be a higher chance to use that neighbors noise options
	 * Percent Chance will be controlled by ChanceToContinueTerrain- Between 0 and 1, 0 meaning randomly choose another terrain option and 1 being always take neighbors terrain option
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Noise")
	TArray<FTerrainInfo> AllTerrainOptions;

	/**
	 * Percent Chance to pick a random option out of the AllTerrainOptions for the Noise Value
	 * Clamped Between 0 and 1
	 * Where 0 is never pick another option and 1 being always try for another
	 * Just because it is told to try it could still randomly choose the same terrain as its neighbor
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Noise", meta=(ClampMin =0,ClampMax =1))
	float ChanceToContinueTerrain =.8;

	/**How large the terrain piece are individually*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Noise")
	float TotalSizeToGenerate=1200;

	/**Change this to get a different effect from the same parameters*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Noise")
	float Seed = 1317;

	/**Divide this by the total size to generate and that will give you how many vertices going across the square there will be. So basically divide this by the total size to generate and the square it and that
	 *is how many verts are in your mesh. But remember there are multiple of these meshes on the map at a time so doing go crazy*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Noise")
	float NoiseResolution=300;

	/**Start the Map up and generate The required planes for it to run based on Visible Range*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void GeneratePlane();

	/**Remove all planes spawned in by the manager*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void RemoveAllPlanes();

	/**Helper Function to spawn in the actual actor at a given location with the correct offset from the direction*/
	AProceduralTerrainGen* SpawnTerrain(FVector Location,FVector SampleStart,EDirection Dir);

	/**Helper Function to spawn in the actual actor at a given location with the correct offset from the direction*/
	AProceduralTerrainGen* SpawnTerrain(FVector Location);

	//Helper function to get the starting square spawned in
	void GenerateSquareLayer();

	//Helper function to get the starting square spawned in
	void GenerateSquareCorners();

	/**Move you square north by one square*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void MoveSquareNorth();

	/**Move you square East by one square*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void MoveSquareEast();

	/**Move you square South by one square*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void MoveSquareSouth();

	/**Move you square West by one square*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrian Generation")
	void MoveSquareWest();

	/**How many cubes away from the active cube that the player can see
	 *If One then the map would look like this, (0 represent invisible and 1 represent Visible)
	 * 00000			11111
	 * 01110			11111
	 * 01110			11111		If we had 2 
	 * 01110			11111
	 * 00000			11111
	 * Basically How many cubes it would take to get from the center of the square to the edge of the square
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Settings" ,meta = (ClampMin=1))
	int VisibleRange = 1;

private:

	//Helper function for when the map is actually spawned in
	FVector UpdateNoiseSamplingLocation(FVector StartLocation, EDirection Dir) const;
	
	/**Array of all Procedural Terrain Gen on the North end of the square this is the perimeter of the square*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Settings", meta = (AllowPrivateAccess = "true"))
	TArray<AProceduralTerrainGen*> North;

	/**Array of all Procedural Terrain Gen on the East end of the square this is the perimeter of the square*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Settings", meta = (AllowPrivateAccess = "true"))
	TArray<AProceduralTerrainGen*> East;

	/**Array of all Procedural Terrain Gen on the South end of the square this is the perimeter of the square*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Settings", meta = (AllowPrivateAccess = "true"))
	TArray<AProceduralTerrainGen*> South;

	/**Array of all Procedural Terrain Gen on the West end of the square this is the perimeter of the square*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Settings", meta = (AllowPrivateAccess = "true"))
	TArray<AProceduralTerrainGen*> West;

	/**This is the starting location of the Map*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Settings", meta = (AllowPrivateAccess = "true"))
	AProceduralTerrainGen* Origin;
	
};
