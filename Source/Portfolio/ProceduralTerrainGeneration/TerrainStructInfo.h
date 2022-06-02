#pragma once
#include "FastNoiseWrapper.h"
#include "Kismet/KismetMathLibrary.h"
#include "TerrainStructInfo.generated.h"

UENUM(BlueprintType)
enum class ENatureBiomes : uint8
{
	Earth				UMETA(DisplayName = "Earth surface"),
	Underwater			UMETA(DisplayName = "Underwater"),
	Both				UMETA(DisplayName = "Both")
};

UENUM(BlueprintType)
enum class ENatureRotationTypes : uint8
{
	Random				UMETA(DisplayName = "Random"),
	TerrainShapeNormal	UMETA(DisplayName = "Terrain Shape Normal"),
	MeshSurfaceNormal	UMETA(DisplayName = "Mesh Surface Normal")
};


USTRUCT(BlueprintType)
struct FNatureInfo
{
	GENERATED_USTRUCT_BODY()

	
public:

	/** Biome where the Nature meshes will be placed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	ENatureBiomes CorrespondingBiome;

	/** Distance (in centimeters) from camera at which each generated instance fades out. A value of 0 means infinite. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (ClampMin = 0, UIMin = 0))
	int32 CullDistance;

	/** Array of meshes used to randomly pick each time it adds a nature mesh in the biome. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	TArray<UStaticMesh*> Meshes;

	/** Minimum nature meshes that will be added on the corresponding biome. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	int32 MinMeshesToSpawn;

	/** Maximum nature meshes that will be added on the corresponding biome. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	int32 MaxMeshesToSpawn;

	/** Min. and max. range to randomly scale each added nature mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	FVector2D MinMaxScale;

	/**
	* Different types of rotation for the nature mesh:
	* - Random: random rotation, commonly used on stones, for example.
	* - Terrain Shape Normal: rotation that matches the terrain face normal, commonly used on trees, for example.
	* - Mesh Surface Normal: rotation that matches the normal of the nature mesh surface where it is placed, commonly used for grass, for example.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	ENatureRotationTypes RotationType;

	/** Seed used for all the random operations of this biome nature. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	int32 Seed;

	/**
	* Height percentage range where nature meshes will be placed.
	* For example, with these values:
	* - Lowest Landscape Height = 0.0
	* - Highest Landscape Height = 1000.0
	* - Height Percentage Range to Locate Meshes = (25.0, 75.0)
	* Nature meshes will be only placed in heights between 250.0 and 750.0.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "100.0", UIMax = "100.0"))
	FVector2D HeightPercentageRangeToLocateNatureMeshes;

	/** Height Offset for the Z axis for spawning in a foliage asset
	 * For example, if the value is positive then the foliage will be spawned higher in the world or if the value is negative the foliage will spawn lower in the world
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float SpawnHeightOffset;

	/**How much shade this mesh produces
	 * The World is broken up into a bunch of squares and if this shading distance is big enough to overlap in the area of a square then that how square will be set to be shaded
	 * and only meshes that have the Grow in Shade boolean will be allowed to grow in that square
	 * If the shade is smaller than what a square's size is then it will be ignored
	 * The size of the square is based off of the Noise Resolution
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties" , meta = (ClampMin = "0.0"))
	float ShadingDistance;

	/**Can the mesh grow in shade or not
	 * If true then the mesh will ignore if there is shade. Useful if this mesh is a rock and is needed to be place everywhere
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	bool bCanGrowInShade;

	/**
	 * The mesh will only be able to grow in shade if this is checked
	 * If this is true it is assumed that bCanGrowInShade is checked if not then the Mesh will not be allow to grow
	 * Ensure both are checked if it is desired for a mesh to grow
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	bool bOnlyGrowInShade;
	
	FNatureInfo()
	{
		CorrespondingBiome = ENatureBiomes::Earth;
		CullDistance = 150000;	
		MinMeshesToSpawn = 100;
		MaxMeshesToSpawn = 1000;
		MinMaxScale = FVector2D(0.5f, 3.0f);
		RotationType = ENatureRotationTypes::MeshSurfaceNormal;
		Seed = 1317;
		HeightPercentageRangeToLocateNatureMeshes = FVector2D(0.0f, 100.0f);
		SpawnHeightOffset = 0;
		ShadingDistance=0;
		bCanGrowInShade=false;
		bOnlyGrowInShade=false;
	}

	~FNatureInfo()
	{
		Meshes.Empty();
	}

};


USTRUCT(BlueprintType)
struct FTerrainInfo
{
	GENERATED_USTRUCT_BODY()

	FTerrainInfo()       
	{
		
	}

public:
	
	//Do we want to average our connections or not
	bool bAverageTerrainConnections=true;

	//The resolution the mesh is at not changed in the structure but in the manager because the resolutions need to match for the terrains
	int NoiseResolution = 300;

	//How many a specific terrain is. Not changed in the structure but in manager because the resolutions need to match for the terrains to work properly
	int TotalSizeToGenerate = 1200;

	//Not used currently, would be used to allow for terrains to index closer around the same location so that noise values do not change as often
	float NoiseInputScale = 0.5;

	/**
	 * Material used for the terrain
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	UMaterialInstance* TerrainMaterial=nullptr;

	/**How tall the highest point and smallest point will be from one another*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float NoiseOutputScale = 2000;

	/**
	 * Water Percentage changed is how much you want the water percentage to change for a given biome. Positive Numbers will add more water while negative numbers will subtract water.
	 * For Example if the Water Percentage is 5 and you want it to be 10 set this number to 5 and it will offset the terrain to have a water percentage of 10.
	 * So if we have a terrain where the lowest point is 0 and highest point is 1000 5 percent water would mean that the water height is 50
	 * Putting 5 here would offset all the terrain in the world by -50 to make up for the water percentage that is desired without moving the actual water percentage to make it so all the water lines up for generation
	 * @Warning making the offset change the water percentage below 0 will create very weird and unpredictable results same will offsetting it above 100
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings", meta = (ClampMin=-100,ClampMax=100))
	float WaterPercentageOffset = 0;

	//Internally Calculated so that once and saved off
	float WaterHeightOffset =0;
	
	/**How the noise is calculated the algorithm that is used*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::Simplex;

	/**Seed used for all noise types. Using different seeds will cause the noise output to change.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	int32 Seed = 1337;

	/**
	* Frequency for all noise types. Affects how coarse the noise output is.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float Frequency = 0.01f;

	/*
	 *Interpolation method used to smooth between noise values in Value and Perlin Noise. Possible interpolation methods (lowest to highest quality): Linear; Hermite; Quintic. Default value: Quintic
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_Interp Interp = EFastNoise_Interp::Quintic;

	/*
	 * Method for combining octaves in all fractal noise types.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_FractalType FractalType = EFastNoise_FractalType::FBM;

	/*
	* Octave count for all fractal noise types. The amount of noise layers used to create the fractal.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	int32 Octaves = 3;

	/*
	* Octave lacunarity for all fractal noise types. The frequency multiplier between each octave.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float Lacunarity = 2.0f;
	/*
	* Octave gain for all fractal noise types. The relative strength of noise from each layer when compared to the last.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float Gain = 0.5f;

	/*
	* Maximum distance a cellular point can move from its grid position. Setting this higher than 1 will cause artifacts.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float CellularJitter = 0.45f;

	/*
	* Distance function used in cellular noise calculations. The distance function used to calculate the cell for a given point.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_CellularDistanceFunction CellularDistanceFunction = EFastNoise_CellularDistanceFunction::Euclidean;

	/*
	* Return type from cellular noise calculations.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	EFastNoise_CellularReturnType CellularReturnType = EFastNoise_CellularReturnType::CellValue;

	/** Array of different meshes per biome */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	TArray<FNatureInfo> BiomeNatures;

	//Water Percentage set in manager but not accessible inside struct
	float WaterPercentage = 10;

	//Water height Used when creating water
	float WaterHeight = 0;

	/**
	 * Material used for the water
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	UMaterialInstance* WaterMaterial=nullptr;

	/**
	 * Decides whether or not the water has collision or not
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	bool bWaterHasCollision=false;
	
};


USTRUCT()
struct FNatureSquares
{
	GENERATED_BODY()
public:
	//Vertexes for Square
	FVector VertexA;
	FVector VertexB;
	FVector VertexC;
	FVector VertexD;
	bool bUsed = false;
	bool bShade = false;
	bool bUsedFirstTriangle=false;
	FVector CenterLocation;
	float DistanceFromCenterToEdge;

	explicit FNatureSquares(const FVector& vertexA = FVector::ZeroVector,const FVector& vertexB = FVector::ZeroVector,const FVector& vertexC = FVector::ZeroVector,const FVector& vertexD = FVector::ZeroVector)
	{
		VertexA = vertexA;
		VertexB = vertexB;
		VertexC = vertexC;
		VertexD = vertexD;
		CenterLocation =  -(VertexC-VertexA)/2 + VertexC;
		DistanceFromCenterToEdge =(VertexB-VertexA).Size()/2;
	}
	~FNatureSquares() {}

	/**
	* Tells whether all the vertexes are in the height range
	*
	* @param LowerHeight -	The lower height value.
	* @param HigherHeight -	The higher height value.
	* @return True if at all vertices of this triangle are within the given height range.
	*/
	inline bool IsInsidePlaneHeightRange(const float LowerHeight, const float HigherHeight) const
	{
		const bool bIsVertexA_InsideHeightRange = VertexA.Z > LowerHeight && VertexA.Z < HigherHeight;
		const bool bIsVertexB_InsideHeightRange = VertexB.Z > LowerHeight && VertexB.Z < HigherHeight;
		const bool bIsVertexC_InsideHeightRange = VertexC.Z > LowerHeight && VertexC.Z < HigherHeight;
		const bool bIsVertexD_InsideHeightRange = VertexD.Z > LowerHeight && VertexD.Z < HigherHeight;

		return bIsVertexA_InsideHeightRange && bIsVertexB_InsideHeightRange && bIsVertexC_InsideHeightRange && bIsVertexD_InsideHeightRange;
	}
	
	/**
	 * Get A random point on the square. First by picking which triangle we want to be using and the do the random calculation we did with the FNatureTriangle
	 * References:
	 * - https://adamswaab.wordpress.com/2009/12/11/random-point-in-a-triangle-barycentric-coordinates/
	 * - https://math.stackexchange.com/questions/538458/triangle-point-picking-in-3d
	 * @returns A boolean if we were successful or not
	 * @param RandomNumberGenerator the FRandomStream that all random calculation are created off of
	 * @param OutLocation the result of the calculation
	 */
	inline bool GetRandomPointOnSquare(const FRandomStream& RandomNumberGenerator, FVector& OutLocation)
	{
		if(bUsed)
		{
			return false;
		}
		else
		{
			const int RandomBool =RandomNumberGenerator.RandRange(0,1);
			if(RandomBool == 0)
			{
				float RandomA = RandomNumberGenerator.GetFraction();
				float RandomB = RandomNumberGenerator.GetFraction();

				if (RandomA + RandomB >= 1.0f)
				{
					RandomA = 1.0f - RandomA;
					RandomB = 1.0f - RandomB;
				}
				OutLocation= VertexA + RandomA * (VertexB - VertexA) + RandomB * (VertexC - VertexA);
				bUsedFirstTriangle=true;
				return true;
			}
			else
			{
				float RandomA = RandomNumberGenerator.GetFraction();
				float RandomB = RandomNumberGenerator.GetFraction();

				if (RandomA + RandomB >= 1.0f)
				{
					RandomA = 1.0f - RandomA;
					RandomB = 1.0f - RandomB;
				}
				OutLocation= VertexD + RandomA * (VertexB - VertexD) + RandomB * (VertexC - VertexD);
				bUsedFirstTriangle=false;
				return true;
			
			}
		}
		
	}

	/**
	* Gets the triangle surface normal quaternion. Uses the interal boolean bUsedFirstTriangle to know which triangle to get the normal of
	* References:
	* - https://math.stackexchange.com/questions/305642/how-to-find-surface-normal-of-a-triangle
	* - https://math.stackexchange.com/questions/538458/triangle-point-picking-in-3d
	*
	* @return The normal quaternion of the triangle surface
	*/
	inline FQuat GetSurfaceNormal() const
	{
		if(bUsedFirstTriangle)
		{
			const FVector v = VertexB - VertexA;
			const FVector w = VertexC - VertexA;
			const FRotator RotatorCorrection = FRotator(90.0f, 0.0f, 0.0f);

			return (FVector((v.Y * w.Z) - (v.Z * w.Y), (v.Z * w.X) - (v.X * w.Z), (v.X * w.Y) - (v.Y * w.X)).Rotation() + RotatorCorrection).Quaternion();
		}
		else
		{
			const FVector v = VertexB - VertexD;
			const FVector w = VertexC - VertexD;
			const FRotator RotatorCorrection = FRotator(90.0f, 0.0f, 0.0f);

			return (FVector((v.Y * w.Z) - (v.Z * w.Y), (v.Z * w.X) - (v.X * w.Z), (v.X * w.Y) - (v.Y * w.X)).Rotation() + RotatorCorrection).Quaternion();
		}
	}
};

USTRUCT(BlueprintType)
struct FBiomeInfo
{
	GENERATED_USTRUCT_BODY()

	FBiomeInfo()        
	{
		
	}

public:
	/**How the noise is calculated the algorithm that is used*/
	EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::Simplex;

	//Choose a seed for the biomes to be created off of
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	int32 Seed = 1337;

	//All of these parameters are hidden. Only can choose the seed to get a different lineup of biomes
	float Frequency = 0.01f;

	EFastNoise_Interp Interp = EFastNoise_Interp::Quintic;

	EFastNoise_FractalType FractalType = EFastNoise_FractalType::FBM;

	int32 Octaves = 3;

	float Lacunarity = 2.0f;

	float Gain = 0.5f;

	float CellularJitter = 0.45f;

	EFastNoise_CellularDistanceFunction CellularDistanceFunction = EFastNoise_CellularDistanceFunction::Euclidean;

	EFastNoise_CellularReturnType CellularReturnType = EFastNoise_CellularReturnType::CellValue;
	
};