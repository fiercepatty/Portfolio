// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/ProceduralTerrainComponent.h"

// Sets default values for this component's properties
UProceduralTerrainComponent::UProceduralTerrainComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	FastNoise =CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("FastNoiseWrapper"));
	// ...
}

void UProceduralTerrainComponent::GenerateMap(FVector StartLocation)
{
	ComponentLocation= StartLocation;
	
	FastNoise->SetupFastNoise(NoiseType, Seed, Frequency, Interp, FractalType,
				Octaves,Lacunarity,Gain,CellularJitter, CellularDistanceFunction, CellularReturnType);

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

bool UProceduralTerrainComponent::IsGenerated()
{
	return bGenerated;
}

bool UProceduralTerrainComponent::IsVisible()
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
			float NoiseResult = GetNoiseValueForGridCoordinates(x, y);
			int index = GetIndexForGridCoordinates(x, y);
			FVector2D Position = GetPositionForGridCoordinates(x, y);
			Vertices[index] = FVector(Position.X, Position.Y, NoiseResult);
			UV[index] = FVector2D(x, y);
		}
	}
}

void UProceduralTerrainComponent::GenerateTriangles()
{
	int QuadSize = 6; // This is the number of triangle indexes making up a quad (square section of the grid)
	int NumberOfQuadsPerLine = NoiseSamplesPerLine - 1; // We have one less quad per line than the amount of vertices, since each vertex is the start of a quad except the last ones
	// In our triangles array, we need 6 values per quad
	int TrianglesArraySize = NumberOfQuadsPerLine * NumberOfQuadsPerLine * QuadSize;
	Triangles.Init(0, TrianglesArraySize);

	for (int y = 0; y < NumberOfQuadsPerLine; y++) {
		for (int x = 0; x < NumberOfQuadsPerLine; x++) {
			int QuadIndex = x + y * NumberOfQuadsPerLine;
			int TriangleIndex = QuadIndex * QuadSize;

			// Getting the indexes of the four vertices making up this quad
			int bottomLeftIndex = GetIndexForGridCoordinates(x, y);
			int topLeftIndex = GetIndexForGridCoordinates(x, y + 1);
			int topRightIndex = GetIndexForGridCoordinates(x + 1, y + 1);
			int bottomRightIndex = GetIndexForGridCoordinates(x + 1, y);

			// Assigning the 6 triangle points to the corresponding vertex indexes, by going counter-clockwise.
			Triangles[TriangleIndex] = bottomLeftIndex;
			Triangles[TriangleIndex + 1] = topLeftIndex;
			Triangles[TriangleIndex + 2] = topRightIndex;
			Triangles[TriangleIndex + 3] = bottomLeftIndex;
			Triangles[TriangleIndex + 4] = topRightIndex;
			Triangles[TriangleIndex + 5] = bottomRightIndex;
		}
	}
}

void UProceduralTerrainComponent::GenerateMesh()
{
	ProceduralMesh->CreateMeshSection(0,Vertices,Triangles,Normals,UV,VertexColors,Tangents,true);
}

float UProceduralTerrainComponent::GetNoiseValueForGridCoordinates(int x, int y)
{
	if(FastNoise)
	{
		return FastNoise->GetNoise3D(
		(x+ ComponentLocation.X),(y+ComponentLocation.Y),
		0.0)* NoiseOutputScale;
	}
	else
	{
		return 0;
	}
}

int UProceduralTerrainComponent::GetIndexForGridCoordinates(int x, int y)
{
	return x + y * NoiseSamplesPerLine;
}

FVector2D UProceduralTerrainComponent::GetPositionForGridCoordinates(int x, int y)
{
	return FVector2D(
		x * NoiseResolution,
		y * NoiseResolution
	);
}

void UProceduralTerrainComponent::SetNoiseTypeComponent(const EFastNoise_NoiseType NewNoiseType)
{
	NoiseType = NewNoiseType;
}

void UProceduralTerrainComponent::SetSeed(const int32 NewSeed)
{
	Seed = NewSeed;
}

void UProceduralTerrainComponent::SetFrequency(const float NewFrequency)
{
	Frequency = NewFrequency;
}

void UProceduralTerrainComponent::SetInterpolation(const EFastNoise_Interp NewInterpolation)
{
	Interp = NewInterpolation;
}

void UProceduralTerrainComponent::SetFractalType(const EFastNoise_FractalType NewFractalType)
{
	FractalType = NewFractalType;
}

void UProceduralTerrainComponent::SetOctaves(const int32 NewOctaves)
{
	Octaves=NewOctaves;
}

void UProceduralTerrainComponent::SetLacunarity(const float NewLacunarity)
{
	Lacunarity=NewLacunarity;
}

void UProceduralTerrainComponent::SetGain(const float NewGain)
{
	Gain=NewGain;
}

void UProceduralTerrainComponent::SetCellularJitter(const float NewCellularJitter)
{
	CellularJitter=NewCellularJitter;
}

void UProceduralTerrainComponent::SetDistanceFunction(const EFastNoise_CellularDistanceFunction NewDistanceFunction)
{
	CellularDistanceFunction=NewDistanceFunction;
}

void UProceduralTerrainComponent::SetReturnType(const EFastNoise_CellularReturnType NewCellularReturnType)
{
	CellularReturnType=NewCellularReturnType;
}

void UProceduralTerrainComponent::SetNoiseResolution(int NewNoiseResolution)
{
	NoiseResolution=NewNoiseResolution;
}

void UProceduralTerrainComponent::SetTotalSizeToGenerate(int NewTotalSizeToGenerate)
{
	TotalSizeToGenerate=NewTotalSizeToGenerate;
}

void UProceduralTerrainComponent::SetNoiseInputScale(float NewNoiseInputScale)
{
	NoiseInputScale=NewNoiseInputScale;
}

void UProceduralTerrainComponent::SetNoiseOutputScale(float NewNoiseOutputScale)
{
	NoiseOutputScale=NewNoiseOutputScale;
}




