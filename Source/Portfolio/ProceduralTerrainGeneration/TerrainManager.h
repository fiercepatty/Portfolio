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

	/**Start the Map up and generate The required planes for it to run based on Visible Range*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrain Generation")
	void GeneratePlane();

	/**Remove all planes spawned in by the manager*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrain Generation")
	void RemoveAllPlanes();
	
	/**Move you square north by one square*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrain Movement")
	void MoveSquareNorth();

	/**Move you square East by one square*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrain Movement")
	void MoveSquareEast();

	/**Move you square South by one square*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrain Movement")
	void MoveSquareSouth();

	/**Move you square West by one square*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrain Movement")
	void MoveSquareWest();
	
	//Getter for the origin
	inline AProceduralTerrainGen* GetOrigin() const {return Origin;}

private:
	//Helper Function to get the Terrain Options of the neighboring Components even if they haven't been created yet
	TArray<FTerrainInfo> FindConnectedTerrains(const AProceduralTerrainGen* CurrentTerrain);
	
	/**Helper Function to spawn in the actual actor at a given location with the correct offset from the direction*/
	AProceduralTerrainGen* SpawnTerrain(FVector Location,FVector SampleStart,EDirection Dir);

	/**Helper Function to spawn in the actual actor at a given location with the correct offset from the direction*/
	AProceduralTerrainGen* SpawnTerrain(FVector Location);

	//Helper function to get the starting square spawned in
	void GenerateSquareLayer();

	//Helper function to get the starting square spawned in
	void GenerateSquareCorners();
	
	/**Need at least one of these to generate terrain
	 * If you have more than one when it is creating terrain it will pick one of these values randomly when creating the terrain
	 * On the edge of two terrain locations it will use the TerrainConnectionOptions values which will allow for terrain to be connected to one another
	 * When pick the noise value from the options it has a higher chance of keeping the noise value of the neighbor that told it to create itself
	 * So if its south neighbor is what told it to create itself there will be a higher chance to use that neighbors noise options
	 * Percent Chance will be controlled by ChanceToContinueTerrain- Between 0 and 1, 0 meaning randomly choose another terrain option and 1 being always take neighbors terrain option
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Noise" ,meta = (AllowPrivateAccess=true))
	TArray<FTerrainInfo> AllTerrainOptions;

	/**
	 * Used to tell the Terrain picker what biome to actually pick.
	 * We use the location for the NoiseComponentLocation that gets updated with the terrains to do the noise calculation this allows us to recreate the same map over and over again
	 * You are able to change the seed which will make it so different biomes will show up at different times
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Biome",meta = (AllowPrivateAccess=true))
	FBiomeInfo BiomeOptions;

	/**
	 * The edges of a biome do we average our connections or do we pick whichever one is closest to the origin
	 * Averaging will allow us to look at the connected corners and average whatever that connected terrain into our terrain so that the edges match up
	 * Picking which one is closest will allow the terrain to look at whichever terrain has a closer NoiseSampling Vector to zero and that one will win out
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Biome",meta = (AllowPrivateAccess=true))
	bool bAverageConnection = true;

	//The Wrapper that holds all the fast noise info
	UPROPERTY()
	UFastNoiseWrapper* BiomeNoise;

	//Init the Fast Noise Wrapper so we can do all of our calculations and they will be consistent and repeatable
	void InitBiomeNoise() const;

	/**
	 * @returns the Index in the AllTerrainOptions that the selected Terrain will have
	 * @param BiomeLocation This is just hte NoiseComponentLocation that will allow it to easily keep track of the individual terrain without making a new variable
	 * If there is only one terrain in All Terrain Option it just return index of 0 else it will do the noise calculation then clamp its result between 0 and 1 and
	 * then it will give each terrain option a interval that that terrain will be selected so if we have 2 terrain options 0-0.49 will be option 1 and .5 to 1 will be option 2
	 */
	int SelectedBiome(FVector BiomeLocation) const;

	/**
	 *This will control how high the water level is If you want no water set water height to -1
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Noise",meta = (AllowPrivateAccess=true))
	FWaterInfo WaterOptions;

	/**How large the terrain piece are individually*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Settings",meta = (ClampMin=1,AllowPrivateAccess=true))
	float TotalSizeToGenerate=1200;

	/**Divide this by the total size to generate and that will give you how many vertices going across the square there will be. So basically divide this by the total size to generate and the square it and that
	 *is how many verts are in your mesh. But remember there are multiple of these meshes on the map at a time so doing go crazy*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Settings",meta = (ClampMin=1,AllowPrivateAccess=true))
	float NoiseResolution=300;

	/**How many cubes away from the active cube that the player can see
	 *If One then the map would look like this, (0 represent invisible and 1 represent Visible)
	 * 00000			11111
	 * 01110			11111
	 * 01110			11111		If we had 2 
	 * 01110			11111
	 * 00000			11111
	 * Basically How many cubes it would take to get from the center of the square to the edge of the square
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Settings" ,meta = (ClampMin=1, AllowPrivateAccess=true))
	int VisibleRange = 1;

	//Have the manager create the terrain at runtime because it needs to be setup in a certain way so if the user has changed things inside the individual pieces it will cause issues so we dont give them the chance
	virtual void BeginPlay() override;
	
	//Helper function for when the map is actually spawned in
	FVector UpdateNoiseSamplingLocation(FVector StartLocation, EDirection Dir) const;
	
	/**Array of all Procedural Terrain Gen on the North end of the square this is the perimeter of the square*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Square Arrays", meta = (AllowPrivateAccess = "true"))
	TArray<AProceduralTerrainGen*> North;

	/**Array of all Procedural Terrain Gen on the East end of the square this is the perimeter of the square*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Square Arrays", meta = (AllowPrivateAccess = "true"))
	TArray<AProceduralTerrainGen*> East;

	/**Array of all Procedural Terrain Gen on the South end of the square this is the perimeter of the square*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Square Arrays", meta = (AllowPrivateAccess = "true"))
	TArray<AProceduralTerrainGen*> South;

	/**Array of all Procedural Terrain Gen on the West end of the square this is the perimeter of the square*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Square Arrays", meta = (AllowPrivateAccess = "true"))
	TArray<AProceduralTerrainGen*> West;

	/**This is the starting location of the Map*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Square Arrays", meta = (AllowPrivateAccess = "true"))
	AProceduralTerrainGen* Origin;
	
};

