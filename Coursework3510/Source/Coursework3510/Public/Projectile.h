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
	// Initialize from definition
	void InitFromDef(const UProjectileDef* Def, AActor* InInstigator, USceneComponent* HomingTarget);

protected:
	virtual void BeginPlay() override;

	// collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* Collision = nullptr;

	// mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh = nullptr;

	// movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* Move = nullptr;


	// definition data
	UPROPERTY() const UProjectileDef* Data = nullptr;
	// instigator actor
	UPROPERTY() AActor* InstigatorActor = nullptr;

	// bounce count
	int32 BounceCount = 0;

	// handle hit event
	UFUNCTION() void OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);
	// handle bounce event
	UFUNCTION() void OnBounce(const FHitResult& Impact, const FVector& Vel);

	// check if should bounce off the other actor
	bool ShouldBounceOff(const AActor* Other) const;
	// destroy the projectile
	void Die();
};