// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileDef.h"
#include "AC_ProjectileComponent.generated.h"

class AProjectile;
class USceneComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COURSEWORK3510_API UAC_ProjectileComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAC_ProjectileComponent();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    FTransform MuzzleOffset;

    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void SetMuzzle(USceneComponent* InMuzzle) { MuzzleComponent = InMuzzle; }

    UFUNCTION(BlueprintCallable, Category = "Projectile")
    bool FireByDef(const UProjectileDef* Def, USceneComponent* HomingTarget = nullptr);

private:
    UFUNCTION(Server, Reliable)
    void ServerFireByDef(const UProjectileDef* Def, USceneComponent* HomingTarget = nullptr);

    UPROPERTY()
    USceneComponent* MuzzleComponent = nullptr;

    USceneComponent* ResolveMuzzle() const;
    FTransform BuildSpawnTM() const;
    void PlayMuzzleFX(const FTransform& SpawnTM);

    USceneComponent* PickAheadHomingTarget() const;
};
