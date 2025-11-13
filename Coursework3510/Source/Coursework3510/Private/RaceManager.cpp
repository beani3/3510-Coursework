
#include "RaceManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "PS_PlayerState.h"

ARaceManager::ARaceManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ARaceManager::BeginPlay()
{
	Super::BeginPlay();
	
}

