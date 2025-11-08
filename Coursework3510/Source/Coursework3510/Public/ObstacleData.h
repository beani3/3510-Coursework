#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "Materials/MaterialInterface.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ObstacleData.generated.h"

USTRUCT(BlueprintType) // Health tuning for obstacles
struct FObstacleHealthTuning
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool  bHasHealth = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth")) float MaxHealth = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth")) bool  bCanDie = true;

	// Optional regen
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth")) bool  bAutoRegen = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth")) float RegenPerSecond = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth")) float RegenDelay = 3.f;

	// Impact damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth"))
	float ImpactDamageScale = 0.001f;
};

UCLASS(BlueprintType)
class UObstacleData : public UDataAsset
{
	GENERATED_BODY()
public:

	/** Visuals */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Visual")
	UStaticMesh* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Visual")
	TArray<UMaterialInterface*> OverrideMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Transform")
	FVector  RelativeScale3D = FVector(1.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Transform")
	FRotator RelativeRotation = FRotator::ZeroRotator;

	/** Collision/Physics */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Physics")
	FName CollisionProfileName = TEXT("PhysicsActor");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Physics")
	bool bSimulatePhysics = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Physics")
	TEnumAsByte<EComponentMobility::Type> Mobility = EComponentMobility::Movable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Physics")
	bool bOverrideMass = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bOverrideMass"), Category = "Obstacle|Physics")
	float MassInKg = 1.f;

	/** Feedback (Hit) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Feedback")
	UNiagaraSystem* HitVFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Feedback")
	USoundBase* HitSFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.01"), Category = "Obstacle|Feedback")
	float VFXScale = 1.f;

	/** Feedback (Death) — optional; falls back to HitVFX/HitSFX when null */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Feedback")
	UNiagaraSystem* DeathVFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Feedback")
	USoundBase* DeathSFX = nullptr;

	/** If <= 0, reuses VFXScale */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0"), Category = "Obstacle|Feedback")
	float DeathVFXScale = 0.f;

	/** Health */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	FObstacleHealthTuning Health;

	/** Death behavior */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Death")
	bool bDestroyOnDeath = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "!bDestroyOnDeath"), Category = "Obstacle|Death")
	UStaticMesh* BrokenMesh = nullptr;

	/** Scoring (awarded to the actor that hits/kills this obstacle) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Score")
	int32 PointsOnHit = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Score")
	FName PointsReason_Hit = TEXT("HitObstacle");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Score")
	int32 PointsOnKill = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Score")
	FName PointsReason_Kill = TEXT("DestroyedObstacle");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Score")
	float PointsOnHitCooldownSeconds = 0.5f;          // time between point awards for same actor

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle|Score")
	float MinImpulseForHitPoints = 0.f;               // min impulse magnitude required for hit points
};
