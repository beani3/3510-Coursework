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


void APC_RaceController::RequestSetPaused(bool bPause)
{
	// This is always called on the local player’s controller
	if (HasAuthority())
	{
		MulticastApplyPaused(bPause, this);
	}
	else
	{
		ServerSetPaused(bPause, this);
	}
}


void APC_RaceController::ServerSetPaused_Implementation(bool bPause, APlayerController* InstigatorPC)
{
	MulticastApplyPaused(bPause, InstigatorPC);
}

void APC_RaceController::MulticastApplyPaused_Implementation(bool bPause, APlayerController* InstigatorPC)
{
	if (UWorld* W = GetWorld())
	{
		UGameplayStatics::SetGamePaused(W, bPause);
	}

	// Is this *the* controller that initiated the pause?
	const bool bIsInstigator = (InstigatorPC == this);



	if (bPause)
	{
		if (bIsInstigator)
		{
			// Show full pause menu only for the pausing player
			ShowPauseMenu();

			if (AHUD_Race* RH = GetHUD<AHUD_Race>())
			{
				RH->SetHUDVisible(false);
			}
		}
		else
		{
			// Show “waiting for <name>” overlay for everyone else
			ShowOtherPlayerPausedWidget();

			
		}
	}
	else
	{
		// Unpause: hide everything on all clients
		if (bIsInstigator)
		{
			HidePauseMenu();

			if (AHUD_Race* RH = GetHUD<AHUD_Race>())
			{
				RH->SetHUDVisible(true);
			}
		}
		else
		{
			HideOtherPlayerPausedWidget();
		}
	}
}




void APC_RaceController::ShowOtherPlayerPausedWidget()
{
	if (!OtherPlayerPauseWidget && OtherPlayerPauseWidgetClass)
	{
		OtherPlayerPauseWidget = CreateWidget<UUserWidget>(this, OtherPlayerPauseWidgetClass);
	}

	if (OtherPlayerPauseWidget && !OtherPlayerPauseWidget->IsInViewport())
	{
		OtherPlayerPauseWidget->AddToViewport(60); // above HUD, below win screen
	}


}

void APC_RaceController::HideOtherPlayerPausedWidget()
{
	if (OtherPlayerPauseWidget && OtherPlayerPauseWidget->IsInViewport())
	{
		OtherPlayerPauseWidget->RemoveFromParent();
	}
}




void APC_RaceController::ResumeGame()
{
	RequestSetPaused(false);
}

void APC_RaceController::ShowPauseMenu()
{
	if (!PauseMenuWidget && PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
	}

	
	if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
	{
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