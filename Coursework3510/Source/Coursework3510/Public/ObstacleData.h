// ObstacleData.h
#pragma once
#include "Engine/DataAsset.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "Materials/MaterialInterface.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ObstacleData.generated.h"

USTRUCT(BlueprintType)
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

    // Impact damage (useful for cones/barrels)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth"))
    float ImpactDamageScale = 0.001f; // Damage = NormalImpulse.Size() * this
};

UCLASS(BlueprintType)
class UObstacleData : public UDataAsset
{
    GENERATED_BODY()
public:
    // VISUALS
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UStaticMesh* Mesh = nullptr;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<UMaterialInterface*> OverrideMaterials;

    // TRANSFORM OFFSETS (optional)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FVector  RelativeScale3D = FVector(1);
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FRotator RelativeRotation = FRotator::ZeroRotator;

    // COLLISION/PHYSICS
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName CollisionProfileName = TEXT("BlockAllDynamic");
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool  bSimulatePhysics = false;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TEnumAsByte<EComponentMobility::Type> Mobility = EComponentMobility::Static;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool  bOverrideMass = false;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bOverrideMass")) float MassInKg = 50.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UPhysicalMaterial* PhysMatOverride = nullptr;

    // FX
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UNiagaraSystem* HitVFX = nullptr;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) USoundBase* HitSFX = nullptr;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.01")) float VFXScale = 1.f;

    // HEALTH
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FObstacleHealthTuning Health;

    // DEATH RESPONSE
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bDestroyOnDeath = true;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UStaticMesh* BrokenMesh = nullptr; // optional swap
};
