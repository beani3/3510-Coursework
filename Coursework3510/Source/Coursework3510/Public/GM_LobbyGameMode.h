// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_LobbyGameMode.generated.h"

/**
 * 
 */

class ALobbySpawnPoint;

UCLASS()
class COURSEWORK3510_API AGM_LobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	


public:
	virtual void BeginPlay() override;

	FTimerHandle AddLobbyPCTimerHandle;
	void AddLobbyPCTimer();

	UPROPERTY(EditAnywhere)
	TArray<ALobbySpawnPoint*> LobbySpawnPoints;

	UPROPERTY(EditAnywhere)
	TArray<APlayerController*> LobbyPlayerControllers;

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void AddLobbyPlayerController(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RemoveLobbyPlayerController(APlayerController* PlayerController);
};
