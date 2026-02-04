// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BaseGameState.generated.h"

/**
 * 
 */
UCLASS()
class MYFPSGAME_API ABaseGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void FactionSizeChanged(EFaction OldFaction, EFaction NewFaction);

	UPROPERTY(Replicated)
	TArray<class ABasePlayerState*> TopScoringPlayers;

	UPROPERTY(Replicated)
	int32 TFactionIndex;

	UPROPERTY(Replicated)
	int32 CTFactionIndex;

	UPROPERTY(Replicated)
	int32 CTFactionScores;

	UPROPERTY(Replicated)
	int32 TFactionScores;
};
