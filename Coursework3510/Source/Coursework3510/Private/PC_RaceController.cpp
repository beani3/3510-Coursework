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

	if (bPause) { ShowPauseMenu(); }
	else { HidePauseMenu(); }

	if (AHUD_Race* RH = GetHUD<AHUD_Race>())
	{
		RH->SetHUDVisible(!bPause);
	}
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