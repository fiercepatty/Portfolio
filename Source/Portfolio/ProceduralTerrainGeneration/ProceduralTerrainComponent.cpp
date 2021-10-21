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

void UProceduralTerrainComponent::GenerateMap(const FVector StartingLocation, const EComponentShapes ComponentShape, const EShapeSide SideOfShape, const ESubSections ShapeSection)
{
	Shapes=ComponentShape;
	ShapeSubSections=ShapeSection;
	ShapeSide=SideOfShape;
	ComponentLocation= StartingLocation;
	
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
			switch (ShapeSide)
			{
			case EShapeSide::Ve_Top:
				{
					Vertices.Init(FVector(0, 0, 0), VerticesArraySize);
					for (int y = 0; y < NoiseSamplesPerLine; y++) {
						for (int x = 0; x < NoiseSamplesPerLine; x++) {
							float NoiseResult = GetNoiseValueForGridCoordinates(x, y,0);
							if(EdgeOfGeometricObject!=EEdgeOfGeometricObject::Ve_None)
							{
								switch (EdgeOfGeometricObject)
								{
								case EEdgeOfGeometricObject::Ve_NorthStart:
									{
										if(y==NoiseSamplesPerLine-1 || x == 0)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_NorthMiddle:
									{
										if(y==NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_NorthEnd:
									{
										if(y==0 || x == 0)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestBottom:
									{
										if(y==NoiseSamplesPerLine-1)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestTop:
									{
										if(y==0)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestEndTop:
									{
										if(y==0 || x == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_WestEndBottom:
									{
										if(y==NoiseSamplesPerLine-1 || x == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_WestEndMiddle:
									{
										if(x == NoiseSamplesPerLine-1) 
										{
											NoiseResult = 0;
										}
										break;
									}
								default: ;
;
								}
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
							float NoiseResult = GetNoiseValueForGridCoordinates(-x, y,0);
							if(EdgeOfGeometricObject!=EEdgeOfGeometricObject::Ve_None)
							{
								switch (EdgeOfGeometricObject)
								{
								case EEdgeOfGeometricObject::Ve_SouthStart:
									{
										if(y==NoiseSamplesPerLine-1 || x == 0)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_SouthMiddle:
									{
										if(y==NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_SouthEnd:
									{
										if(y==0 || x == 0)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestBottom:
									{
										if(y==NoiseSamplesPerLine-1)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestTop:
									{
										if(y==0)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestEndTop:
									{
										if(y==0 || x == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_WestEndBottom:
									{
										if(y==NoiseSamplesPerLine-1 || x == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_WestEndMiddle:
									{
										if(x == NoiseSamplesPerLine-1) 
										{
											NoiseResult = 0;
										}
										break;
									}
								default: ;
								}
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
							float NoiseResult = GetNoiseValueForGridCoordinates(x, 0,z);
							if(EdgeOfGeometricObject!=EEdgeOfGeometricObject::Ve_None)
							{
								switch (EdgeOfGeometricObject)
								{
								case EEdgeOfGeometricObject::Ve_WestBottom:
									{
										if(z==NoiseSamplesPerLine-1)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestTop:
									{
										if(z==0)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestEndTop:
									{
										if(z==0 || x == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_WestEndBottom:
									{
										if(z==NoiseSamplesPerLine-1 || x == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_WestEndMiddle:
									{
										if(x == NoiseSamplesPerLine-1) 
										{
											NoiseResult = 0;
										}
										break;
									}
								default: ;
								}
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
							float NoiseResult = GetNoiseValueForGridCoordinates(-x,0, z);
							if(EdgeOfGeometricObject!=EEdgeOfGeometricObject::Ve_None)
							{
								switch (EdgeOfGeometricObject)
								{
								case EEdgeOfGeometricObject::Ve_EastBottom:
									{
										if(z==NoiseSamplesPerLine-1)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_EastTop:
									{
										if(z==0)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_EastEndTop:
									{
										if(z==0 || x == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_EastEndBottom:
									{
										if(z==NoiseSamplesPerLine-1 || x == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_EastEndMiddle:
									{
										if(x == NoiseSamplesPerLine-1) 
										{
											NoiseResult = 0;
										}
										break;
									}
								default: ;
								}
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
							float NoiseResult = GetNoiseValueForGridCoordinates(0, y,z);
							if(EdgeOfGeometricObject!=EEdgeOfGeometricObject::Ve_None)
							{
								switch (EdgeOfGeometricObject)
								{
								case EEdgeOfGeometricObject::Ve_SouthStart:
									{
										if(y==NoiseSamplesPerLine-1 || z == 0)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_SouthMiddle:
									{
										if(y==NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_SouthEnd:
									{
										if(y==0 || z == 0)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestBottom:
									{
										if(y==NoiseSamplesPerLine-1)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestTop:
									{
										if(y==0)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestEndTop:
									{
										if(y==0 || z == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_WestEndBottom:
									{
										if(y==NoiseSamplesPerLine-1 || z == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_WestEndMiddle:
									{
										if(z == NoiseSamplesPerLine-1) 
										{
											NoiseResult = 0;
										}
										break;
									}
								default: ;
								}
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
							float NoiseResult = GetNoiseValueForGridCoordinates(0, -y,z);
							if(EdgeOfGeometricObject!=EEdgeOfGeometricObject::Ve_None)
							{
								switch (EdgeOfGeometricObject)
								{
								case EEdgeOfGeometricObject::Ve_NorthStart:
									{
										if(y==NoiseSamplesPerLine-1 || z == 0)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_NorthMiddle:
									{
										if(y==NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_NorthEnd:
									{
										if(y==0 || z == 0)
										{
											NoiseResult = 0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestBottom:
									{
										if(y==NoiseSamplesPerLine-1)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestTop:
									{
										if(y==0)
										{
											NoiseResult=0;
										}
										break;
									}
								case EEdgeOfGeometricObject::Ve_WestEndTop:
									{
										if(y==0 || z == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_WestEndBottom:
									{
										if(y==NoiseSamplesPerLine-1 || z == NoiseSamplesPerLine-1)
										{
											NoiseResult = 0;
										}
										break;
									}

								case EEdgeOfGeometricObject::Ve_WestEndMiddle:
									{
										if(z == NoiseSamplesPerLine-1) 
										{
											NoiseResult = 0;
										}
										break;
									}
								default: ;
									;
								}
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
			switch (ShapeSide)
			{
			case EShapeSide::Ve_Top:
				{
					break;
				}
			case EShapeSide::Ve_Bottom:
				{
					break;
				}
			case EShapeSide::Ve_Left:
				{
					break;
				}
			case EShapeSide::Ve_Right:
				{
					break;
				}
			case EShapeSide::Ve_Front:
				{
					break;
				}
			case EShapeSide::Ve_Back:
				{
					break;
				}
			default: ;
			}
			break;
		}
	default: ;
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

void UProceduralTerrainComponent::SetNoiseResolution(const int NewNoiseResolution)
{
	NoiseResolution=NewNoiseResolution;
}

void UProceduralTerrainComponent::SetTotalSizeToGenerate(const int NewTotalSizeToGenerate)
{
	TotalSizeToGenerate=NewTotalSizeToGenerate;
}

void UProceduralTerrainComponent::SetNoiseInputScale(const float NewNoiseInputScale)
{
	NoiseInputScale=NewNoiseInputScale;
}

void UProceduralTerrainComponent::SetNoiseOutputScale(const float NewNoiseOutputScale)
{
	NoiseOutputScale=NewNoiseOutputScale;
}

void UProceduralTerrainComponent::SetEdgeOfGeometricObject(EEdgeOfGeometricObject GeometricObject)
{
	EdgeOfGeometricObject=GeometricObject;
}




