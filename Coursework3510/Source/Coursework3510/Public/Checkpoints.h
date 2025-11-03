#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Checkpoints.generated.h"


UCLASS()
class COURSEWORK3510_API ACheckpoints : public AActor
{
	GENERATED_BODY()
	
public:	
	ACheckpoints();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void PostInitializeComponents();

	UFUNCTION()	
	void OnVolumeBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); //function for begin overlap event
	
	UFUNCTION()
	void OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex); //function for end overlap event

	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	UBoxComponent* Volume; //Box component to define the checkpoint area
	

	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	int32 CheckpointNumber = 1; //number of this checkpoint in the chain

	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	int32 MaxCheckpoints = 1; //total number of checkpoints in the chain

	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	bool bStartFinishLine = false; //is this checkpoint the start/finish line

};
