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

	/** Enable this option to get the inverted result (nature meshes will be located only out of height range). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	bool bUseLocationsOutsideHeightRange;
	
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
		bUseLocationsOutsideHeightRange = false;
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
};


USTRUCT(BlueprintType)
struct FWaterInfo
{
	GENERATED_USTRUCT_BODY()

	FWaterInfo()        
	{
		
	}

public:
	/**The waters resolution so the larger this number is the less triangles it will be to create the water mesh but make sure this number is divisible by the TotalSizeToGenerate in the Manager*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	int NoiseResolution = 300;

	//Saved off for how large the terrain is
	int TotalSizeToGenerate = 1200;
	
	/**At this Height Water will be generated at. So everything below this would be underwater. Keep this value lower than the NoiseOutputHeight so that there is land*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	float WaterHeight = 300;

	/**How tall the highest point and smallest point will be from one another*/
	float NoiseOutputScale = 2000;

	/**
	 * Material used for the water
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrian Generation | Noise Settings")
	UMaterialInstance* WaterMaterial=nullptr;


	
	
};



USTRUCT()
struct FNatureTriangle
{
	GENERATED_BODY()

public:

	//The vertexes that create the triangle
	FVector VertexA;
	FVector VertexB;
	FVector VertexC;
	bool bInUse=false;
	
	explicit FNatureTriangle(const FVector& vertexA = FVector::ZeroVector,const FVector& vertexB = FVector::ZeroVector,const FVector& vertexC = FVector::ZeroVector)
	{
		VertexA = vertexA;
		VertexB = vertexB;
		VertexC = vertexC;
	}

	~FNatureTriangle() {}

	/**
	* Tells whether at least 2 vertices of this triangle are within the given height range for plane terrain shapes.
	*
	* @param LowerHeight -	The lower height value.
	* @param HigherHeight -	The higher height value.
	* @return True if at least 2 vertices of this triangle are within the given height range.
	*/
	inline bool IsInsidePlaneHeightRange(const float LowerHeight, const float HigherHeight) const
	{
		const bool bIsVertexA_InsideHeightRange = VertexA.Z > LowerHeight && VertexA.Z < HigherHeight;
		const bool bIsVertexB_InsideHeightRange = VertexB.Z > LowerHeight && VertexB.Z < HigherHeight;
		const bool bIsVertexC_InsideHeightRange = VertexC.Z > LowerHeight && VertexC.Z < HigherHeight;

		return (bIsVertexA_InsideHeightRange && bIsVertexB_InsideHeightRange)
			|| (bIsVertexA_InsideHeightRange && bIsVertexC_InsideHeightRange)
			|| (bIsVertexB_InsideHeightRange && bIsVertexC_InsideHeightRange);
	}

	inline bool IsInUse()const
	{
		return bInUse;
	}

	inline void UsedTriangle()
	{
		bInUse=true;
	}

	inline FVector GetMiddlePointAB() const
	{
		return VertexA - ((VertexA - VertexB) * 0.5);
	}

	inline FVector GetMiddlePointBC() const
	{
		return VertexB - ((VertexB - VertexC) * 0.5);
	}

	inline FVector GetMiddlePointAC() const
	{
		return VertexA - ((VertexA - VertexC) * 0.5);
	}

	/**
	* Gets a random point on the triangle surface.
	* References:
	* - https://adamswaab.wordpress.com/2009/12/11/random-point-in-a-triangle-barycentric-coordinates/
	* - https://math.stackexchange.com/questions/538458/triangle-point-picking-in-3d
	*
	* @param RandomNumberGenerator -	Random generator stream.
	* @return A random point in the triangle.
	*/
	inline FVector GetRandomPointOnTriangle(const FRandomStream& RandomNumberGenerator) const
	{
		if(bInUse)
		{
			return FVector(0);
		}
		float RandomA = RandomNumberGenerator.GetFraction();
		float RandomB = RandomNumberGenerator.GetFraction();

		if (RandomA + RandomB >= 1.0f)
		{
			RandomA = 1.0f - RandomA;
			RandomB = 1.0f - RandomB;
		}
		return VertexA + RandomA * (VertexB - VertexA) + RandomB * (VertexC - VertexA);
	}

	/**
	* Gets the triangle surface normal quaternion.
	* References:
	* - https://math.stackexchange.com/questions/305642/how-to-find-surface-normal-of-a-triangle
	* - https://math.stackexchange.com/questions/538458/triangle-point-picking-in-3d
	*
	* @return The normal quaternion of the triangle surface
	*/
	inline FQuat GetSurfaceNormal() const
	{
		const FVector v = VertexB - VertexA;
		const FVector w = VertexC - VertexA;
		const FRotator rotatorCorrection = FRotator(90.0f, 0.0f, 0.0f);

		return (FVector((v.Y * w.Z) - (v.Z * w.Y), (v.Z * w.X) - (v.X * w.Z), (v.X * w.Y) - (v.Y * w.X)).Rotation() + rotatorCorrection).Quaternion();
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