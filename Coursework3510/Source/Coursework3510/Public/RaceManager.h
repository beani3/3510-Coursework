/*
Thid id the RaceManager class, and I want it to manage the finish screen for each player, displaying times and positions. Whilst also handling
the amounbt of laps needed to finish, and a courtesy time after the first player finishes to allow others to complete
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaceManager.generated.h"


//
// old no lomger used race manager class
//
UCLASS()
class COURSEWORK3510_API ARaceManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ARaceManager();




protected:
	virtual void BeginPlay() override;


public:	
	
};
