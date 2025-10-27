/*
Thid id the RaceManager class, and I want it to manage the finish screen for each player, displaying times and positions. Whilst also handling
the amounbt of laps needed to finish, and a courtesy time after the first player finishes to allow others to complete
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaceManager.generated.h"

UCLASS()
class COURSEWORK3510_API ARaceManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARaceManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race") //The amount of laps needed to win + 1
	int32 TotalLaps = 4;


	UPROPERTY(BlueprintReadOnly, Category = "Race") //The time it took to complete the map
	float ElapsedTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Race") //whether or not the race is running
	bool bRaceRunning = false;

	UPROPERTY(BlueprintReadOnly, Category = "Race") //whethter or not the race finished
	bool bRaceFinished = false;

	UFUNCTION(BlueprintCallable, Category = "Race") //Start the race
	void StartRace();

	UFUNCTION(BlueprintCallable, Category = "Race") //Calls the function when it incriments a lap
	void NotifyLapCompleted(int32 NewLap);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FTimerHandle TimerHandle_Tick;

	UFUNCTION()
	void TickTimer(); //Timer tick function to update elapsed time

	void FinishRace(); //Function for when the race finishes

	UFUNCTION(BlueprintImplementableEvent, Category = "Race") //What happens after the race finihsed
	void BP_OnRaceFinished(float FinalTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
