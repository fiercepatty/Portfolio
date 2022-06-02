// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavModifierVolume.h"
#include "ProceduralTerrainGen.h"
#include "GameFramework/Actor.h"
#include "TerrainStructInfo.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "TerrainManager.generated.h"
/**Direction needed to be moved to get to another terrain location*/
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

	/**Volume to prevent the player from being able to walk on water needs reference to a blocking modifier to work*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Volume" ,meta = (AllowPrivateAccess=true))
	ANavModifierVolume* BlockingWaterLevelVolume;

	/**How much the block water will be offset from the water level. Lower if desired for more water access or higher for less access*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Volume" ,meta = (AllowPrivateAccess=true))
	float BlockingWaterOffset = 0;

	/**How tall the Block Water Volume will be. The Z axis*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Volume" ,meta = (AllowPrivateAccess=true))
	float BlockingWaterSize =2;
	/**
	 * Volume to make the world traversable by AI
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Volume" ,meta = (AllowPrivateAccess=true))
	ANavMeshBoundsVolume* NavMeshBoundsVolume;
	
	/**Need at least one of these to generate terrain
	 * If you have more than one when it is creating terrain it will pick one of these values randomly when creating the terrain
	 * This is all the parameters for the Noise for generating the map.
	 * If there are more than one of these it will average the noise value of connected terrain to generate the map
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Noise" ,meta = (AllowPrivateAccess=true))
	TArray<FTerrainInfo> AllTerrainOptions;

	/**Percentage of the world that will be water.
	 *For example if the noise output scale for terrain is 500 then the highest point in the world is 500 and lowest is -500
	 *A Water height you would want to used for this would be -500, 500 or you could even put it so that no matter what you set the water percentage to, 400 to 500 will always be above water if you put the Water Height Range  to -500, 400 this effect will happen
	 *For this we will use 0 and 1000. If the world is 10 percent water than we divide 1000 by percentage and that will be the height of the water
	 *So for 10 percent it will be 100. If we wanted 50 percent it would be 500 and so on and so forth. Number must be a between 0 and 100. If 0 then there will be no water
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Water", meta = (ClampMin = 0, ClampMax=100, AllowPrivateAccess=true))
	float WaterPercentage = 10;

	/**The Height Water Height and lowest water height achievable
	 * Used when calculating the Water Percentage.
	 * @warning if these water ranges are nowhere near the terrain noise output scale then this water will become independent of the noise output scale so keep these parameter between the noise output scale.
	 * Lower Number should be Greater than negative output scale and Larger Number should be less than Noise Output scale
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Water", meta = (AllowPrivateAccess=true))
	FVector2D WaterHeightRange =FVector2D(-2000,2000);

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
	 * NOT IMPLEMENTED Picking which one is closest will allow the terrain to look at whichever terrain has a closer NoiseSampling Vector to zero and that one will win out
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Biome",meta = (AllowPrivateAccess=true))
	bool bAverageConnection = true;

	//The Wrapper that holds all the fast noise info for the biome, Recreate the class or reload unreal if this has a nullptr in it
	UPROPERTY(VisibleAnywhere, Category = "Terrain Settings | Noise")
	UFastNoiseWrapper* BiomeNoise;

	//Init the Fast Noise Wrapper so we can do all of our calculations and they will be consistent and repeatable
	void InitBiomeNoise() const;

	/**
	 * @returns the Index in the AllTerrainOptions that the selected Terrain will have
	 * @param BiomeLocation This is just the NoiseComponentLocation that will allow it to easily keep track of the individual terrain without making a new variable
	 * If there is only one terrain in All Terrain Option it just return index of 0 else it will do the noise calculation then clamp its result between 0 and 1 and
	 * then it will give each terrain option a interval that that terrain will be selected so if we have 2 terrain options 0-0.49 will be option 1 and .5 to 1 will be option 2
	 */
	int SelectedBiome(FVector BiomeLocation) const;

	/**How large the terrain piece are individually*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Settings",meta = (ClampMin=1,AllowPrivateAccess=true))
	float TotalSizeToGenerate=1200;

	/**Divide this by the total size to generate and that will give you how many vertices going across the square there will be. So basically divide this by the total size to generate and multiple that by itself and that
	 *is how many verts are in your mesh. But remember there are multiple of these meshes on the map at a time so don't go crazy*/
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

	/**How large the terrain piece are individually*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Spawning",meta = (ClampMin=1,AllowPrivateAccess=true))
	FVector SampleNoiseStartLocation= FVector(0);

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

	/**Whether or not to destroy the components when they are unloaded or to just make the invisible*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Settings | Square Arrays", meta = (AllowPrivateAccess = "true"))
	bool bDestroyOnHide = false;
	
	//Internal Sized variable for the height of the water
	float WaterHeight =0;
};

