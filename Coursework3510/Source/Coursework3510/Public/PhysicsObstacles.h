#pragma once

#include "CoreMinimal.h"
#include "ObstaclesBase.h"
#include "PhysicsObstacles.generated.h"

/** Physics-based obstacle; base sets everything up */
UCLASS()
class COURSEWORK3510_API APhysicsObstacles : public AObstaclesBase
{
	GENERATED_BODY()

public:
	APhysicsObstacles();
};
