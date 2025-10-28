// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemTypes.h"
#include "ProjectileDef.h"
#include "AC_ProjectileComponent.generated.h"

class AProjectile;
class USceneComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURSEWORK3510_API UAC_ProjectileComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAC_ProjectileComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/** Projectile class to spawn (AProjectile or BP child) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

	/** Fallback local offset if no muzzle is set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FTransform MuzzleOffset;

	/** Explicitly set the muzzle from BP (BeginPlay of your kart) */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetMuzzle(USceneComponent* InMuzzle) { MuzzleComponent = InMuzzle; }

	/** Fire from the muzzle (or fallback) */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	bool FireByDef(const UProjectileDef* Def, USceneComponent* HomingTarget = nullptr);

private:
	UPROPERTY() USceneComponent* MuzzleComponent = nullptr; // cached after SetMuzzle

	USceneComponent* ResolveMuzzle() const;
	FTransform BuildSpawnTM() const;
};

