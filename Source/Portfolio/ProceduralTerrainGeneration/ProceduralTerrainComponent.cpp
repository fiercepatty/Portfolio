// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/ProceduralTerrainComponent.h"

#include "TerrainStructInfo.h"

// Sets default values for this component's properties
UProceduralTerrainComponent::UProceduralTerrainComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	
	FastNoise =CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("FastNoiseWrapper"));

	ConnectionNoise =CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("ConnectionFastNoiseWrapper"));
	// ...
}

void UProceduralTerrainComponent::InitializeFastNoise(const FTerrainInfo Terrain)
{
	FastNoise->SetupFastNoise(Terrain.NoiseType, Terrain.Seed, Terrain.Frequency, Terrain.Interp, Terrain.FractalType,
				Terrain.Octaves,Terrain.Lacunarity,Terrain.Gain,Terrain.CellularJitter, Terrain.CellularDistanceFunction, Terrain.CellularReturnType);
	NoiseResolution=Terrain.NoiseResolution;
	TotalSizeToGenerate=Terrain.TotalSizeToGenerate;
	NoiseInputScale=Terrain.NoiseInputScale;
	FastNoiseOutputScale=Terrain.NoiseOutputScale;
	
}

void UProceduralTerrainComponent::InitializeConnectionNoise(const FTerrainInfo Terrain) 
{
	ConnectionNoise->SetupFastNoise(Terrain.NoiseType, Terrain.Seed, Terrain.Frequency, Terrain.Interp, Terrain.FractalType,
				Terrain.Octaves,Terrain.Lacunarity,Terrain.Gain,Terrain.CellularJitter, Terrain.CellularDistanceFunction, Terrain.CellularReturnType);
	ConnectionNoiseOutputScale=Terrain.NoiseOutputScale;
}

void UProceduralTerrainComponent::GenerateMap(FVector StartLocation)
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
	bIsVisible=true;
	bGenerated=true;
}



bool UProceduralTerrainComponent::IsGenerated() const
{
	return bGenerated;
}

bool UProceduralTerrainComponent::IsVisible() const
{
	return bIsVisible;
}

void UProceduralTerrainComponent::UnLoadMesh()
{
	ProceduralMesh->SetVisibility(false);
	bIsVisible=false;
}

void UProceduralTerrainComponent::LoadMesh()
{
	ProceduralMesh->SetVisibility(true);
	bIsVisible=true;
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
	ProceduralMesh->CreateMeshSection(0,Vertices,Triangles,Normals,UV,VertexColors,Tangents,true);
	
}

float UProceduralTerrainComponent::GetNoiseValueForGridCoordinates(const int X, const int Y) const
{
	if(FastNoise)
	{
		return FastNoise->GetNoise3D(
		(X+ ComponentLocation.X),(Y+ComponentLocation.Y),
		0.0)* FastNoiseOutputScale;
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



