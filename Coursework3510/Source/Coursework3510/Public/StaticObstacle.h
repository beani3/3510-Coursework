#pragma once
#include "CoreMinimal.h"
#include "ObstaclesBase.h"
#include "StaticObstacle.generated.h"

/**
 * Static Obstacles are derived from Obstacle Base.
 * They do not move and have no physics simulation enabled.
 * Can apply damage to the player on collision.
 */
UCLASS()
class COURSEWORK3510_API AStaticObstacle : public AObstaclesBase
{
	GENERATED_BODY()
	
public:
	AStaticObstacle();
};
