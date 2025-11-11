// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbySpawnPoint.h"
#include "Components/ArrowComponent.h"

// Sets default values
ALobbySpawnPoint::ALobbySpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(FName(TEXT("PlayerSpawnTransform")));
	ArrowComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALobbySpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALobbySpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbySpawnPoint::SpawnPlayer()
{

	FActorSpawnParameters SpawnParameters;

	if (IsValid(PlayerActor))
	{
		PlayerActor->Destroy();
	}

	PlayerActor = GetWorld()->SpawnActor<AActor>(SpawnActor, ArrowComponent->GetComponentLocation(), ArrowComponent->GetComponentRotation(), SpawnParameters);
}

void ALobbySpawnPoint::DespawnPlayer()
{

	if (IsValid(PlayerActor))
	{
		PlayerActor->Destroy();
		PlayerActor = nullptr;
		PlayerController = nullptr;
	}

}

