// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppMemberFunctionMayBeStatic
#include "ProceduralTerrainGen.h"

#include "DrawDebugHelpers.h"
#include "ProceduralFoliageSpawner.h"
#include "ProceduralResourceParent.h"
#include "Foliage/Public/ProceduralFoliageComponent.h"

// Sets default values
AProceduralTerrainGen::AProceduralTerrainGen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//Creating a scene component for all the terrain to be relatively located around
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultScene"));

	//Procedural Terrain Component
	if(!ProceduralTerrain)
	{
		ProceduralTerrain = CreateDefaultSubobject<UProceduralTerrainComponent>(TEXT("ProceduralTerrain"));
		ProceduralTerrain->ProceduralTerrainMesh->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	}
	//Procedural Water Component
	if(!ProceduralWater)
	{
		ProceduralWater=CreateDefaultSubobject<UProceduralWaterComponent>(TEXT("ProceduralWater"));
		ProceduralWater->ProceduralWaterMesh->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	}
	//Create and attach the trigger box to the root component that is the procedural mesh
	TerrainTriggerBox=CreateDefaultSubobject<UBoxComponent>(TEXT("TerrainTriggerBox"));
	TerrainTriggerBox->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	TerrainTriggerBox->SetCollisionProfileName(FName(TEXT("Trigger")));
	TerrainTriggerBox->SetGenerateOverlapEvents(true);

}

void AProceduralTerrainGen::DestroyAllAttachedActors()
{
	for (AProceduralActorParent* ProceduralActor : AllProceduralActors)
	{
		if(ProceduralActor)
			ProceduralActor->Destroy();
	}
}


void AProceduralTerrainGen::DrawNatureSquares()
{
	for(TArray<FNatureSquares>& RowOfSquares : NatureSquares)
	{
		for(FNatureSquares& Square : RowOfSquares)
		{
			FVector Pos = Square.CenterLocation +GetActorLocation();
			FVector BoxExtent = FVector(Square.DistanceFromCenterToEdge);
			if(Square.bUsed)
			{
				DrawDebugBox(GetWorld(),Pos,BoxExtent,ColorInUse,bPersistentLines,DebugDrawTime,DepthPriority,DebugBoxThickness);
			}
			else if(Square.bShade)
			{
				DrawDebugBox(GetWorld(),Pos,BoxExtent,ColorInShade,bPersistentLines,DebugDrawTime,DepthPriority,DebugBoxThickness);
			}
			else
			{
				DrawDebugBox(GetWorld(),Pos,BoxExtent,ColorNoFoliageOrShade,bPersistentLines,DebugDrawTime,DepthPriority,DebugBoxThickness);
			}
		}
	}
	
}

void AProceduralTerrainGen::GenerateTerrain()
{
	//Generate the Terrain
	GenerateCurrentLandscape();

	//Generate Nature
	GenerateNatureInternal();

	//After we generate the landscape we want to find any and all connected landscapes
	FindConnections(this);
}



void AProceduralTerrainGen::GenerateNatureInternal()
{
	if(bGenerateNature)
	{
		//Find the Squares for the mesh	
		InitNatureSquares();

		FRandomStream RandomNumberGenerator;
	
		for(const FNatureInfo& BiomeNature :  BiomeFoliage)
		{
			uint32 NumMeshes;
			
			if(BiomeNature.bSpawnBlueprintClass)
			{
				NumMeshes = BiomeNature.SpawningActorClasses.Num();
			}
			else
			{
				NumMeshes = BiomeNature.Meshes.Num();
			}
			//Check to see if we are spawning in any meshes 
			
			if(NumMeshes ==0) continue;

			//Create the seed for our number stream
			const int RandomizeSeed = ProceduralTerrain->GetValueFromNoise(FVector(NoiseComponentStartLocation.X,NoiseComponentStartLocation.Y,BiomeNature.Seed)) * 100000;
			RandomNumberGenerator.Initialize(RandomizeSeed);

			//Determine how many meshes to spawn
			const uint32 MeshesToSpawn = RandomNumberGenerator.RandRange(BiomeNature.MinMeshesToSpawn,BiomeNature.MaxMeshesToSpawn);

			//Get our square candidates for spawning in a mesh
			TArray<int32> SquareCandidateIndexes;
			GetSquareCandidates
			(
				BiomeNature.HeightPercentageRangeToLocateNatureMeshes,
				MeshesToSpawn,
				BiomeNature.CorrespondingBiome,
				RandomNumberGenerator,
				SquareCandidateIndexes,
				BiomeNature.bCanGrowInShade,
				BiomeNature.bOnlyGrowInShade
			);


			// If there are no triangle candidates go to the next Biome Nature Structure
			const uint32 NumSquareCandidates = SquareCandidateIndexes.Num();
			if (NumSquareCandidates == 0) continue;

			//Determine what rotation the Biome Nature needs
			GetBiomeRotationFuncPtr GetBiomeElementRotationFunction = nullptr;
			switch (BiomeNature.RotationType)
			{
			case ENatureRotationTypes::Random:
				{
					GetBiomeElementRotationFunction = &AProceduralTerrainGen::GetBiomeRandomRotation;
					break;
				}

			case ENatureRotationTypes::TerrainShapeNormal:
				{
					GetBiomeElementRotationFunction = &AProceduralTerrainGen::GetBiomePlaneShapeRotation;
					break;
				}
			case ENatureRotationTypes::MeshSurfaceNormal:
				{
					GetBiomeElementRotationFunction = &AProceduralTerrainGen::GetBiomeMeshSurfaceRotation;
					break;
				}
			default:
				{}
			}

			
			const uint32 NumMeshesLessOne = NumMeshes - 1;
			const uint32 NumSquareCandidatesLessOne = NumSquareCandidates - 1;
			const float MinScale = BiomeNature.MinMaxScale.X;
			const float MaxScale = BiomeNature.MinMaxScale.Y;
			const TArray<UStaticMesh*>& Meshes = BiomeNature.Meshes;
			const int32 CullDistance = BiomeNature.CullDistance;
			FTransform MeshTransform;


			// Iterate through every mesh to generate, giving him a new random location, scale and rotation if needed
			for (uint32 NewMeshIndex = 0; NewMeshIndex < MeshesToSpawn; NewMeshIndex++)
			{
				//If we reach the end of our candidates we break out of trying to spawn more because there isn't any locations to spawn at
				if(NewMeshIndex> NumSquareCandidatesLessOne)
					break;

				FNatureSquares& RandomSquare = GetNatureSquareAtIndex(SquareCandidateIndexes[NewMeshIndex]);

				if(RandomSquare.bShade)
				{
					if(!BiomeNature.bCanGrowInShade)
					{
						continue;
					}
				}

				// Pick a random available location on triangle
				FVector RandomLocation;
				const bool bSuccess = RandomSquare.GetRandomPointOnSquare(RandomNumberGenerator,RandomLocation);
				if(bSuccess)
				{
					RandomLocation.Z += BiomeNature.SpawnHeightOffset;
					MeshTransform.SetLocation(RandomLocation);
				}
				else
				{
					continue;
				}

				// Set rotation
				MeshTransform.SetRotation(((this->*GetBiomeElementRotationFunction)(RandomNumberGenerator, RandomSquare, MeshTransform.GetLocation())));

				// Generate a random mesh scale between a defined min. and max.
				const float RandomScale = RandomNumberGenerator.FRandRange(MinScale, MaxScale);
				MeshTransform.SetScale3D(FVector(RandomScale, RandomScale, RandomScale));

				// Pick a random mesh from the array
				const int32 MeshToPickIndex = RandomNumberGenerator.RandRange(0, NumMeshesLessOne);
				
				//If we are spawning in a actor class
				if(BiomeNature.bSpawnBlueprintClass)
				{
					//Pick the actor class and if successful try to spawn in
					TSubclassOf<AProceduralActorParent> ActorToPick = BiomeNature.SpawningActorClasses[MeshToPickIndex];
					if(ActorToPick == nullptr)	continue;

					MeshTransform.SetLocation(MeshTransform.GetLocation()+GetActorLocation());
					
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner=this;
					SpawnParams.Instigator = GetInstigator();
					
					AProceduralActorParent* SpawnedActor = GetWorld()->SpawnActor<AProceduralActorParent>(ActorToPick,MeshTransform,SpawnParams);

					//IF successfully spawned actor add into array of all procedural actors and do the shading required for the actor
					if(SpawnedActor)
					{
						AllProceduralActors.Push(SpawnedActor);

						//Setting the Square to used
						RandomSquare.bUsed=true;

						//Setting the squares shading
						ApplyMeshShade(BiomeNature,SquareCandidateIndexes[NewMeshIndex]);
					}

				}
				else
				{
					
					UStaticMesh* MeshToPick = Meshes[MeshToPickIndex];
					if (MeshToPick == nullptr) continue;	// if mesh is empty, continue

					//Setting the Square to used
					RandomSquare.bUsed=true;

					//Setting the squares shading
					ApplyMeshShade(BiomeNature,SquareCandidateIndexes[NewMeshIndex]);
				
					// If HISM was found, add new instance
					if (UHierarchicalInstancedStaticMeshComponent** HISM_ref = NatureStaticMeshHISM_Correspondence.Find(MeshToPick))
					{
						(*HISM_ref)->AddInstance(MeshTransform);
					}
					// If there is no corresponding HISM component, create one and store it
					else if (UHierarchicalInstancedStaticMeshComponent* HISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this))
					{
						// The property documentation says that this should be enabled only for small objects without collision, and that's not the case, so we disable it
						HISM->bUseDefaultCollision=1;
						HISM->bEnableDensityScaling = 0;	// false
						HISM->SetStaticMesh(MeshToPick);
						HISM->SetEnableGravity(false);
						HISM->bApplyImpulseOnDamage = false;
						HISM->SetGenerateOverlapEvents(false);
						HISM->SetCullDistances(0, CullDistance);
						HISM->SetMobility(EComponentMobility::Movable);
						HISM->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
						HISM->RegisterComponent();

						// Add the new HISM to the map
						NatureStaticMeshHISM_Correspondence.Emplace(MeshToPick, HISM);

						// Finally, add instance
						HISM->AddInstance(MeshTransform);
					}
				}
			}
			
		}
	}
}

bool IsInsideCircle(const FVector CircleCenter, const FVector TileLocation, const float CircleRadius)
{
	const float Dy = CircleCenter.Y - TileLocation.Y;
	const float Dx = CircleCenter.X - TileLocation.X;
	const float DistanceSquared = Dy * Dy + Dx *Dx;
	const float CircleRadiusSquared = CircleRadius* CircleRadius;
	return DistanceSquared <= CircleRadiusSquared;
}


void AProceduralTerrainGen::ApplyMeshShade(const FNatureInfo& BiomeNature, const int NatureSquareIndex)
{
	int SquareRow;
	int SquareColumn;
	CorrectedSquareIndex(NatureSquareIndex,SquareRow,SquareColumn);
	const FVector CircleLocation = NatureSquares[SquareRow][SquareColumn].CenterLocation;
	const float SquareSize = NatureSquares[SquareRow][SquareColumn].DistanceFromCenterToEdge*2;
	
	if(BiomeNature.ShadingDistance >= SquareSize)
	{
		const int BoundingBoxLength =BiomeNature.ShadingDistance / SquareSize;
		SquareRow -= BoundingBoxLength;
		SquareColumn -= BoundingBoxLength;
		for(int Index=0;Index<=BoundingBoxLength*2+1;Index++)
		{
			if(SquareRow <0)
			{
				SquareRow++;
				continue;
			}
			if(SquareRow >=NumberOfQuadsPerLine)
				break;
			
			for(int Jedex=0; Jedex<=BoundingBoxLength*2+1;Jedex++)
			{
				if(SquareColumn < 0)
				{
					SquareColumn++;
					continue;
				}
				if(SquareColumn>=NumberOfQuadsPerLine)
				{
					break;
				}

				if(IsInsideCircle(CircleLocation,NatureSquares[SquareRow][SquareColumn].CenterLocation,BiomeNature.ShadingDistance))
					NatureSquares[SquareRow][SquareColumn].bShade=true;
				SquareColumn++;
			}
			SquareColumn-=BoundingBoxLength*2+2;
			SquareRow++;
		}
	}
	
}

void AProceduralTerrainGen::FindConnections(AProceduralTerrainGen* CurrentTerrain) const
{
	//We want to look in more of a center location instead of the bottom left corner so that if we overlap with something we know that it is the correct mesh
	FVector StartLocation =CurrentTerrain->GetActorLocation() +FVector(ProceduralTerrain->NoiseResolution,ProceduralTerrain->NoiseResolution,0);
	//If we dont have a north then we look for one
	if(!CurrentTerrain->NorthTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation+FVector(ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0,0),ProceduralTerrain->FastNoiseOutputScale);
		if(Out.IsValidBlockingHit())
		{
			AProceduralTerrainGen* Gen =Cast<AProceduralTerrainGen>(Out.Actor);
			if(Gen)
			{
				CurrentTerrain->NorthTerrainGenerated=Gen;
				if(!Gen->SouthTerrainGenerated)
					Gen->SouthTerrainGenerated=CurrentTerrain;
			}
		}
	}
	//If we dont have a east then we look for one
	if(!CurrentTerrain->EastTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation+FVector(0,ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0),ProceduralTerrain->FastNoiseOutputScale);
		if(Out.IsValidBlockingHit())
		{
			AProceduralTerrainGen* Gen =Cast<AProceduralTerrainGen>(Out.Actor);
			if(Gen)
			{
				CurrentTerrain->EastTerrainGenerated=Gen;
				if(!Gen->WestTerrainGenerated)
					Gen->WestTerrainGenerated=CurrentTerrain;
			}
		}
	}
	//If we dont have a south we look for one
	if(!CurrentTerrain->SouthTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation-FVector(ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0,0),ProceduralTerrain->FastNoiseOutputScale);
		if(Out.IsValidBlockingHit())
		{
			AProceduralTerrainGen* Gen =Cast<AProceduralTerrainGen>(Out.Actor);
			if(Gen)
			{
				CurrentTerrain->SouthTerrainGenerated=Gen;
				if(!Gen->NorthTerrainGenerated)
					Gen->NorthTerrainGenerated=CurrentTerrain;
			}
		}
	}
	//If we dont have a west we look for one
	if(!CurrentTerrain->WestTerrainGenerated)
	{
		FHitResult Out =LineTrace(StartLocation-FVector(0,ProceduralTerrain->TotalSizeToGenerate-ProceduralTerrain->NoiseResolution,0),ProceduralTerrain->FastNoiseOutputScale);
		if(Out.IsValidBlockingHit())
		{
			AProceduralTerrainGen* Gen =Cast<AProceduralTerrainGen>(Out.Actor);
			if(Gen)
			{
				CurrentTerrain->WestTerrainGenerated=Gen;
				if(!Gen->EastTerrainGenerated)
					Gen->EastTerrainGenerated=CurrentTerrain;
			}
		}
	}
}
constexpr float GMin_Angle = 0.0f;
constexpr float GMax_Angle = 359.99f;
constexpr float PI2 = PI * 2.0f;

FQuat AProceduralTerrainGen::GetBiomeRandomRotation(const FRandomStream& RandomNumberGenerator, const FNatureSquares& Square, const FVector& Location) const
{
	return FRotator(RandomNumberGenerator.FRandRange(GMin_Angle, GMax_Angle), RandomNumberGenerator.FRandRange(GMin_Angle, GMax_Angle), RandomNumberGenerator.FRandRange(GMin_Angle, GMax_Angle)).Quaternion();
}

FQuat AProceduralTerrainGen::GetRandomYawQuat(const FRandomStream& RandomNumberGenerator) 
{
	return FQuat(FVector::UpVector, RandomNumberGenerator.FRandRange(0.0f, PI2));
}

FQuat AProceduralTerrainGen::GetBiomePlaneShapeRotation(const FRandomStream& RandomNumberGenerator, const FNatureSquares& Square, const FVector& Location) const
{
	return GetRandomYawQuat(RandomNumberGenerator);
}

FQuat AProceduralTerrainGen::GetBiomeMeshSurfaceRotation(const FRandomStream& RandomNumberGenerator, const FNatureSquares& Square, const FVector& Location) const
{
	return Square.GetSurfaceNormal() * GetRandomYawQuat(RandomNumberGenerator);
}


void AProceduralTerrainGen::HideNature()
{
	//Loop through all the elements of the HISM and hide all the meshes
	for (const auto& Elem : NatureStaticMeshHISM_Correspondence)
	{
		if (UHierarchicalInstancedStaticMeshComponent* HISM = Elem.Value)
		{
			HISM->SetVisibility(false);
		}
	}
	for (AProceduralActorParent* ProceduralActor : AllProceduralActors)
	{
		AProceduralResourceParent* Resource = Cast<AProceduralResourceParent>(ProceduralActor);
		if(Resource)
		{
			Resource->SetVisibility(false);
		}
	}
}

void AProceduralTerrainGen::LoadNature()
{
	//Loop through all the elements of the HISM and make visible all the meshes
	for (const auto& Elem : NatureStaticMeshHISM_Correspondence)
	{
		if (UHierarchicalInstancedStaticMeshComponent* HISM = Elem.Value)
		{
			HISM->SetVisibility(true);
		}
	}
	for (AProceduralActorParent* ProceduralActor : AllProceduralActors)
	{
		AProceduralResourceParent* Resource = Cast<AProceduralResourceParent>(ProceduralActor);
		if(Resource)
		{
			Resource->SetVisibility(true);
		}
	}
}


void AProceduralTerrainGen::GetSquareCandidates(const FVector2D& HeightPercentageRangeToLocateElements,
	const int32 MaxCandidatesToGet, const ENatureBiomes Biome, const FRandomStream& RandomNumberGenerator,
	TArray<int32>& SquareCandidateIndexes,const bool bCanGrowInShade, const bool bOnlyGrowInShade)
{
	
	//Init the heights that are needed
	const float WaterHeight = ProceduralWater->WaterHeight;
	const float TerrainHeight = ProceduralTerrain->FastNoiseOutputScale + ProceduralTerrain->WaterHeightOffset;
	const float LowestTerrainHeight = (-ProceduralTerrain->FastNoiseOutputScale) + ProceduralTerrain->WaterHeightOffset;
	FVector2D HeightRange;

	//Pick the height range desired
	switch (Biome)
	{
	case ENatureBiomes::Earth:
		{
			HeightRange = FVector2D(WaterHeight,TerrainHeight);
			break;
		}
	case ENatureBiomes::Underwater:
		{
			HeightRange =FVector2D(LowestTerrainHeight,WaterHeight);
			break;
		}
	case ENatureBiomes::Both:
		{
			HeightRange =FVector2D(LowestTerrainHeight,TerrainHeight);
			break;
		}
	default: ;
	}

	//Based off of the range clamp the allowed heights
	const FVector2D PercentageRange = FVector2D(0.0,100.0f);
	const float LowestAllowedHeight = FMath::GetMappedRangeValueClamped(PercentageRange, HeightRange, HeightPercentageRangeToLocateElements.X);
	const float HighestAllowedHeight = FMath::GetMappedRangeValueClamped(PercentageRange, HeightRange, HeightPercentageRangeToLocateElements.Y);
	
	const int32 NumSquares = NatureSquares.Num() * NumberOfQuadsPerLine;
	const int32 LastIndex = NumSquares - 1;

	//Creating a array of all the possible indexes
	TArray<int> SquareIndexes = TArray<int>();
	for (int32 i = 0; i < LastIndex; i++)
	{
		SquareIndexes.Add(i);
	}
	
	const bool bHeightCheckNeeded = Biome != ENatureBiomes::Both || HeightPercentageRangeToLocateElements.X != 0.0f || HeightPercentageRangeToLocateElements.Y != 100.0f;
	for (int32 i = 0; i < LastIndex; i++)
	{
		//Grabbing a random available index and getting that square but also making it so we cant get that index again   
		const int32 Index = RandomNumberGenerator.RandRange(i,SquareIndexes.Num()-1);
		if(i != Index)
			SquareIndexes.Swap(i,Index);
		
		const FNatureSquares& Square = GetNatureSquareAtIndex(SquareIndexes[i]);

		if(Square.bUsed)
		{
			continue;
		}

		if(bOnlyGrowInShade)
		{
			if(!Square.bShade)
			{
				continue;
			}
		}

		if(Square.bShade)
		{
			if(!bCanGrowInShade)
			{
				continue;
			}
		}
		

		// If height check is needed...
		if (bHeightCheckNeeded)
		{
			if(!Square.IsInsidePlaneHeightRange(LowestAllowedHeight,HighestAllowedHeight))
			{
				continue;
			}
		}

		// Add triangle index as a valid candidate
		SquareCandidateIndexes.Add(SquareIndexes[i]);

		// If we have enough candidates, exit
		if (SquareCandidateIndexes.Num() == MaxCandidatesToGet)
		{
			break;
		}
	}
	
}


FHitResult AProceduralTerrainGen::LineTrace(FVector StartLocation, float OutputScale) const
 {
	FHitResult OutHit;

	//Make the start and end where the desired location is but with a negative z for the output scale
	//and positive still we use the start location because if we generated the map at a location that is
	//not 0,0,0 then we would not be able to reliably hit it with a line trace so we just take what the max height
	//that the terrain could be and we subtract that from the z and we also add it to the z to keep our line trace relative
	FVector Start = FVector(StartLocation.X,StartLocation.Y,StartLocation.Z-OutputScale);
	FVector End =  FVector(StartLocation.X,StartLocation.Y,StartLocation.Z +OutputScale);
	

	FCollisionQueryParams CollisionParams;

	//Uncomment to get a debug trace for where we are doing the line traces
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 25, 0, 5);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);
	return OutHit;
}


void AProceduralTerrainGen::GenerateCurrentLandscape() const
{
	ProceduralTerrain->GenerateMap(NoiseComponentStartLocation);
	if(bWaterEnabled)
		ProceduralWater->GenerateMap(NoiseComponentStartLocation);
	else
	{
		ProceduralWater->DestroyComponent();
	}
	
}

void AProceduralTerrainGen::InitializeVariable(const FTerrainInfo TerrainInfo, const TArray<FTerrainInfo> ConnectedTerrainInfos) 
{
	//Init the Fast Noise or the terrain map and the Connection noise until I get a better solution for the connection
	ProceduralTerrain->InitializeFastNoise(TerrainInfo);
	ProceduralTerrain->InitializeConnectionNoise(ConnectedTerrainInfos);

	//Creating the trigger box the box extent is just the same size as the map generated. 
	TerrainTriggerBox->SetBoxExtent(FVector((TerrainInfo.TotalSizeToGenerate-TerrainInfo.NoiseResolution)*0.5 ,(TerrainInfo.TotalSizeToGenerate-TerrainInfo.NoiseResolution)*0.5 ,TerrainInfo.NoiseOutputScale * 3));

	//And we need to make it so the box is in the center of the terrain so we off set it by half of the size of the map on x and y
	TerrainTriggerBox->SetRelativeLocation(FVector((TerrainInfo.TotalSizeToGenerate-TerrainInfo.NoiseResolution)*0.5,(TerrainInfo.TotalSizeToGenerate-TerrainInfo.NoiseResolution)*0.5,TerrainInfo.WaterHeight));

	if(TerrainInfo.WaterPercentage != 0)
	{
		ProceduralWater->InitializeWaterComponent(TerrainInfo);
		bWaterEnabled=true;
	}

	if(TerrainInfo.BiomeNatures.Num()>0)
	{
		BiomeFoliage = TerrainInfo.BiomeNatures;
		bGenerateNature=true;
	}
}

void AProceduralTerrainGen::LoadTerrain() 
{
	if(bWaterEnabled)
		ProceduralWater->LoadMesh();
	ProceduralTerrain->LoadMesh();
	TerrainTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LoadNature();
	
}

void AProceduralTerrainGen::UnloadTerrain()
{
	if(bWaterEnabled)
		ProceduralWater->UnLoadMesh();
	ProceduralTerrain->UnLoadMesh();
	TerrainTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HideNature();
	
}
