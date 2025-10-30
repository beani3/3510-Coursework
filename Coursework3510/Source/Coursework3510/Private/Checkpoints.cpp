#include "Checkpoints.h"
#include "MyPlayerCar.h"

ACheckpoints::ACheckpoints()
{
	PrimaryActorTick.bCanEverTick = true;

	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	Volume->InitBoxExtent(FVector(400.f, 400.f, 500.f));
	Volume->SetCollisionResponseToAllChannels(ECR_Overlap);
	Volume->SetupAttachment(GetRootComponent());

}

void ACheckpoints::BeginPlay()
{
	Super::BeginPlay();
	
}

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

