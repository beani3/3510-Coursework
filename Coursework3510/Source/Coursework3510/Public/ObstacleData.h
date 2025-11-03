#pragma once
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

    //Optional regen
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth")) bool  bAutoRegen = false;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth")) float RegenPerSecond = 5.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth")) float RegenDelay = 3.f;

    // Impact damage
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasHealth"))
    float ImpactDamageScale = 0.001f;
};

UCLASS(BlueprintType) // Data asset for obstacle configuration
class UObstacleData : public UDataAsset
{
    GENERATED_BODY()
public:
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UStaticMesh* Mesh = nullptr; // Mesh asset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<UMaterialInterface*> OverrideMaterials; // Optional material overrides

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FVector  RelativeScale3D = FVector(1); // Transform overrides
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FRotator RelativeRotation = FRotator::ZeroRotator; // Rotation

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName CollisionProfileName = TEXT("BlockAllDynamic"); // Collision profile
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool  bSimulatePhysics = false; // Physics simulation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TEnumAsByte<EComponentMobility::Type> Mobility = EComponentMobility::Static; // Mobility type
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool  bOverrideMass = false; // Mass override
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bOverrideMass")) float MassInKg = 50.f; // Mass value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UPhysicalMaterial* PhysMatOverride = nullptr; // Physical material override

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UNiagaraSystem* HitVFX = nullptr; // Hit VFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) USoundBase* HitSFX = nullptr; // Hit SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.01")) float VFXScale = 1.f; // VFX scale

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FObstacleHealthTuning Health; // Health tuning

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bDestroyOnDeath = true;// Destroy actor on death
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UStaticMesh* BrokenMesh = nullptr; // optional swap
};
