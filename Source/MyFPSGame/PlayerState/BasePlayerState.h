// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MYFPSGAME_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	void AddToScore(float ScoreAmount = 1.f);
	void AddToDefeats(int32 DefeatsAmount = 1);

	UFUNCTION(Server, Reliable)
	void ServerSetFaction(EFaction InFaction);


	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();

	UFUNCTION()
	virtual void OnRep_Faction();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const	override;
	EFaction GetFaction() const;
	//virtual void OnRep_PlayerName();
	//virtual void OnRep_bIsInactive();
	//virtual void OnRep_PlayerId();
	//virtual void OnRep_UniqueId();
	
private:
	UPROPERTY()
	class ABaseCharacter* Character;
	UPROPERTY()
	class ABasePlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	UPROPERTY(ReplicatedUsing = OnRep_Faction)
	EFaction Faction = EFaction::EF_NULL;
};
