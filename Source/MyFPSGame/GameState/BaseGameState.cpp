// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameState.h"
#include "Net/UnrealNetwork.h"
#include "MyFPSGame/Enum/AllEnum.h"

void ABaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, TopScoringPlayers);
	DOREPLIFETIME(ThisClass, TFactionIndex);
	DOREPLIFETIME(ThisClass, CTFactionIndex);
	DOREPLIFETIME(ThisClass, CTFactionScores);
	DOREPLIFETIME(ThisClass, TFactionScores);
}

void ABaseGameState::FactionSizeChanged(EFaction OldFaction, EFaction NewFaction){

	if (OldFaction != EFaction::EF_NULL) {
		switch (OldFaction){
		case EFaction::EF_CT:
			--CTFactionIndex;
			break;
		case EFaction::EF_T:
			--TFactionIndex;
			break;
		}
	}
	if (NewFaction != EFaction::EF_NULL) {
		switch (NewFaction) {
		case EFaction::EF_CT:
			++CTFactionIndex;
			break;
		case EFaction::EF_T:
			++TFactionIndex;
			break;
		}
	}

}
