// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"
#include "../Character/BaseCharacter.h"
#include "../PlayerController/BasePlayerController.h"
#include "MyFPSGame/GameState/BaseGameState.h"
#include "Net/UnrealNetwork.h"
#include "MyFPSGame/GameMode/ServerGameMode.h"

void ABasePlayerState::AddToScore(float ScoreAmount){
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<ABaseCharacter>(GetPawn()) : Character;
	if (Character) {
		Controller = Controller == nullptr ? Cast<ABasePlayerController>(Character->GetController()) : Controller;
		if (Controller) {
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABasePlayerState::AddToDefeats(int32 DefeatsAmount){
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<ABaseCharacter>(GetPawn()) : Character;
	if (Character) {
		Controller = Controller == nullptr ? Cast<ABasePlayerController>(Character->GetController()) : Controller;
		if (Controller) {
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ABasePlayerState::ServerSetFaction_Implementation(EFaction InFaction){
	ABaseGameState* BaseGameState = GetWorld()->GetGameState<ABaseGameState>();
	if (BaseGameState) {
		BaseGameState->FactionSizeChanged(Faction, InFaction);
	}
	Faction = InFaction;
	Controller->SetInputMode(FInputModeGameOnly());
	Controller->SetShowMouseCursor(false);
	Controller->bHiddenCharacterOverlay = false;
	Controller->bHiddenFactionSelection = true;
	Controller->ClientSetCarma = TEXT("MyCamera");
	AServerGameMode* ServerGameMode = GetWorld()->GetAuthGameMode<AServerGameMode>();
	if (ServerGameMode) {
		ServerGameMode->RequestResPawn(Controller->GetCharacter(), Controller);
	}

	
}


void ABasePlayerState::OnRep_Score(){
	Super::OnRep_Score();
	Character = Character == nullptr ? Cast<ABaseCharacter>(GetPawn()) : Character;
	if (Character) {
		Controller = Controller == nullptr ? Cast<ABasePlayerController>(Character->GetController()) : Controller;
		if (Controller) {
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABasePlayerState::OnRep_Defeats(){
	Character = Character == nullptr ? Cast<ABaseCharacter>(GetPawn()) : Character;
	if (Character) {
		Controller = Controller == nullptr ? Cast<ABasePlayerController>(Character->GetController()) : Controller;
		if (Controller) {
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ABasePlayerState::OnRep_Faction(){
	
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, Defeats);
	DOREPLIFETIME(ThisClass, Faction);
}

EFaction ABasePlayerState::GetFaction() const{
	return Faction;
}
