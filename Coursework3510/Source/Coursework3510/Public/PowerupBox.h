// PowerupBox.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerupBox.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class UPowerItemDef;           

UCLASS()
class COURSEWORK3510_API APowerupBox : public AActor
{
	GENERATED_BODY()

public:
	APowerupBox();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URotatingMovementComponent> RotatingMovement;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	TArray<TObjectPtr<UPowerItemDef>> PickUpPool;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	TObjectPtr<UPowerItemDef> CurrentDef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "0"))
	float RespawnTime = 5.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	bool bIsConsumed = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	bool bIsFixed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Collision", meta = (ClampMin = "0"))
	float SphereRadius = 100.f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Rotation")
	FRotator SpinRate = FRotator( 20.f, 180.f, 5.332421f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Rotation")
	bool bSpinInLocalSpace = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Rotation")
	FVector SpinPivot = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Rotation")
	bool bSpinOnlyWhenRendered = false;

	
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnConsumedBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnRespawnedBP();

	
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void Consume();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void Respawn();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void RandomizeFromPool();

private:
	FTimerHandle TimerHandle_Respawn;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void ApplyRotationSettings() const;
	void SetPickupVisible(bool bVisible) const;
	void SetPickupCollision(bool bEnable) const;
};
