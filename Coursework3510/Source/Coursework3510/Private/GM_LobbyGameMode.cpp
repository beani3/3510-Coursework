// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_LobbyGameMode.h"
#include "LobbySpawnPoint.h"
#include "Kismet/GameplayStatics.h"

void AGM_LobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbySpawnPoint::StaticClass(), OutActors);
	for (AActor* FoundActor : OutActors)
	{
		LobbySpawnPoints.Add(Cast<ALobbySpawnPoint>(FoundActor));
	}
}

void AGM_LobbyGameMode::AddLobbyPlayerController(APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		LobbyPlayerControllers.Add(PlayerController);

		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Yellow, TEXT("Player Controller Logged in"), true);

		GetWorld()->GetTimerManager().SetTimer(AddLobbyPCTimerHandle, this, &AGM_LobbyGameMode::AddLobbyPCTimer, 1.0f,
			false);
	}
}

void AGM_LobbyGameMode::AddLobbyPCTimer()
{
	if (LobbySpawnPoints.Num() >= LobbyPlayerControllers.Num())
	{
		LobbySpawnPoints[LobbyPlayerControllers.Num() - 1]->SpawnPlayer();
	}
}

void AGM_LobbyGameMode::RemoveLobbyPlayerController(APlayerController* PlayerController)
{
	LobbyPlayerControllers.Remove(PlayerController);
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Yellow, TEXT("Player Controller Logged out"), true);
}
