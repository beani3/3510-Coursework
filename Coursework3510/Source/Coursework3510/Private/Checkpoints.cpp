// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoints.h"
#include "MyPlayerCar.h"

//Sets default values
ACheckpoints::ACheckpoints()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	Volume->InitBoxExtent(FVector(100.f, 400.f, 500.f));
	Volume->SetCollisionResponseToAllChannels(ECR_Overlap);
	Volume->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ACheckpoints::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckpoints::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoints::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Volume->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoints::OnVolumeBeginOverlap);
	Volume->OnComponentEndOverlap.AddDynamic(this, &ACheckpoints::OnVolumeEndOverlap);
}

void ACheckpoints::OnVolumeBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AMyPlayerCar>(OtherActor))
	{
		AMyPlayerCar* Car = Cast<AMyPlayerCar>(OtherActor);
		Car->LapCheckpoint(CheckpointNumber, MaxCheckpoints, bStartFinishLine);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not a car"));
	}
}

void ACheckpoints::OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Triggered when vehicles exits Box Trigger
}

