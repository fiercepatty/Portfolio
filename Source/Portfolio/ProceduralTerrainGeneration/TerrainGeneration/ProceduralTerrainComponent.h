// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "FastNoiseWrapper.h"
#include "TerrainStructInfo.h"
#include "ProceduralTerrainComponent.generated.h"


struct FTerrainInfo;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_API UProceduralTerrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralTerrainComponent();
	
	//Init the Fast Noise Or the noise values used for our terrain are init here
	void InitializeFastNoise(FTerrainInfo Terrain);

	//Init all the connection noise so that we can use them to make sure borders are flush up with one another
	void InitializeConnectionNoise(TArray<FTerrainInfo> Terrains) ;

	/**
	 * @param StartingLocation - This is where we will be indexing in the noise map to keep consistent with even map created
	 * Function used to created the Procedural Map Calls multiple helper functions
	 */
	UFUNCTION(BlueprintCallable)
	void GenerateMap(FVector StartingLocation);

	/**
	 * Used to get a noise value from a position using the terrain wrapper. Used to make it so that our foliage has repeatable seeds but does not allow the foliage to repeat itself over and over again on a terrain
	 */
	inline float GetValueFromNoise(const FVector Position) const {return TerrainNoise->GetNoise3D(Position.X,Position.Y,Position.Z);}

	/**
	 * Used to unload the mesh and any collisions that are unnecessary when we can not see the map
	 */
	void UnLoadMesh() const;

	/**
	 * Used to load the mesh and collision back when we need to see the map
	 */
	void LoadMesh() const;

	/**This is the starting location saved off that is passed in Generate Map*/
	FVector NoiseComponentStartLocation;

	//This is the resolution the terrain is at so basically how many triangle used to create the terrain. Divide this number by total size to generate to get the amount of triangle that the terrain has. In one line
	//(Total Size / NoiseResolution ) **2 will give you how many triangles
	int NoiseResolution = 300;

	//How large the actual terrain is
	int TotalSizeToGenerate = 1200;

	//Used to make it so that the noise values for a particular location are index closer together to allow for landscapes to have less variation with the smaller the number is
	//Currently not used in the calculation
	float NoiseInputScale = 0.5;

	//How large the output the Terrain will be. This will be doubled so there will be 4000 units used its just half will be in negative space
	float FastNoiseOutputScale = 2000;
	
	//Saved off to know how many triangles there are per line of the square and how many lines there are for the square
	//This is subtracted by one everywhere to get the correct amount
	int NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	
	/**The Wrapper that saves off the Noise that the terrain is using*/
	UPROPERTY(VisibleAnywhere, Category = "Terrian Generation | Noise Settings")
	UFastNoiseWrapper* TerrainNoise;

	/**All of the connections with this terrain so all the places that its edges touch corners count as well*/
	UPROPERTY()
	TArray<UFastNoiseWrapper*> ConnectionNoises;

	//How many vertices there are in the terrain
	int VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;

	/**This is the actual mesh that is created when generating meshes*/
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralTerrainMesh;

	//Function used to create a static mesh based off of the actual procedural mesh. Made to test the performance between using static meshes and using an actual procedural mesh
	//Found out the static meshes did not create much of a benefit how ever the meshes did make me save off all the static meshes that were created without a real way of destroying them after runtime
	UStaticMesh* CreateStaticMesh() const;

	//Getter for the Vertices of he terrain;
	//inline TArray<FNatureTriangle> GetNatureTriangles()
	//{
	//	return NatureTriangles;
	//}

	//Getter for the Nature Squares for the foliage calculation
	inline TArray<TArray<FNatureSquares>> GetNatureSquares() const
	{
		return NatureSquares;
	}


private:
	//Should we average between the edges of two terrains or not
	bool bAverage = true;

	//This is where the Noise starts indexing in the Noise Texture. This is the starting point to index in our texture. 
	FVector ComponentLocation;

	//All the vertices of the Terrain
	TArray<FVector> Vertices;

	//All the triangles of the terrain
	TArray<int> Triangles;
	

	//Array of all the nature squares in the terrain seperated in rows
	TArray<TArray<FNatureSquares>> NatureSquares;

	//Internal function used create the mesh description needed to create a static mesh. Used the procedural mesh component function for converting a procedural mesh to static mesh but removed the prompt to name the mesh
	static FMeshDescription BuildANewMeshDescription(UProceduralMeshComponent* ProcMeshComp );


	UMaterialInstance* Mat=nullptr;

	//Internal Function used to generate all the vertices of the Terrain and the UVs for the mesh
	void GenerateVertices();
	
	//Internal function for generating all the triangles giving the procedural mesh a way of knowing what vertices indexes are in reference to what triangles
	void GenerateTriangles();

	//Internal function that just passes in the parameter needed to generate the mesh for the procedural mesh
	void GenerateMesh() const;

	//Internal Function that is used to tell what noise value is represented at every location.
	//So how high the z axis the terrain should be. On the edges of the terrain it averages the values of the touching terrain with itself to make smooth transitions
	float GetNoiseValueForGridCoordinates(int X, int Y) const;

	//Used to get the index for a specific coordinate in the array of vertices
	int GetIndexForGridCoordinates(int X, int Y) const;

	//Get the location of where the coordinate would be in reference to the actual mesh
	FVector2D GetPositionForGridCoordinates(int X, int Y) const;

	// Other things needed to generate the mesh
	//Array of all the Normals for the Vertices 
	TArray<FVector> Normals;

	//Array of all the tangents for the vertices
	TArray<FProcMeshTangent> Tangents;

	//Array of all the UV for the terrain
	TArray<FVector2D> UV;

	//Colors for the actual vertexes
	TArray<FColor> VertexColors;
	
};
