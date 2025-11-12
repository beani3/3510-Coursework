#include "GM_RaceManager.h"
#include "GS_RaceState.h"
#include "TimerManager.h"

void AGM_RaceManager::StartRaceWithCountdown(float Seconds)
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		const double Now = RS->GetServerWorldTimeSeconds();
		RS->TotalLaps = Config_TotalLaps;
		RS->bRaceFinished = false;
		RS->bRaceRunning = false;
		
		RS->CountdownEndServerTime = Now + Seconds;

		RS->NotifyCountdownStarted();

		// server-local refresh if you show countdown on host
		RS->OnRep_Countdown();

		GetWorldTimerManager().ClearTimer(TH_CountdownDone);
		GetWorldTimerManager().SetTimer(TH_CountdownDone, this, &AGM_RaceManager::HandleCountdownFinished, Seconds, false);
	}
}

void AGM_RaceManager::HandleCountdownFinished()
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		const double Now = RS->GetServerWorldTimeSeconds();
		RS->RaceStartServerTime = Now;
		RS->bRaceRunning = true;

		// Host/server UI
		OnStarted.Broadcast();

		// Fire server-side now; clients will fire via replication
		RS->OnRep_RaceFlags();
	}
}

void AGM_RaceManager::HandleCourtesyEnd()
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		RS->bRaceRunning = false;
		RS->bRaceFinished = true;

		OnFinished.Broadcast();
		RS->OnRep_RaceFlags();
	}
}

void AGM_RaceManager::ForceFinishRace(float CourtesySeconds)
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		const double Now = RS->GetServerWorldTimeSeconds();
		RS->RaceEndServerTime = Now + CourtesySeconds;
		RS->bRaceRunning = false;
		RS->bRaceFinished = true;

		OnFinished.Broadcast();
		RS->OnRep_RaceFlags();

		// Optional: also start a timer that calls HandleCourtesyEnd after CourtesySeconds
		GetWorldTimerManager().ClearTimer(TH_CourtesyEnd);
		if (CourtesySeconds > 0.f)
		{
			GetWorldTimerManager().SetTimer(TH_CourtesyEnd, this, &AGM_RaceManager::HandleCourtesyEnd, CourtesySeconds, false);
		}
	}
}

void AGM_RaceManager::RestartRace()
{
	if (!HasAuthority()) return;

	GetWorldTimerManager().ClearTimer(TH_CountdownDone);
	GetWorldTimerManager().ClearTimer(TH_CourtesyEnd);

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		RS->bRaceRunning = false;
		RS->bRaceFinished = false;
		RS->CountdownEndServerTime = 0.0;
		RS->RaceStartServerTime = 0.0;
		RS->RaceEndServerTime = 0.0;
	}
}
