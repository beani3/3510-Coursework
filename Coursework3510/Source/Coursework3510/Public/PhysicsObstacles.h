#pragma once
#include "CoreMinimal.h"
#include "ObstaclesBase.h"
#include "PhysicsObstacles.generated.h"


/**
 * Physics Obstacles are derived from Obstacle Base.
 * They have physics simulation enabled and can move when impacted.
 * Can apply damage to the player on collision.
 * Impulse can be scaled
 */
UCLASS()
class COURSEWORK3510_API APhysicsObstacles : public AObstaclesBase
{
	GENERATED_BODY()

public:
	APhysicsObstacles();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics") // Scale of the impulse applied on hit
	float ImpulseScale = 1.0f;

	virtual void HandlePostEffectsOnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FVector& NormalImpulse, const FHitResult& Hit) override; //Handles effects after the hit
};
