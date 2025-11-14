// PowerupBox.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerupBox.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class UPowerItemDef;           


// powerup box actor moved over from pure blueprint to C++
UCLASS()
class COURSEWORK3510_API APowerupBox : public AActor
{
	GENERATED_BODY()

public:
	APowerupBox();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	//default root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultRoot;

	//sphere collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Sphere;

	//static mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	//rotating movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URotatingMovementComponent> RotatingMovement;

	// array of possible powerup definitions to randomize from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	TArray<TObjectPtr<UPowerItemDef>> PickUpPool;

	// current powerup definition
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	TObjectPtr<UPowerItemDef> CurrentDef;

	// time in seconds before respawning after being consumed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "0"))
	float RespawnTime = 5.f;

	// whether the pickup is currently consumed
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	bool bIsConsumed = false;

	// whether the pickup is fixed not pooled
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	bool bIsFixed = false;

	// sphere collision radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Collision", meta = (ClampMin = "0"))
	float SphereRadius = 100.f;

	// rotation settings
	//spin rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Rotation")
	FRotator SpinRate = FRotator( 20.f, 180.f, 5.332421f);
	
	//whether to spin in local space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Rotation")
	bool bSpinInLocalSpace = true;

	//pivot offset for spinning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Rotation")
	FVector SpinPivot = FVector::ZeroVector;

	//whether to only spin when rendered
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Rotation")
	bool bSpinOnlyWhenRendered = false;

	//events for on consumed
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnConsumedBP();

	//events for on respawned
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnRespawnedBP();

	//consume the pickup
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void Consume();

	//respawn the pickup
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void Respawn();

	//randomize the pickup from the pool
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void RandomizeFromPool();

private:
	// timer handle for respawn
	FTimerHandle TimerHandle_Respawn;


	// overlap begin function
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	// apply rotation settings
	void ApplyRotationSettings() const;
	// set pickup visibility
	void SetPickupVisible(bool bVisible) const;
	// set pickup collision
	void SetPickupCollision(bool bEnable) const;
};
