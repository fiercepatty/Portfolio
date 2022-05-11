// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainComponent.h"

#include "TerrainStructInfo.h"

// Sets default values for this component's properties
UProceduralTerrainComponent::UProceduralTerrainComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	ProceduralTerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralTerrainMesh"));
	
	TerrainNoise =CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("FastNoiseWrapper"));

	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("NorthNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("NorthEastNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("EastNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("SouthEastNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("SouthNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("SouthWestNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("WestNoise")));
	ConnectionNoises.Add(CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("NorthWestNoise")));
	// ...
}

void UProceduralTerrainComponent::InitializeFastNoise(const FTerrainInfo Terrain)
{
	TerrainNoise->SetupFastNoise(Terrain.NoiseType, Terrain.Seed, Terrain.Frequency, Terrain.Interp, Terrain.FractalType,
				Terrain.Octaves,Terrain.Lacunarity,Terrain.Gain,Terrain.CellularJitter, Terrain.CellularDistanceFunction, Terrain.CellularReturnType);
	NoiseResolution=Terrain.NoiseResolution;
	TotalSizeToGenerate=Terrain.TotalSizeToGenerate;
	NoiseInputScale=Terrain.NoiseInputScale;
	FastNoiseOutputScale=Terrain.NoiseOutputScale;
	bAverage=Terrain.bAverageTerrainConnections;
	
}

void UProceduralTerrainComponent::InitializeConnectionNoise(TArray<FTerrainInfo> Terrains) 
{
	for(int Index=0;Index < ConnectionNoises.Num();Index++)
	{
		ConnectionNoises[Index]->SetupFastNoise(Terrains[Index].NoiseType, Terrains[Index].Seed, Terrains[Index].Frequency, Terrains[Index].Interp, Terrains[Index].FractalType,
				Terrains[Index].Octaves,Terrains[Index].Lacunarity,Terrains[Index].Gain,Terrains[Index].CellularJitter, Terrains[Index].CellularDistanceFunction, Terrains[Index].CellularReturnType);
	}
}

void UProceduralTerrainComponent::GenerateMap(const FVector StartLocation)
{
	ComponentLocation= StartLocation;
	

	NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;
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

void UProceduralTerrainComponent::GenerateVertices()
{
	Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
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
	constexpr int QuadSize = 6; // This is the number of triangle indexes making up a quad (square section of the grid)
	const int NumberOfQuadsPerLine = NoiseSamplesPerLine - 1; // We have one less quad per line than the amount of vertices, since each vertex is the start of a quad except the last ones
	// In our triangles array, we need 6 values per quad
	const int TrianglesArraySize = NumberOfQuadsPerLine * NumberOfQuadsPerLine * QuadSize;
	Triangles.Init(0, TrianglesArraySize);

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
		}
	}
}

void UProceduralTerrainComponent::GenerateMesh() const
{
	ProceduralTerrainMesh->CreateMeshSection(0,Vertices,Triangles,Normals,UV,VertexColors,Tangents,true);
	
}

float UProceduralTerrainComponent::GetNoiseValueForGridCoordinates(const int X, const int Y) const
{
	//Array Order [North,NorthEast,East,SouthEast,South,SouthWest,West,NorthWest]
	
	//Check if all of the noise maps are created 
	if(TerrainNoise && ConnectionNoises[0] && ConnectionNoises[1] && ConnectionNoises[2] && ConnectionNoises[3] && ConnectionNoises[4]
		&& ConnectionNoises[5] && ConnectionNoises[6] && ConnectionNoises[7])
	{
		//South Side 
		if(X == 0)
		{
			//SouthWestCorner
			if(Y == 0)
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =ConnectionNoises[4]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //South
					const float SecondNoise = TerrainNoise->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //Current
					const float ThirdNoise = ConnectionNoises[6]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //West
					const float FourthNoise = ConnectionNoises[5]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //SouthWest
					return (FirstNoise+SecondNoise+ThirdNoise+FourthNoise)/4 *FastNoiseOutputScale;
				}
			}
			//SouthEastCorner
			else if(Y == NoiseSamplesPerLine-1)
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =ConnectionNoises[4]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //South
					const float SecondNoise = TerrainNoise->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //Current
					const float ThirdNoise = ConnectionNoises[2]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //East
					const float FourthNoise = ConnectionNoises[3]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //SourthEast
					return (FirstNoise+SecondNoise+ThirdNoise+FourthNoise)/4 *FastNoiseOutputScale;
				}
			}
			else
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =ConnectionNoises[4]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //South
					const float SecondNoise = TerrainNoise->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //Current
					return (FirstNoise+SecondNoise) * 0.5 * FastNoiseOutputScale;
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
					const float FirstNoise =ConnectionNoises[0]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //North
					const float SecondNoise = TerrainNoise->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //Current
					const float ThirdNoise = ConnectionNoises[6]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //West
					const float FourthNoise = ConnectionNoises[7]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //NorthWest
					return (FirstNoise+SecondNoise+ThirdNoise+FourthNoise)/4 *FastNoiseOutputScale;
				}
			}
			else if(Y==NoiseSamplesPerLine-1)
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =ConnectionNoises[0]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //North
					const float SecondNoise = TerrainNoise->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //Current
					const float ThirdNoise = ConnectionNoises[2]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //East
					const float FourthNoise = ConnectionNoises[1]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //NorthEast
					return (FirstNoise+SecondNoise+ThirdNoise+FourthNoise)/4 *FastNoiseOutputScale;
				}
			}
			else
			{
				//Take an average of all the sides that connect to this point on the map
				if(bAverage)
				{
					const float FirstNoise =ConnectionNoises[0]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //North
					const float SecondNoise = TerrainNoise->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //Currrent
					return (FirstNoise+SecondNoise) * 0.5 * FastNoiseOutputScale;
				}
			}
		
		}
		else if(Y == 0)
		{
			//Take an average of all the sides that connect to this point on the map
			if(bAverage)
			{
				const float FirstNoise =ConnectionNoises[6]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //East
				const float SecondNoise = TerrainNoise->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //Current
				return (FirstNoise+SecondNoise) * 0.5 * FastNoiseOutputScale;
			}
		}
		else if(Y == NoiseSamplesPerLine-1)
		{
			//Take an average of all the sides that connect to this point on the map
			if(bAverage)
			{
				const float FirstNoise =ConnectionNoises[2]->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //West
				const float SecondNoise = TerrainNoise->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //Current
				return (FirstNoise+SecondNoise) * 0.5 * FastNoiseOutputScale;
			}
		}
		//If we are not on one of the edges of the terrain we do our normal noise calculation
		else
		{
			return TerrainNoise->GetNoise3D(
			(X+ ComponentLocation.X),(Y+ComponentLocation.Y),
			0.0)* FastNoiseOutputScale;
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



