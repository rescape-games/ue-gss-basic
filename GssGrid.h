#pragma once

#include "CoreMinimal.h"
#include "GssGrid.generated.h"

USTRUCT(BlueprintType)
struct FTile
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	bool bIsValid;

	FTile()
		: bIsValid(false)
	{
	}
};

UCLASS()
class GSSBASIC_API AGssGrid : public AActor
{
	GENERATED_BODY()

public:    
	AGssGrid();

protected:
	virtual void BeginPlay() override;

public:    
	virtual void Tick(float DeltaTime) override;
	
	void DebugTileLocation(const FVector& StartLocation, const FVector& EndLocation, const FHitResult& HitResult, bool bHit);

	FHitResult PerformLineTrace(UObject* StartObject, float TraceDistance, bool bDebug);
	
	void SetPreviewObjectLocation(UObject* PreviewObject, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	FTile GetNearestTile(
		UObject* TraceRefObject = nullptr,
		UObject* PreviewObject = nullptr,
		FVector2D GridBounds = FVector2D(20000.0f, 20000.0f),
		int32 TileSize = 100,
		float TraceDistance = 10000.0f,
		bool bSnapToX = true,
		bool bSnapToY = true,
		bool bSnapToZ = true,
		bool bDebug = false);
};
