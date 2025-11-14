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

	if (CountdownWidgetClass)
	{
		CountdownWidget = CreateWidget<UUserWidget>(this, CountdownWidgetClass);
		if (CountdownWidget)
		{
			CountdownWidget->AddToViewport(40);
			CountdownWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (AGS_RaceState* RS = GetWorld() ? GetWorld()->GetGameState<AGS_RaceState>() : nullptr)
	{
		RS->OnCountdownStarted.AddDynamic(this, &APC_RaceController::HandleCountdownStarted);
		RS->OnRaceStarted.AddDynamic(this, &APC_RaceController::OnRaceStarted);
		RS->OnRaceFinished.AddDynamic(this, &APC_RaceController::OnRaceFinished);

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

// Called from pawn or UI
void APC_RaceController::RequestSetPaused(bool bPause)
{
	// Always route through server so it can notify everyone
	if (HasAuthority())
	{
		ServerSetPaused_Implementation(bPause);
	}
	else
	{
		ServerSetPaused(bPause);
	}
}

void APC_RaceController::ServerSetPaused_Implementation(bool bPause)
{
	UWorld* W = GetWorld();
	if (!W) return;

	// Pause/unpause the game world on the server
	UGameplayStatics::SetGamePaused(W, bPause);

	// This controller is the one that initiated the pause
	APlayerController* InstigatorPC = this;

	// Tell each player controller what to show
	for (FConstPlayerControllerIterator It = W->GetPlayerControllerIterator(); It; ++It)
	{
		if (APC_RaceController* RC = Cast<APC_RaceController>(*It))
		{
			const bool bIsInstigator = (RC == InstigatorPC);
			RC->ClientApplyPaused(bPause, bIsInstigator);
		}
	}
}

void APC_RaceController::ClientApplyPaused_Implementation(bool bPause, bool bIsInstigator)
{
	// Make sure this local world is actually paused/unpaused
	if (UWorld* W = GetWorld())
	{
		UGameplayStatics::SetGamePaused(W, bPause);
	}

	if (bPause)
	{
		// Everyone gets the overlay
		ShowPausedOverlay();

		if (bIsInstigator)
		{
			// Only the pausing player gets the full pause menu
			ShowPauseMenu();

			if (AHUD_Race* RH = GetHUD<AHUD_Race>())
			{
				RH->SetHUDVisible(false);
			}
		}
	}
	else
	{
		// Everyone loses the overlay
		HidePausedOverlay();

		if (bIsInstigator)
		{
			HidePauseMenu();

			if (AHUD_Race* RH = GetHUD<AHUD_Race>())
			{
				RH->SetHUDVisible(true);
			}
		}
	}
}

void APC_RaceController::ResumeGame()
{
	RequestSetPaused(false);
}

// ---------- Overlay for all players ----------

void APC_RaceController::ShowPausedOverlay()
{
	if (!PausedOverlayWidget && PausedOverlayWidgetClass)
	{
		PausedOverlayWidget = CreateWidget<UUserWidget>(this, PausedOverlayWidgetClass);
	}

	if (PausedOverlayWidget && !PausedOverlayWidget->IsInViewport())
	{
		// Overlay layer, under menus and win screen if you like
		PausedOverlayWidget->AddToViewport(45);
	}
}

void APC_RaceController::HidePausedOverlay()
{
	if (PausedOverlayWidget && PausedOverlayWidget->IsInViewport())
	{
		PausedOverlayWidget->RemoveFromParent();
	}
}

// ---------- Pause menu for instigator ----------

void APC_RaceController::ShowPauseMenu()
{
	if (!PauseMenuWidget && PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
	}

	if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
	{
		// Menu on top of the overlay
		PauseMenuWidget->AddToViewport(50);
	}

	bShowMouseCursor = true;

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(PauseMenuWidget ? PauseMenuWidget->TakeWidget() : TSharedPtr<SWidget>());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(Mode);
}

void APC_RaceController::HidePauseMenu()
{
	if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->RemoveFromParent();
	}

	bShowMouseCursor = false;

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
}

// ---------- Race events ----------

void APC_RaceController::OnRaceStarted()
{
	if (CountdownWidget)
	{
		CountdownWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APC_RaceController::OnRaceFinished()
{
	if (WinScreenWidgetClass)
	{
		WinScreenWidget = CreateWidget<UUserWidget>(this, WinScreenWidgetClass);
		if (WinScreenWidget)
		{
			WinScreenWidget->AddToViewport(100);
		}
	}

	if (CountdownWidget)
	{
		CountdownWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APC_RaceController::ClientShowImmediateWinScreen_Implementation()
{
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
}
