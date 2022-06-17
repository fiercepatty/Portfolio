// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppTooWideScope
#include "ProceduralTerrainComponent.h"

#include "MeshDescription.h"
#include "TerrainStructInfo.h"
#include "ProceduralMeshComponent.h"


// Sets default values for this component's properties
UProceduralTerrainComponent::UProceduralTerrainComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ReSharper disable once CppJoinDeclarationAndAssignment
	
	
	ProceduralTerrainMesh->CreateMeshSection(0,Vertices,Triangles,Normals,UV,VertexColors,Tangents,true);

	//Init the terrain for this specific terrain mesh
	TerrainNoise =CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("FastNoiseWrapper"));

	//Init all the noises for the connected terrains
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("NorthNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("NorthEastNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("EastNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("SouthEastNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("SouthNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("SouthWestNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("WestNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("NorthWestNoise")));

	ConnectionNoiseOutputSizes.Init(0.0f,8);
	ConnectionNoiseWaterHeightOffset.Init(0.0f,8);
	// ...
}

void UProceduralTerrainComponent::InitializeFastNoise(const FTerrainInfo Terrain)
{
	//init out fast noise
	TerrainNoise->SetupFastNoise(Terrain.NoiseType, Terrain.Seed, Terrain.Frequency, Terrain.Interp, Terrain.FractalType,
				Terrain.Octaves,Terrain.Lacunarity,Terrain.Gain,Terrain.CellularJitter, Terrain.CellularDistanceFunction, Terrain.CellularReturnType);

	//Save off the variables we need for calculations so we dont have to use getter in the fast noise to get access to these variables
	NoiseResolution=Terrain.NoiseResolution;
	TotalSizeToGenerate=Terrain.TotalSizeToGenerate;
	NoiseInputScale=Terrain.NoiseInputScale;
	FastNoiseOutputScale=Terrain.NoiseOutputScale;
	Mat=Terrain.TerrainMaterial;
	bAverage=Terrain.bAverageTerrainConnections;
	WaterHeightOffset = Terrain.WaterHeightOffset;
	
}

void UProceduralTerrainComponent::InitializeConnectionNoise(TArray<FTerrainInfo> Terrains) 
{
	//Init the connected terrain noises
	for(int Index=0;Index < ConnectionNoises.Num();Index++)
	{
		ConnectionNoises[Index]->SetupFastNoise(Terrains[Index].NoiseType, Terrains[Index].Seed, Terrains[Index].Frequency, Terrains[Index].Interp, Terrains[Index].FractalType,
				Terrains[Index].Octaves,Terrains[Index].Lacunarity,Terrains[Index].Gain,Terrains[Index].CellularJitter, Terrains[Index].CellularDistanceFunction, Terrains[Index].CellularReturnType);
		ConnectionNoiseOutputSizes[Index] = Terrains[Index].NoiseOutputScale;
		ConnectionNoiseWaterHeightOffset[Index]= Terrains[Index].WaterHeightOffset;
	}
}

void UProceduralTerrainComponent::GenerateMap(const FVector StartLocation)
{
	//Start location for our map to generate around
	ComponentLocation= StartLocation;
	

	//How many points are in each line and how many lines there are to make the square
	NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;

	//How many vertices there are 
	VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;

	//Creating all the normals for the terrain
	Normals.Init(FVector(0, 0, 1), VerticesArraySize);
	//Tangents.Init(FRuntimeMeshTangent(0, -1, 0), VerticesArraySize);
	UV.Init(FVector2D(0, 0), VerticesArraySize);
	VertexColors.Init(FColor::White, VerticesArraySize);
	
	GenerateVertices();
	GenerateTriangles();
	GenerateMesh();
}

void UProceduralTerrainComponent::UnLoadMesh() const
{
	ProceduralTerrainMesh->SetVisibility(false);
	ProceduralTerrainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UProceduralTerrainComponent::LoadMesh() const
{
	ProceduralTerrainMesh->SetVisibility(true);
	ProceduralTerrainMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

float UProceduralTerrainComponent::EvaluateConnectionNoiseAtIndex(const int Index, const int X, const int Y) const
{
	return ConnectionNoises[Index]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0) * ConnectionNoiseOutputSizes[Index] + ConnectionNoiseWaterHeightOffset[Index];
}

void UProceduralTerrainComponent::GenerateVertices()
{
	Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
	//Loop through each vertices and set the position the vertices is located at scaled to the actual mesh
	for (int y = 0; y < NoiseSamplesPerLine; y++) {
		for (int x = 0; x < NoiseSamplesPerLine; x++) {
			
			const float NoiseResult = GetNoiseValueForGridCoordinates(x, y);
			const int Index = GetIndexForGridCoordinates(x, y);
			const FVector2D Position = GetPositionForGridCoordinates(x, y);
			Vertices[Index] = FVector(Position.X, Position.Y, NoiseResult);
			UV[Index] = FVector2D(x, y);
		}
	}
}

void UProceduralTerrainComponent::GenerateTriangles()
{
	// This is the number of triangle indexes making up a quad (square section of the grid)
	constexpr int QuadSize = 6;
	// We have one less quad per line than the amount of vertices, since each vertex is the start of a quad except the last ones
	const int NumberOfQuadsPerLine = NoiseSamplesPerLine - 1;
	// In our triangles array, we need 6 values per quad
	const int TrianglesArraySize = NumberOfQuadsPerLine * NumberOfQuadsPerLine * QuadSize;
	Triangles.Init(0, TrianglesArraySize);
	//Init the amount of Squares the Mesh has
	NatureSquares.Init(TArray<FNatureSquares>(),NumberOfQuadsPerLine);

	for (int y = 0; y < NumberOfQuadsPerLine; y++) {
		for (int x = 0; x < NumberOfQuadsPerLine; x++) {
			const int QuadIndex = x + y * NumberOfQuadsPerLine;
			const int TriangleIndex = QuadIndex * QuadSize;

			// Getting the indexes of the four vertices making up this quad
			const int BottomLeftIndex = GetIndexForGridCoordinates(x, y);
			const int TopLeftIndex = GetIndexForGridCoordinates(x, y + 1);
			const int TopRightIndex = GetIndexForGridCoordinates(x + 1, y + 1);
			const int BottomRightIndex = GetIndexForGridCoordinates(x + 1, y);

			// Assigning the 6 triangle points to the corresponding vertex indexes, by going counter-clockwise.
			Triangles[TriangleIndex] = BottomLeftIndex;
			Triangles[TriangleIndex + 1] = TopLeftIndex;
			Triangles[TriangleIndex + 2] = TopRightIndex;

			Triangles[TriangleIndex + 3] = BottomLeftIndex;
			Triangles[TriangleIndex + 4] = TopRightIndex;
			Triangles[TriangleIndex + 5] = BottomRightIndex;

			//Creating the Nature Squares for Foliage Generation
			NatureSquares[y].Add(FNatureSquares(Vertices[BottomLeftIndex],Vertices[TopLeftIndex],
				Vertices[TopRightIndex],Vertices[BottomRightIndex]));
		}
	}
}

void UProceduralTerrainComponent::GenerateMesh() const
{
	
	ProceduralTerrainMesh->CreateMeshSection(0,Vertices,Triangles,Normals,UV,VertexColors,Tangents,true);


	if(Mat)
		ProceduralTerrainMesh->SetMaterial(0,Mat);
	
}

float UProceduralTerrainComponent::GetNoiseValueForGridCoordinates(const int X, const int Y) const
{
	//Array Order [North,NorthEast,East,SouthEast,South,SouthWest,West,NorthWest]
	//Check if all of the noise maps are created 
	if(TerrainNoise && ConnectionNoises[0] && ConnectionNoises[1] && ConnectionNoises[2] && ConnectionNoises[3] && ConnectionNoises[4]
		&& ConnectionNoises[5] && ConnectionNoises[6] && ConnectionNoises[7])
	{
		const float CurrentTerrainNoise =  TerrainNoise->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0)* FastNoiseOutputScale + WaterHeightOffset;
		//South Side 
		if(X == 0)
		{
			//SouthWestCorner
			if(Y == 0)
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =EvaluateConnectionNoiseAtIndex(4,X,Y); //South
					const float SecondNoise = CurrentTerrainNoise; //Current
					const float ThirdNoise = EvaluateConnectionNoiseAtIndex(6,X,Y) ; //West
					const float FourthNoise = EvaluateConnectionNoiseAtIndex(5,X,Y); //SouthWest
					return (FirstNoise+SecondNoise+ThirdNoise+FourthNoise)/4;
				}
			}
			//SouthEastCorner
			else if(Y == NoiseSamplesPerLine-1)
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =EvaluateConnectionNoiseAtIndex(4,X,Y); //South
					const float SecondNoise = CurrentTerrainNoise; //Current
					const float ThirdNoise = EvaluateConnectionNoiseAtIndex(2,X,Y); //East
					const float FourthNoise = EvaluateConnectionNoiseAtIndex(3,X,Y); //SourthEast
					return (FirstNoise+SecondNoise+ThirdNoise+FourthNoise)/4;
				}
			}
			else
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =EvaluateConnectionNoiseAtIndex(4,X,Y); //South
					const float SecondNoise = CurrentTerrainNoise; //Current
					return (FirstNoise+SecondNoise) * 0.5 ;
				}
			}
		}
		else if(X == NoiseSamplesPerLine-1)
		{
			//North
			if(Y==0)
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =EvaluateConnectionNoiseAtIndex(0,X,Y); //North
					const float SecondNoise = CurrentTerrainNoise; //Current
					const float ThirdNoise = EvaluateConnectionNoiseAtIndex(6,X,Y); //West
					const float FourthNoise = EvaluateConnectionNoiseAtIndex(7,X,Y); //NorthWest
					return (FirstNoise+SecondNoise+ThirdNoise+FourthNoise)/4;
				}
			}
			else if(Y==NoiseSamplesPerLine-1)
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =EvaluateConnectionNoiseAtIndex(0,X,Y); //North
					const float SecondNoise = CurrentTerrainNoise; //Current
					const float ThirdNoise = EvaluateConnectionNoiseAtIndex(2,X,Y); //East
					const float FourthNoise =EvaluateConnectionNoiseAtIndex(1,X,Y); //NorthEast
					return (FirstNoise+SecondNoise+ThirdNoise+FourthNoise)/4;
				}
			}
			else
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =EvaluateConnectionNoiseAtIndex(0,X,Y); //North
					const float SecondNoise = CurrentTerrainNoise; //Current
					return (FirstNoise+SecondNoise) * 0.5 ;
				}
			}
		
		}
		else if(Y == 0)
		{
			//Take an average of all the sides that connect to this point on the map
			if(bAverage)
			{
				const float FirstNoise =EvaluateConnectionNoiseAtIndex(6,X,Y); //East
				const float SecondNoise = CurrentTerrainNoise; //Current
				return (FirstNoise+SecondNoise) * 0.5 ;
			}
		}
		else if(Y == NoiseSamplesPerLine-1)
		{
			//Take an average of all the sides that connect to this point on the map
			if(bAverage)
			{
				const float FirstNoise =EvaluateConnectionNoiseAtIndex(2,X,Y); //West
				const float SecondNoise = CurrentTerrainNoise; //Current
				return (FirstNoise+SecondNoise) * 0.5;
			}
		}
		//If we are not on one of the edges of the terrain we do our normal noise calculation
		else
		{
			return CurrentTerrainNoise;
		}
	}
	return 0;

}

int UProceduralTerrainComponent::GetIndexForGridCoordinates(const int X, const int Y) const
{
	return X + Y * NoiseSamplesPerLine;
}

FVector2D UProceduralTerrainComponent::GetPositionForGridCoordinates(const int X, const int Y) const
{
	return FVector2D(
		X * NoiseResolution,
		Y * NoiseResolution
	);
}





