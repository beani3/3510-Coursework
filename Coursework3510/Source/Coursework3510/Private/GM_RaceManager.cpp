#include "GM_RaceManager.h"
#include "GS_RaceState.h"
#include "PS_PlayerState.h"
#include "TimerManager.h"

void AGM_RaceManager::StartRaceWithCountdown(float Seconds)
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		const double Now = RS->GetServerWorldTimeSeconds();

		// Reset internal state
		bCourtesyStarted = false;

		RS->TotalLaps = Config_TotalLaps;
		RS->bRaceFinished = false;
		RS->bRaceRunning = false;
		RS->CountdownEndServerTime = Now + Seconds;
		RS->RaceStartServerTime = 0.0;
		RS->RaceEndServerTime = 0.0;

		// Server local notify; clients see it via OnRep_Countdown
		RS->NotifyCountdownStarted();
		RS->OnRep_Countdown();

		// Timer to actually start the race when countdown ends
		GetWorldTimerManager().ClearTimer(TH_CountdownDone);
		GetWorldTimerManager().SetTimer(
			TH_CountdownDone,
			this,
			&AGM_RaceManager::HandleCountdownFinished,
			Seconds,
			false
		);

		// Clear any old courtesy timer
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

		// Host/server UI
		OnStarted.Broadcast();

		// Replication notify so clients fire OnRaceStarted
		RS->OnRep_RaceFlags();
	}
}

void AGM_RaceManager::NotifyPlayerFinished(APS_PlayerState* FinishedPlayer)
{
	if (!HasAuthority() || !FinishedPlayer) return;

	// First finisher starts courtesy timer
	if (!bCourtesyStarted)
	{
		bCourtesyStarted = true;

		// Start courtesy timer. When it ends, race will be marked finished
		ForceFinishRace(Config_CourtesySeconds);
	}

	// (Any extra logic per-player on finish can go here later)
}

void AGM_RaceManager::ForceFinishRace(float CourtesySeconds)
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		// Race is no longer running, but not "finished" until courtesy ends
		RS->bRaceRunning = false;
		RS->bRaceFinished = false;

		// Clear any existing courtesy timer and restart
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
		RS->OnRep_RaceFlags();   // triggers OnRaceFinished on clients
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
