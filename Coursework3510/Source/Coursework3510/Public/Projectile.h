// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "ItemTypes.h"
#include "ProjectileDef.h"
#include "Projectile.generated.h"

UCLASS()
class COURSEWORK3510_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	void InitFromDef(const UProjectileDef* Def, AActor* InInstigator, USceneComponent* HomingTarget);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* Collision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* Move = nullptr;

	UPROPERTY() const UProjectileDef* Data = nullptr;
	UPROPERTY() AActor* InstigatorActor = nullptr;

	int32 BounceCount = 0;

	UFUNCTION() void OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION() void OnBounce(const FHitResult& Impact, const FVector& Vel);

	bool ShouldBounceOff(const AActor* Other) const;
	void Die();
};