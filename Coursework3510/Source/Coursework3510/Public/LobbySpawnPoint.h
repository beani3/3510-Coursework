// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbySpawnPoint.generated.h"

class UArrowComponent;

UCLASS()
class COURSEWORK3510_API ALobbySpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbySpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UArrowComponent* ArrowComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "LobbySpawnPoint")
	void SpawnPlayer();

	UFUNCTION(BlueprintCallable, Category = "LobbySpawnPoint")
	void DespawnPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LobbySpawnPoint")
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LobbySpawnPoint")
	AActor* PlayerActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LobbySpawnPoint")
	TSubclassOf<AActor> SpawnActor;
};
