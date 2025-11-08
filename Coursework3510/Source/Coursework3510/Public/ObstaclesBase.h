#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstaclesBase.generated.h"

class UStaticMeshComponent;
class UObstacleData;
class UAC_HealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObstacleDamagedSig, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FObstacleDiedSig);

UCLASS()
class COURSEWORK3510_API AObstaclesBase : public AActor
{
	GENERATED_BODY()

public:
	AObstaclesBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle")
	UObstacleData* Data = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Obstacle")
	void ApplyData(bool bApplyTransformOffsets);

	UFUNCTION(BlueprintCallable, Category = "Obstacle|Combat")
	virtual void ApplyDamage(float Amount, AActor* DamageInstigator = nullptr);

	UFUNCTION(BlueprintPure, Category = "Obstacle|Combat")
	float GetHealth() const { return CachedHealth; }

	UPROPERTY(BlueprintAssignable, Category = "Obstacle|Combat")
	FObstacleDamagedSig OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Obstacle|Combat")
	FObstacleDiedSig OnDied;

protected:
	/** Mesh = root, ONLY physics/collision body */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obstacle")
	UStaticMeshComponent* Mesh = nullptr;

	/** Health component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obstacle")
	UAC_HealthComponent* HealthComp = nullptr;

	/** Apply DA scale/rotation in Construction for preview (not at BeginPlay) */
	UPROPERTY(EditAnywhere, Category = "Obstacle|Setup")
	bool bApplyDAVisualOffsetsInConstruction = true;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	/** Feedback helpers */
	void PlayHitFeedback(const FVector& Where);
	void PlayDeathFeedback(const FVector& Where);

	/** Destroy/swap mesh on death */
	virtual void OnDied_Handle();

	/** Physics hit callback */
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	/** Health component events */
	UFUNCTION()
	void OnHealthChanged(float NewHealth, float Delta);

	UFUNCTION()
	void OnDied_FromHealth();

	/** Scoring helper: give points to a receiver (interface first, then component) */
	void AwardPoints(AActor* Receiver, int32 Amount, FName Reason, AActor* Causer);

protected:
	float CachedHealth = 0.f;

	/** Track who last caused damage (for kill credit) */
	UPROPERTY()
	TWeakObjectPtr<AActor> LastDamageInstigator;
	
	
	// Internal record of last time we gave points to each OtherActor
	TMap<TWeakObjectPtr<AActor>, double> LastHitScoreTime;
};
