#pragma once

#include "CoreMinimal.h"
#include "ObstaclesBase.h"
#include "StaticObstacle.generated.h"

/** Non-simulating (static) obstacle that still uses mesh collision */
UCLASS()
class COURSEWORK3510_API AStaticObstacle : public AObstaclesBase
{
	GENERATED_BODY()

public:
	AStaticObstacle();
};
