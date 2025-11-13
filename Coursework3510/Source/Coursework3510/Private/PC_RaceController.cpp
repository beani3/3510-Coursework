// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_RaceController.h"
#include "GS_RaceState.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "HUD_Race.h"

APC_RaceController::APC_RaceController()
{
	bReplicates = true;
}

void APC_RaceController::BeginPlay()
{
	Super::BeginPlay();

	// Create the countdown UI
	if (CountdownWidgetClass)
	{
		CountdownWidget = CreateWidget<UUserWidget>(this, CountdownWidgetClass);
		if (CountdownWidget)
		{
			CountdownWidget->AddToViewport(40);
			CountdownWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// Bind RaceState events
	if (AGS_RaceState* RS = GetWorld() ? GetWorld()->GetGameState<AGS_RaceState>() : nullptr)
	{
		RS->OnCountdownStarted.AddDynamic(this, &APC_RaceController::HandleCountdownStarted);
		RS->OnRaceStarted.AddDynamic(this, &APC_RaceController::OnRaceStarted);
		RS->OnRaceFinished.AddDynamic(this, &APC_RaceController::OnRaceFinished);

		// If we joined mid-countdown, show it immediately
		if (RS->IsCountdownActive() && CountdownWidget)
		{
			CountdownWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void APC_RaceController::HandleCountdownStarted()
{
	if (CountdownWidget)
	{
		CountdownWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

/* =========================
   Pause Flow
   ========================= */

void APC_RaceController::RequestSetPaused(bool bPause)
{
	// Client asks server; host calls directly too
	if (!HasAuthority())
	{
		ServerSetPaused(bPause);
		return;
	}
	ServerSetPaused(bPause);
}

void APC_RaceController::ServerSetPaused_Implementation(bool bPause)
{
	MulticastApplyPaused(bPause);
}

void APC_RaceController::MulticastApplyPaused_Implementation(bool bPause)
{
	if (UWorld* W = GetWorld())
	{
		UGameplayStatics::SetGamePaused(W, bPause);
	}

	// Show/hide pause menu
	if (bPause) { ShowPauseMenu(); }
	else { HidePauseMenu(); }

	// Hide HUD while menus are up
	if (AHUD_Race* RH = GetHUD<AHUD_Race>())
	{
		RH->SetHUDVisible(!bPause);
	}
}

/* =========================
   UI Helpers
   ========================= */

void APC_RaceController::ShowPauseMenu()
{
	// Create if needed
	if (!PauseMenuWidget && PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
	}

	// Add to viewport if not already
	if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->AddToViewport(50); // high Z-order so it’s on top
	}

	// Switch to UI-only input and show cursor
	bShowMouseCursor = true;

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(PauseMenuWidget ? PauseMenuWidget->TakeWidget() : TSharedPtr<SWidget>());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(Mode);
}

void APC_RaceController::HidePauseMenu()
{
	// Remove widget
	if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->RemoveFromParent();
	}

	// Restore input to game
	bShowMouseCursor = false;

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
}

/* =========================
   Optional: Race events
   ========================= */

void APC_RaceController::OnRaceStarted()
{
	// Hide countdown when race actually begins
	if (CountdownWidget)
	{
		CountdownWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APC_RaceController::OnRaceFinished()
{
	// Show win/results screen
	if (WinScreenWidgetClass)
	{
		WinScreenWidget = CreateWidget<UUserWidget>(this, WinScreenWidgetClass);
		if (WinScreenWidget)
		{
			WinScreenWidget->AddToViewport(100);
		}
	}

	// Also hide countdown on finish, just in case
	if (CountdownWidget)
	{
		CountdownWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APC_RaceController::ClientShowImmediateWinScreen_Implementation()
{
	// You can reuse the same WinScreenWidgetClass, or make a different one if you like
	if (WinScreenWidgetClass)
	{
		if (!WinScreenWidget)
		{
			WinScreenWidget = CreateWidget<UUserWidget>(this, WinScreenWidgetClass);
		}

		if (WinScreenWidget && !WinScreenWidget->IsInViewport())
		{
			WinScreenWidget->AddToViewport(100);
		}
	}

	//  also hide HUD / countdown / etc. here for the winner only

}