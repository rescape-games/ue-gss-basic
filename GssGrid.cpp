#include "GssGrid.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"

AGssGrid::AGssGrid()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGssGrid::BeginPlay()
{
	Super::BeginPlay();
}

void AGssGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGssGrid::DebugTileLocation(const FVector& StartLocation, const FVector& EndLocation, const FHitResult& HitResult, const bool bHit)
{
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Cyan, false, 0.1f);
	}
	else
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::FromHex("FF2E70"), false, 0.1f, 0, 1.0f);
	}
}

FHitResult AGssGrid::PerformLineTrace(UObject* StartObject, const float TraceDistance, bool bDebug)
{
    if (!StartObject)
    {
        return FHitResult();
    }

    FVector StartLocation;
    FRotator StartRotation;

    if (const AActor* Actor = Cast<AActor>(StartObject))
    {
        StartLocation = Actor->GetActorLocation();
        StartRotation = Actor->GetActorRotation();
    }
    else if (const USceneComponent* SceneComponent = Cast<USceneComponent>(StartObject))
    {
        StartLocation = SceneComponent->GetComponentLocation();
        StartRotation = SceneComponent->GetComponentRotation();
    }
    else if (const UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(StartObject))
    {
        StartLocation = StaticMesh->GetComponentLocation();
        StartRotation = StaticMesh->GetComponentRotation();
    }
    else if (const UCameraComponent* CameraComponent = Cast<UCameraComponent>(StartObject))
    {
        StartLocation = CameraComponent->GetComponentLocation();
        StartRotation = CameraComponent->GetComponentRotation();
    }
    else if (const ACharacter* Character = Cast<ACharacter>(StartObject))
    {
        StartLocation = Character->GetActorLocation();
        StartRotation = Character->GetActorRotation();
    }
    else if (const APawn* Pawn = Cast<APawn>(StartObject))
    {
        StartLocation = Pawn->GetActorLocation();
        StartRotation = Pawn->GetActorRotation();
    }
    else
    {
        return FHitResult();
    }

    const FVector ForwardVector = StartRotation.Vector();
    const FVector EndLocation = StartLocation + (ForwardVector * TraceDistance);

    FHitResult HitResult;
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(Cast<AActor>(StartObject));

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        TraceParams
    );

	if (bDebug)
		DebugTileLocation(StartLocation, EndLocation, HitResult, bHit);

    return HitResult;
}

void AGssGrid::SetPreviewObjectLocation(UObject* PreviewObject, const FVector& Location)
{
	if (AActor* Actor = Cast<AActor>(PreviewObject))
	{
		Actor->SetActorLocation(Location);
	}
	else if (USceneComponent* SceneComponent = Cast<USceneComponent>(PreviewObject))
	{
		SceneComponent->SetWorldLocation(Location);
	}
	else if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(PreviewObject))
	{
		StaticMesh->SetWorldLocation(Location);
	}
	else if (UCameraComponent* CameraComponent = Cast<UCameraComponent>(PreviewObject))
	{
		CameraComponent->SetWorldLocation(Location);
	}
	else if (ACharacter* Character = Cast<ACharacter>(PreviewObject))
	{
		Character->SetActorLocation(Location);
	}
	else if (APawn* Pawn = Cast<APawn>(PreviewObject))
	{
		Pawn->SetActorLocation(Location);
	}
}

FTile AGssGrid::GetNearestTile(
	UObject* TraceRefObject,
	UObject* PreviewObject,
	FVector2D GridBounds,
	int32 TileSize,
	float TraceDistance,
	bool bSnapToX,
	bool bSnapToY,
	bool bSnapToZ,
	bool bDebug)
{
	const FHitResult HitResult = PerformLineTrace(TraceRefObject, TraceDistance, bDebug);

	FTile Tile;
	
	if (HitResult.bBlockingHit)
	{
		const FVector Location = HitResult.Location - FVector(TileSize / 2);
		
		Tile.Location = Location;

		if (bSnapToX)
		{
			Tile.Location.X = FMath::RoundToInt(Location.X / TileSize) * TileSize;
		}

		if (bSnapToY)
		{
			Tile.Location.Y = FMath::RoundToInt(Location.Y / TileSize) * TileSize;
		}

		if (bSnapToZ)
		{
			Tile.Location.Z = FMath::RoundToInt(Location.Z / TileSize) * TileSize;
		}

		const float HalfGridWidth = GridBounds.X / 2.0f;
		const float HalfGridHeight = GridBounds.Y / 2.0f;

		Tile.Location.X = FMath::Clamp(Tile.Location.X, -HalfGridWidth, HalfGridWidth);
		Tile.Location.Y = FMath::Clamp(Tile.Location.Y, -HalfGridHeight, HalfGridHeight);

		Tile.Location += FVector(TileSize / 2, TileSize / 2, 0);
		Tile.bIsValid = true;

		if (PreviewObject)
		{
			SetPreviewObjectLocation(PreviewObject, Tile.Location);
		}
	}
	
	return Tile;
}
