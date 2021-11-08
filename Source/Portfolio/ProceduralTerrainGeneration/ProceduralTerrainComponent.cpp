// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainGeneration/ProceduralTerrainComponent.h"
// Sets default values for this component's properties
UProceduralTerrainComponent::UProceduralTerrainComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	ProceduralMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("GeneratedMesh"));
	FastNoise =CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("FastNoiseWrapper"));
	// ...
}

void UProceduralTerrainComponent::GenerateMap(const FVector StartingLocation)
{
	ComponentLocation= StartingLocation;
	Shapes=EComponentShapes::Ve_Plain;

	FastNoise->SetupFastNoise(NoiseType, Seed, Frequency, Interp, FractalType,
				Octaves,Lacunarity,Gain,CellularJitter, CellularDistanceFunction, CellularReturnType);

	NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;
	Normals.Init(FVector(0, 0, 1), VerticesArraySize);
	Tangents.Init(FRuntimeMeshTangent(0, -1, 0), VerticesArraySize);
	UV.Init(FVector2D(0, 0), VerticesArraySize);
	VertexColors.Init(FColor::White, VerticesArraySize);
	
	GenerateVertices();
	GenerateTriangles();
	GenerateMesh();
	bIsVisible=true;
	bGenerated=true;
}

void UProceduralTerrainComponent::GenerateMapSphere(FVector StartingLocation, EShapeSide SideOfShape,
	ESubSections ShapeSection)
{
	Shapes=EComponentShapes::Ve_Sphere;
	ShapeSubSections=ShapeSection;
	ShapeSide=SideOfShape;
	ComponentLocation= StartingLocation;
	
	FastNoise->SetupFastNoise(NoiseType, Seed, Frequency, Interp, FractalType,
				Octaves,Lacunarity,Gain,CellularJitter, CellularDistanceFunction, CellularReturnType);

	NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;
	Normals.Init(FVector(0, 0, 1), VerticesArraySize);
	Tangents.Init(FRuntimeMeshTangent(0, -1, 0), VerticesArraySize);
	UV.Init(FVector2D(0, 0), VerticesArraySize);
	VertexColors.Init(FColor::White, VerticesArraySize);
	
	GenerateVertices();
	GenerateTriangles();
	GenerateMesh();
	bIsVisible=true;
	bGenerated=true;
}

void UProceduralTerrainComponent::GenerateMap(const FVector StartingLocation, const EShapeSide SideOfShape, const ESubSections ShapeSection)
{
	Shapes=EComponentShapes::Ve_Cube;
	ShapeSubSections=ShapeSection;
	ShapeSide=SideOfShape;
	ComponentLocation= StartingLocation;
	
	FastNoise->SetupFastNoise(NoiseType, Seed, Frequency, Interp, FractalType,
				Octaves,Lacunarity,Gain,CellularJitter, CellularDistanceFunction, CellularReturnType);

	NoiseSamplesPerLine = TotalSizeToGenerate / NoiseResolution;
	VerticesArraySize = NoiseSamplesPerLine * NoiseSamplesPerLine;
	Normals.Init(FVector(0, 0, 1), VerticesArraySize);
	Tangents.Init(FRuntimeMeshTangent(0, -1, 0), VerticesArraySize);
	UV.Init(FVector2D(0, 0), VerticesArraySize);
	VertexColors.Init(FColor::White, VerticesArraySize);
	
	GenerateVertices();
	GenerateTriangles();
	GenerateMesh();
	bIsVisible=true;
	bGenerated=true;
}



void UProceduralTerrainComponent::GenerateVertices()
{
	switch (Shapes)
	{
	case EComponentShapes::Ve_Plain:
		{
			Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
			for (int y = 0; y < NoiseSamplesPerLine; y++) {
				for (int x = 0; x < NoiseSamplesPerLine; x++) {
					const float NoiseResult = GetNoiseValueForGridCoordinates(x, y,0);
					const int Index = GetIndexForGridCoordinates(x, y);
					const FVector2D Position = GetPositionForGridCoordinates(x, y);
					Vertices[Index] = FVector(Position.X, Position.Y, NoiseResult);
					UV[Index] = FVector2D(x, y);
				}
			}
			break;
		}
	case EComponentShapes::Ve_Cube:
		{
			bool bNeedToGenerateNoise=false;
			float NoiseResult =0;
			switch (ShapeSide)
			{
			case EShapeSide::Ve_Top:
				{
					
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int y = 0; y < NoiseSamplesPerLine; y++) {
						for (int x = 0; x < NoiseSamplesPerLine; x++) {
							bNeedToGenerateNoise=true;
							if(bClampTop && x==NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampBottom && x==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampRight && y==NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampLeft && y==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bNeedToGenerateNoise)
							{
								NoiseResult = GetNoiseValueForGridCoordinates(x, y,0);
							}
							const int Index = GetIndexForGridCoordinates(x, y);
							const FVector2D Position = GetPositionForGridCoordinates(x, y);
							Vertices[Index] = FVector(Position.X, Position.Y, NoiseResult);
							UV[Index] = FVector2D(x, y);
						}
					}
					break;
				}
			case EShapeSide::Ve_Bottom:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int y = 0; y < NoiseSamplesPerLine; y++) {
						for (int x = 0; x < NoiseSamplesPerLine; x++) {
							bNeedToGenerateNoise=true;
							if(bClampTop && x==NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampBottom && x==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampRight && y==NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampLeft && y==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bNeedToGenerateNoise)
							{
								NoiseResult = GetNoiseValueForGridCoordinates(-x, y,0);
							}
							const int Index = GetIndexForGridCoordinates(x, y);
							const FVector2D Position = GetPositionForGridCoordinates(x, y);
							Vertices[Index] = FVector(-Position.X+TotalSizeToGenerate-NoiseResolution, Position.Y, -(NoiseResult));
							UV[Index] = FVector2D(x, y);
						}
					}
					break;
				}
			case EShapeSide::Ve_Left:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int z = 0; z < NoiseSamplesPerLine; z++) {
						for (int x = 0; x < NoiseSamplesPerLine; x++) {
							bNeedToGenerateNoise=true;
							if(bClampTop && x==NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampBottom && x==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampRight && z== NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampLeft && z==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bNeedToGenerateNoise)
							{
								NoiseResult = GetNoiseValueForGridCoordinates(x, 0,z);
							}
							const int Index = GetIndexForGridCoordinates(x, z);
							const FVector2D Position = GetPositionForGridCoordinates(x, z);
							Vertices[Index] = FVector(Position.X, -(NoiseResult), Position.Y);
							UV[Index] = FVector2D(x, z);
						}
					}
					break;
				}
			case EShapeSide::Ve_Right:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int z = 0; z < NoiseSamplesPerLine; z++) {
						for (int x = 0; x < NoiseSamplesPerLine; x++) {
							bNeedToGenerateNoise=true;
							if(bClampTop && x==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampBottom && x==NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampRight && z==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampLeft && z==NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bNeedToGenerateNoise)
							{
								NoiseResult = GetNoiseValueForGridCoordinates(-x, 0,z);
							}
							const int Index = GetIndexForGridCoordinates(x, z);
							const FVector2D Position = GetPositionForGridCoordinates(x, z);
							Vertices[Index] = FVector(-Position.X+TotalSizeToGenerate-NoiseResolution, (NoiseResult+TotalSizeToGenerate-NoiseResolution), Position.Y);
							UV[Index] = FVector2D(x, z);
						}
					}
					break;
				}
			case EShapeSide::Ve_Front:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int y = 0; y < NoiseSamplesPerLine; y++) {
						for (int z = 0; z < NoiseSamplesPerLine; z++) {
							bNeedToGenerateNoise=true;
							if(bClampTop && z ==NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampBottom && z==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampRight && y==NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampLeft && y == 0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bNeedToGenerateNoise)
							{
								NoiseResult = GetNoiseValueForGridCoordinates(0, y,z);
							}
							const int Index = GetIndexForGridCoordinates(z, y);
							const FVector2D Position = GetPositionForGridCoordinates(z, y);
							Vertices[Index] = FVector(-(NoiseResult), Position.Y,Position.X );
							UV[Index] = FVector2D(z, y);
						}
					}
					break;
				}
			case EShapeSide::Ve_Back:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int y = 0; y < NoiseSamplesPerLine; y++) {
						for (int z = 0; z < NoiseSamplesPerLine; z++) {
							bNeedToGenerateNoise=true;
							if(bClampTop && z ==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampBottom && z==NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampRight && y==0)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bClampLeft && y == NoiseSamplesPerLine-1)
							{
								bNeedToGenerateNoise=false;
								NoiseResult=0;
							}
							if(bNeedToGenerateNoise)
							{
								NoiseResult = GetNoiseValueForGridCoordinates(0, -y,z);
							}
							const int Index = GetIndexForGridCoordinates(z, y);
							const FVector2D Position = GetPositionForGridCoordinates(z, y);
							Vertices[Index] = FVector((NoiseResult+TotalSizeToGenerate-NoiseResolution), -Position.Y+TotalSizeToGenerate-NoiseResolution,Position.X );
							UV[Index] = FVector2D(y,z);
						}
					}
					break;
				}
			default: ;
			}
			break;
		}
	case EComponentShapes::Ve_Sphere:
		{
			float NoiseResult =0;
			switch (ShapeSide)
			{
			case EShapeSide::Ve_Top:
				{
					
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int y = 0; y < NoiseSamplesPerLine; y++) {
						for (int x = 0; x < NoiseSamplesPerLine; x++) {
							NoiseResult =0;
							//NoiseResult = GetNoiseValueForGridCoordinates(x, y,0);
							const int Index = GetIndexForGridCoordinates(x, y);
							const FVector2D Position = GetPositionForGridCoordinates(x, y);
							Vertices[Index] = FVector(Position.X, Position.Y, NoiseResult).GetSafeNormal();
							UV[Index] = FVector2D(x, y);
						}
					}
					break;
				}
			case EShapeSide::Ve_Bottom:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int y = 0; y < NoiseSamplesPerLine; y++) {
						for (int x = 0; x < NoiseSamplesPerLine; x++) {
							NoiseResult =0;
							//NoiseResult = GetNoiseValueForGridCoordinates(-x, y,0);
							const int Index = GetIndexForGridCoordinates(x, y);
							const FVector2D Position = GetPositionForGridCoordinates(x, y);
							Vertices[Index] = FVector(-Position.X+TotalSizeToGenerate-NoiseResolution, Position.Y, -(NoiseResult)).GetSafeNormal();
							UV[Index] = FVector2D(x, y);
						}
					}
					break;
				}
			case EShapeSide::Ve_Left:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int z = 0; z < NoiseSamplesPerLine; z++) {
						for (int x = 0; x < NoiseSamplesPerLine; x++) {
							//NoiseResult = GetNoiseValueForGridCoordinates(x, 0,z);
							NoiseResult =0;
							const int Index = GetIndexForGridCoordinates(x, z);
							const FVector2D Position = GetPositionForGridCoordinates(x, z);
							Vertices[Index] = FVector(Position.X, -(NoiseResult), Position.Y).GetSafeNormal();
							UV[Index] = FVector2D(x, z);
						}
					}
					break;
				}
			case EShapeSide::Ve_Right:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int z = 0; z < NoiseSamplesPerLine; z++) {
						for (int x = 0; x < NoiseSamplesPerLine; x++) {
						
							//NoiseResult = GetNoiseValueForGridCoordinates(-x, 0,z);
							NoiseResult =0;
							const int Index = GetIndexForGridCoordinates(x, z);
							const FVector2D Position = GetPositionForGridCoordinates(x, z);
							Vertices[Index] = FVector(-Position.X+TotalSizeToGenerate-NoiseResolution, (NoiseResult+TotalSizeToGenerate-NoiseResolution), Position.Y).GetSafeNormal();
							UV[Index] = FVector2D(x, z);
						}
					}
					break;
				}
			case EShapeSide::Ve_Front:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int y = 0; y < NoiseSamplesPerLine; y++) {
						for (int z = 0; z < NoiseSamplesPerLine; z++) {
							//NoiseResult = GetNoiseValueForGridCoordinates(0, y,z);
							NoiseResult =0;

							const int Index = GetIndexForGridCoordinates(z, y);
							const FVector2D Position = GetPositionForGridCoordinates(z, y);
							Vertices[Index] = FVector(-(NoiseResult), Position.Y,Position.X ).GetSafeNormal();
							UV[Index] = FVector2D(z, y);
						}
					}
					break;
				}
			case EShapeSide::Ve_Back:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int y = 0; y < NoiseSamplesPerLine; y++) {
						for (int z = 0; z < NoiseSamplesPerLine; z++) {
							//NoiseResult = GetNoiseValueForGridCoordinates(0, -y,z);
							NoiseResult =0;
							const int Index = GetIndexForGridCoordinates(z, y);
							const FVector2D Position = GetPositionForGridCoordinates(z, y);
							Vertices[Index] = FVector((NoiseResult+TotalSizeToGenerate-NoiseResolution), -Position.Y+TotalSizeToGenerate-NoiseResolution,Position.X ).GetSafeNormal();
							UV[Index] = FVector2D(y,z);
						}
					}
					break;
				}
			default: ;
			}
			break;
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
	ProceduralMesh->CreateSectionFromComponents(0,0,0,Vertices,Triangles,Normals,UV,VertexColors,Tangents);
}

float UProceduralTerrainComponent::GetNoiseValueForGridCoordinates(const int X, const int Y, const int Z) const
{
	return FastNoise->GetNoise3D(
		(X+ ComponentLocation.X),(Y+ComponentLocation.Y),
		(Z+ ComponentLocation.Z))* NoiseOutputScale;
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

void UProceduralTerrainComponent::InitializeVariables(UNoiseParameters* NoiseParameters)
{
	NoiseType=NoiseParameters->NoiseType;
	Seed = NoiseParameters->Seed;
	Frequency= NoiseParameters->Frequency;
	Interp=NoiseParameters->Interp;
	FractalType=NoiseParameters->FractalType;
	Octaves=NoiseParameters->Octaves;
	Lacunarity=NoiseParameters->Lacunarity;
	Gain=NoiseParameters->Gain;
	CellularJitter=NoiseParameters->CellularJitter;
	CellularDistanceFunction=NoiseParameters->CellularDistanceFunction;
	CellularReturnType=NoiseParameters->CellularReturnType;
	NoiseResolution=NoiseParameters->NoiseResolution;
	TotalSizeToGenerate=NoiseParameters->TotalSizeToGenerate;
	NoiseInputScale=NoiseParameters->NoiseInputScale;
	NoiseOutputScale=NoiseParameters->NoiseOutputScale;
}