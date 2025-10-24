// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ItemTypes.h"
#include "Projectile.generated.h"

UCLASS()
class COURSEWORK3510_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	void InitFromItemRow(const FProjectileRow& Row, AActor* InInstigator, USceneComponent* HomingTarget);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* Move;

	FProjectileRow Data;
	UPROPERTY() AActor* InstigatorActor = nullptr;
	int32 BounceCount = 0;

	UFUNCTION() void OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION() void OnBounce(const FHitResult& Impact, const FVector& Vel);

	bool ShouldBounceOff(const AActor* Other) const;
	void Die();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
