// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/WaterGeneration/ProceduralWaterComponent.h"

// Sets default values for this component's properties
UProceduralWaterComponent::UProceduralWaterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//Procedural Mesh
	ProceduralWaterMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralWaterMesh"));
	
	// ...
}

void UProceduralWaterComponent::GenerateMap(const FVector StartingLocation)
{
	//Start location for map to generate around
	ComponentLocation= StartingLocation;
	
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

void UProceduralWaterComponent::InitializeWaterComponent(const FTerrainInfo TerrainInfo)
{
	//Save off the variables we need for calculations so we dont have to use getter in the fast noise to get access to these variables
	NoiseResolution=TerrainInfo.NoiseResolution;
	TotalSizeToGenerate=TerrainInfo.TotalSizeToGenerate;
	
	OutputScale=TerrainInfo.NoiseOutputScale;
	bEnableCollision = TerrainInfo.bWaterHasCollision;
	WaterHeight = TerrainInfo.WaterHeight;
	
	
	Mat=TerrainInfo.WaterMaterial;
}

void UProceduralWaterComponent::UnLoadMesh() const
{
	ProceduralWaterMesh->SetVisibility(false);
	if(bEnableCollision)
		ProceduralWaterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UProceduralWaterComponent::LoadMesh() const
{
	ProceduralWaterMesh->SetVisibility(true);
	if(bEnableCollision)
		ProceduralWaterMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UProceduralWaterComponent::GenerateVertices()
{
	Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
	//Loop through each vertices and set the position the vertices is located at scaled to the actual mesh
	for (int y = 0; y < NoiseSamplesPerLine; y++) {
		for (int x = 0; x < NoiseSamplesPerLine; x++) {
			//Noise Result is just the water height dont do any calculation
			const float NoiseResult = WaterHeight;
			const int Index = GetIndexForGridCoordinates(x, y);
			const FVector2D Position = GetPositionForGridCoordinates(x, y);
			Vertices[Index] = FVector(Position.X, Position.Y, NoiseResult);
			UV[Index] = FVector2D(x, y);
		}
	}
}

void UProceduralWaterComponent::GenerateTriangles()
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

void UProceduralWaterComponent::GenerateMesh() const
{
	ProceduralWaterMesh->CreateMeshSection(0,Vertices,Triangles,Normals,UV,VertexColors,Tangents,true);
	if(Mat)
		ProceduralWaterMesh->SetMaterial(0,Mat);
	if(!bEnableCollision)
	{
		ProceduralWaterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ProceduralWaterMesh->SetCanEverAffectNavigation(false);
	}
	else
	{
		ProceduralWaterMesh->bFillCollisionUnderneathForNavmesh=true;
	}
	
}

int UProceduralWaterComponent::GetIndexForGridCoordinates(int X, int Y) const
{
	return X + Y * NoiseSamplesPerLine;
}

FVector2D UProceduralWaterComponent::GetPositionForGridCoordinates(int X, int Y) const
{
	return FVector2D(
		X * NoiseResolution,
		Y * NoiseResolution
	);
}

