// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstaclesBase.h"
#include "PhysicsObstacles.generated.h"

UCLASS()
class COURSEWORK3510_API APhysicsObstacles : public AObstaclesBase
{
	GENERATED_BODY()

public:
	APhysicsObstacles();

protected:
	// Scale applied to incoming NormalImpulse to convert to an impulse applied to this obstacle's mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
	float ImpulseScale = 1.0f;

	virtual void HandlePostEffectsOnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FVector& NormalImpulse, const FHitResult& Hit) override;
};
