#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_RaceManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStarted); //
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinished); //

UCLASS()
class COURSEWORK3510_API AGM_RaceManager : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGM_RaceManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	int32 TotalLaps = 4; //total laps needed to win (plus one to allow for a completion of the current lap)

	UPROPERTY(BlueprintReadOnly, Category = "Race")
	float ElapsedTime = 0.f; //time since the race started

	UPROPERTY(BlueprintReadOnly, Category = "Race")
	bool bRaceRunning = false; //race state

	UPROPERTY(BlueprintReadOnly, Category = "Race")
	bool bRaceFinished = false; //race state

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	float MaxRaceTime = 0.f; //How long the race will go on for (0 = unlimited)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	float CourtesyTime = 10.f; //time after the first finisher during which other players can finish

	UPROPERTY(BlueprintReadOnly, Category = "Race")
	int32 TotalPlayers = 1; //how many players in the game

	UPROPERTY(BlueprintReadOnly, Category = "Race")
	int32 PlayersFinished = 0; //how many have finished so far

	UFUNCTION(BlueprintCallable, Category = "Race")
	void StartRace(); //The callable function that starts the race immeidately

	UFUNCTION(BlueprintCallable, Category = "Race")
	void StartRaceWithCountdown(float CountdownSeconds); //starts the race with a countdown (settable in the function call)

	UFUNCTION(BlueprintCallable, Category = "Race")
	void NotifyPlayerFinished(); //call this when a player finishes

	UFUNCTION(BlueprintCallable, Category = "Race")
	void EndRace(); //ends the race immediately


	UFUNCTION(BlueprintPure, Category = "Race|UI")
	float GetCountdownRemaining() const; //gets remaining countdown time

	UFUNCTION(BlueprintPure, Category = "Race|UI")
	float GetElapsedTime() const; //gets the elapsed time


	UFUNCTION(BlueprintImplementableEvent, Category = "Race")
	void BP_OnRaceStarted(); //blueprnit event for race starting

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStarted OnStarted; //event for started

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStarted OnFinished; //event for finished
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Race")
	void BP_OnRaceEnded(); //event for end of race

	UFUNCTION(BlueprintImplementableEvent, Category = "Race")
	void BP_OnPlayerFinished(int32 PlayerFinishOrder, float FinishTime); //event for player finish, used for UI




	UFUNCTION(BlueprintCallable)
	void CallCreateLobby();

	UFUNCTION(BlueprintCallable)
	void CallClientTravel(const FString& Address);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FTimerHandle TimerHandle_Tick;
	FTimerHandle TimerHandle_RaceDuration;
	FTimerHandle TimerHandle_Countdown;
	FTimerHandle TimerHandle_Courtesy;

	void TickTimer(); //internal timer used for elapsed time

	void OnRaceDurationExpired(); //caleld when race hits max time
	void OnCourtesyExpired(); //called when courtesy time expires
	void OnCountdownComplete(); //called when countdown finishes

	void FinishRaceInternal();

};
