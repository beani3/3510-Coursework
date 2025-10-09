 //Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	ACheckpoints();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PostInitializeComponents();

	UFUNCTION()	
	void OnVolumeBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	UBoxComponent* Volume;
	

	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	int32 CheckpointNumber = 1;

	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	int32 MaxCheckpoints = 1;

	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	bool bStartFinishLine = false;

};
