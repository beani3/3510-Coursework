#include "GM_RaceManager.h"
#include "GS_RaceState.h"
#include "PS_PlayerState.h"
#include "TimerManager.h"

// the game mode manages the race works with the game and player states to control


void AGM_RaceManager::StartRaceWithCountdown(float Seconds)
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		const double Now = RS->GetServerWorldTimeSeconds();

		
		bCourtesyStarted = false;

		RS->TotalLaps = Config_TotalLaps;
		RS->bRaceFinished = false;
		RS->bRaceRunning = false;
		RS->CountdownEndServerTime = Now + Seconds;
		RS->RaceStartServerTime = 0.0;
		RS->RaceEndServerTime = 0.0;

		
		RS->NotifyCountdownStarted();
		RS->OnRep_Countdown();

		
		GetWorldTimerManager().ClearTimer(TH_CountdownDone);
		GetWorldTimerManager().SetTimer(
			TH_CountdownDone,
			this,
			&AGM_RaceManager::HandleCountdownFinished,
			Seconds,
			false
		);

		
		GetWorldTimerManager().ClearTimer(TH_CourtesyEnd);
	}
}

void AGM_RaceManager::HandleCountdownFinished()
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		const double Now = RS->GetServerWorldTimeSeconds();

		RS->CountdownEndServerTime = 0.0;
		RS->RaceStartServerTime = Now;
		RS->bRaceRunning = true;
		RS->bRaceFinished = false;

		
		OnStarted.Broadcast();

		
		RS->OnRep_RaceFlags();
	}
}

void AGM_RaceManager::NotifyPlayerFinished(APS_PlayerState* FinishedPlayer)
{
	if (!HasAuthority() || !FinishedPlayer) return;

	
	if (!bCourtesyStarted)
	{
		bCourtesyStarted = true;

		
		ForceFinishRace(Config_CourtesySeconds);
	}

	
}

void AGM_RaceManager::ForceFinishRace(float CourtesySeconds)
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		RS->bRaceRunning = false;
		RS->bRaceFinished = false;
	
		GetWorldTimerManager().ClearTimer(TH_CourtesyEnd);
		if (CourtesySeconds > 0.f)
		{
			GetWorldTimerManager().SetTimer(
				TH_CourtesyEnd,
				this,
				&AGM_RaceManager::HandleCourtesyEnd,
				CourtesySeconds,
				false
			);
		}
		else
		{
			HandleCourtesyEnd();
		}
	}
}

void AGM_RaceManager::HandleCourtesyEnd()
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		const double Now = RS->GetServerWorldTimeSeconds();

		RS->RaceEndServerTime = Now;
		RS->bRaceRunning = false;
		RS->bRaceFinished = true;

		OnFinished.Broadcast();
		RS->OnRep_RaceFlags();  
	}
}

void AGM_RaceManager::RestartRace()
{
	if (!HasAuthority()) return;

	GetWorldTimerManager().ClearTimer(TH_CountdownDone);
	GetWorldTimerManager().ClearTimer(TH_CourtesyEnd);
	bCourtesyStarted = false;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		RS->bRaceRunning = false;
		RS->bRaceFinished = false;
		RS->CountdownEndServerTime = 0.0;
		RS->RaceStartServerTime = 0.0;
		RS->RaceEndServerTime = 0.0;
	}
}
