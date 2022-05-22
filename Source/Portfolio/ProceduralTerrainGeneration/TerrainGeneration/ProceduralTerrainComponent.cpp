// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppTooWideScope
#include "ProceduralTerrainComponent.h"

#include "MeshDescription.h"
#include "TerrainStructInfo.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "IAssetTools.h"
#include "DetailCategoryBuilder.h"
#include "ProceduralMeshComponent.h"
#include "StaticMeshAttributes.h"
#include "PhysicsEngine/BodySetup.h"
#include "Materials/Material.h"


// Sets default values for this component's properties
UProceduralTerrainComponent::UProceduralTerrainComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	ProceduralTerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralTerrainMesh"));

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
	
}

void UProceduralTerrainComponent::InitializeConnectionNoise(TArray<FTerrainInfo> Terrains) 
{
	//Init the connected terrain noises
	for(int Index=0;Index < ConnectionNoises.Num();Index++)
	{
		ConnectionNoises[Index]->SetupFastNoise(Terrains[Index].NoiseType, Terrains[Index].Seed, Terrains[Index].Frequency, Terrains[Index].Interp, Terrains[Index].FractalType,
				Terrains[Index].Octaves,Terrains[Index].Lacunarity,Terrains[Index].Gain,Terrains[Index].CellularJitter, Terrains[Index].CellularDistanceFunction, Terrains[Index].CellularReturnType);
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


TMap<UMaterialInterface*, FPolygonGroupID> BuildMaterialMap(const UProceduralMeshComponent* ProcMeshComp, FMeshDescription& MeshDescription)
{
	TMap<UMaterialInterface*, FPolygonGroupID> UniqueMaterials;
	const int32 NumSections = ProcMeshComp->GetNumSections();
	UniqueMaterials.Reserve(NumSections);

	FStaticMeshAttributes AttributeGetter(MeshDescription);
	const TPolygonGroupAttributesRef<FName> PolygonGroupNames = AttributeGetter.GetPolygonGroupMaterialSlotNames();
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		UMaterialInterface *Material = ProcMeshComp->GetMaterial(SectionIdx);
		if (Material == nullptr)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}

		if (!UniqueMaterials.Contains(Material))
		{
			FPolygonGroupID NewPolygonGroup = MeshDescription.CreatePolygonGroup();
			UniqueMaterials.Add(Material, NewPolygonGroup);
			PolygonGroupNames[NewPolygonGroup] = Material->GetFName();
		}
	}
	return UniqueMaterials;
}

FMeshDescription UProceduralTerrainComponent::BuildANewMeshDescription(UProceduralMeshComponent* ProcMeshComp)
{
	FMeshDescription MeshDescription;

	FStaticMeshAttributes AttributeGetter(MeshDescription);
	AttributeGetter.Register();

	TVertexAttributesRef<FVector> VertexPositions = AttributeGetter.GetVertexPositions();
	TVertexInstanceAttributesRef<FVector> DescriptionTangents = AttributeGetter.GetVertexInstanceTangents();
	TVertexInstanceAttributesRef<float> BinomialSigns = AttributeGetter.GetVertexInstanceBinormalSigns();
	TVertexInstanceAttributesRef<FVector> DescriptionNormals = AttributeGetter.GetVertexInstanceNormals();
	TVertexInstanceAttributesRef<FVector4> Colors = AttributeGetter.GetVertexInstanceColors();
	TVertexInstanceAttributesRef<FVector2D> UVs = AttributeGetter.GetVertexInstanceUVs();

	// Materials to apply to new mesh
	const int32 NumSections = ProcMeshComp->GetNumSections();
	int32 VertexCount = 0;
	int32 VertexInstanceCount = 0;
	int32 PolygonCount = 0;

	TMap<UMaterialInterface*, FPolygonGroupID> UniqueMaterials = BuildMaterialMap(ProcMeshComp, MeshDescription);
	TArray<FPolygonGroupID> PolygonGroupForSection;
	PolygonGroupForSection.Reserve(NumSections);

	// Calculate the totals for each ProcMesh element type
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection *ProcSection =
			ProcMeshComp->GetProcMeshSection(SectionIdx);
		VertexCount += ProcSection->ProcVertexBuffer.Num();
		VertexInstanceCount += ProcSection->ProcIndexBuffer.Num();
		PolygonCount += ProcSection->ProcIndexBuffer.Num() / 3;
	}
	MeshDescription.ReserveNewVertices(VertexCount);
	MeshDescription.ReserveNewVertexInstances(VertexInstanceCount);
	MeshDescription.ReserveNewPolygons(PolygonCount);
	MeshDescription.ReserveNewEdges(PolygonCount * 2);
	UVs.SetNumIndices(4);

	// Create the Polygon Groups
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		UMaterialInterface *Material = ProcMeshComp->GetMaterial(SectionIdx);
		if (Material == nullptr)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}

		FPolygonGroupID *PolygonGroupID = UniqueMaterials.Find(Material);
		check(PolygonGroupID != nullptr);
		PolygonGroupForSection.Add(*PolygonGroupID);
	}


	// Add Vertex and VertexInstance and polygon for each section
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection *ProcSection =
			ProcMeshComp->GetProcMeshSection(SectionIdx);
		FPolygonGroupID PolygonGroupID = PolygonGroupForSection[SectionIdx];
		// Create the vertex
		int32 NumVertex = ProcSection->ProcVertexBuffer.Num();
		TMap<int32, FVertexID> VertexIndexToVertexID;
		VertexIndexToVertexID.Reserve(NumVertex);
		for (int32 VertexIndex = 0; VertexIndex < NumVertex; ++VertexIndex)
		{
			FProcMeshVertex &Vert = ProcSection->ProcVertexBuffer[VertexIndex];
			const FVertexID VertexID = MeshDescription.CreateVertex();
			VertexPositions[VertexID] = Vert.Position;
			VertexIndexToVertexID.Add(VertexIndex, VertexID);
		}
		// Create the VertexInstance
		int32 NumIndices = ProcSection->ProcIndexBuffer.Num();
		int32 NumTri = NumIndices / 3;
		TMap<int32, FVertexInstanceID> InduceIndexToVertexInstanceID;
		InduceIndexToVertexInstanceID.Reserve(NumVertex);
		for (int32 InduceIndex = 0; InduceIndex < NumIndices; InduceIndex++)
		{
			const int32 VertexIndex = ProcSection->ProcIndexBuffer[InduceIndex];
			const FVertexID VertexID = VertexIndexToVertexID[VertexIndex];
			const FVertexInstanceID VertexInstanceID =
				MeshDescription.CreateVertexInstance(VertexID);
			InduceIndexToVertexInstanceID.Add(InduceIndex, VertexInstanceID);

			FProcMeshVertex &ProcVertex = ProcSection->ProcVertexBuffer[VertexIndex];

			DescriptionTangents[VertexInstanceID] = ProcVertex.Tangent.TangentX;
			DescriptionNormals[VertexInstanceID] = ProcVertex.Normal;
			BinomialSigns[VertexInstanceID] =
				ProcVertex.Tangent.bFlipTangentY ? -1.f : 1.f;

			Colors[VertexInstanceID] = FLinearColor(ProcVertex.Color);

			UVs.Set(VertexInstanceID, 0, ProcVertex.UV0);
			UVs.Set(VertexInstanceID, 1, ProcVertex.UV1);
			UVs.Set(VertexInstanceID, 2, ProcVertex.UV2);
			UVs.Set(VertexInstanceID, 3, ProcVertex.UV3);
		}

		// Create the polygons for this section
		for (int32 TriIdx = 0; TriIdx < NumTri; TriIdx++)
		{
			FVertexID VertexIndexes[3];
			TArray<FVertexInstanceID> VertexInstanceIDs;
			VertexInstanceIDs.SetNum(3);

			for (int32 CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
			{
				const int32 InduceIndex = (TriIdx * 3) + CornerIndex;
				const int32 VertexIndex = ProcSection->ProcIndexBuffer[InduceIndex];
				VertexIndexes[CornerIndex] = VertexIndexToVertexID[VertexIndex];
				VertexInstanceIDs[CornerIndex] =
					InduceIndexToVertexInstanceID[InduceIndex];
			}

			// Insert a polygon into the mesh
			MeshDescription.CreatePolygon(PolygonGroupID, VertexInstanceIDs);
		}
	}
	return MeshDescription;
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
					const float SecondNoise = TerrainNoise->GetNoise3D((X+ ComponentLocation.X),(Y+ComponentLocation.Y),0.0); //Current
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

UStaticMesh* UProceduralTerrainComponent::CreateStaticMesh() const
{
	
	UProceduralMeshComponent* ProcMeshComp = ProceduralTerrainMesh;
	if (ProcMeshComp != nullptr)
	{
		const FString NewNameSuggestion = FString(TEXT("ProcMesh"));
		const FString PackageName = FString(TEXT("/Game/Meshes/")) + NewNameSuggestion;
		FString Name;


		const FString UserPackageName =PackageName;
		const FName MeshName =*Name;


		FMeshDescription MeshDescription = BuildANewMeshDescription(ProcMeshComp);

		// If we got some valid data.
		if (MeshDescription.Polygons().Num() > 0)
		{
			// Then find/create it.
			UPackage* Package = CreatePackage(*UserPackageName);
			check(Package);

			// Create StaticMesh object
			UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, MeshName, RF_Public | RF_Standalone);
			StaticMesh->InitResources();

			StaticMesh->LightingGuid = FGuid::NewGuid();

			// Add source to new StaticMesh
			FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
			SrcModel.BuildSettings.bRecomputeNormals = false;
			SrcModel.BuildSettings.bRecomputeTangents = false;
			SrcModel.BuildSettings.bRemoveDegenerates = false;
			SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
			SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
			SrcModel.BuildSettings.bGenerateLightmapUVs = true;
			SrcModel.BuildSettings.SrcLightmapIndex = 0;
			SrcModel.BuildSettings.DstLightmapIndex = 1;
			StaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
			StaticMesh->CommitMeshDescription(0);

			//// SIMPLE COLLISION
			if (!ProcMeshComp->bUseComplexAsSimpleCollision )
			{
				StaticMesh->CreateBodySetup();
				UBodySetup* NewBodySetup = StaticMesh->BodySetup;
				NewBodySetup->BodySetupGuid = FGuid::NewGuid();
				NewBodySetup->AggGeom.ConvexElems = ProcMeshComp->ProcMeshBodySetup->AggGeom.ConvexElems;
				NewBodySetup->bGenerateMirroredCollision = false;
				NewBodySetup->bDoubleSidedGeometry = true;
				NewBodySetup->CollisionTraceFlag = CTF_UseDefault;
				NewBodySetup->CreatePhysicsMeshes();
			}

			//// MATERIALS
			TSet<UMaterialInterface*> UniqueMaterials;
			const int32 NumSections = ProcMeshComp->GetNumSections();
			for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
			{
				UMaterialInterface *Material = ProcMeshComp->GetMaterial(SectionIdx);
				UniqueMaterials.Add(Material);
			}
			// Copy materials to new mesh
			for (auto* Material : UniqueMaterials)
			{
				StaticMesh->StaticMaterials.Add(FStaticMaterial(Material));
			}

			//Set the Imported version before calling the build
			StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

			// Build mesh from source
			StaticMesh->Build(false);
			StaticMesh->PostEditChange();

			// Notify asset registry of new asset
			FAssetRegistryModule::AssetCreated(StaticMesh);


			return StaticMesh;
		}
	}
	return nullptr;
}




