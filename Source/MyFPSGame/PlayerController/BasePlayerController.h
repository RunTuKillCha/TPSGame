// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYFPSGAME_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDWeapon(bool EquipWeapon, bool MainWeapon);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarryAmmo(int32 CarryAmmo);
	void SetHUDTimeRemaining(int32 TimeRemaining);
	void SetElimDelay(float Delay);

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const	override;

	float GetElimDelay();
	float GetServerTime();
	void ChooseFaction();
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();

	void ParameterCorrections();//参数修正

	UPROPERTY(Replicated)
	float WaitPlayerDuration; //服务器等待玩家加入时长
	UPROPERTY(Replicated)
	float WarmupDuration; //游戏开始热身倒计时时长
	UPROPERTY(Replicated)
	float RoundStartDuration; //回合开始倒计时时长
	UPROPERTY(Replicated)
	float RoundTime; //回合时长
	UPROPERTY(Replicated)
	float RoundEndTime; //下一回合等待时长



	UPROPERTY(Replicated)
	float LevelStartingTime = 0.f;//关卡开启时间

	UPROPERTY(Replicated) 
	float ReadyTime = 0.f;//玩家全部就绪游戏开始时间

	UPROPERTY(Replicated)
	float ReadyEndTime = 0.f;//热身结束时间

	UPROPERTY(Replicated)
	float RoundStartingTime = 0.f;//回合开始时间

	UPROPERTY(Replicated)
	float RoundOverTime = 0.f;//回合结束时间

	bool bChooseFaction = false;

	UFUNCTION(Server,Reliable)
	void ServerDebug();


	UPROPERTY(ReplicatedUsing = OnRep_bHiddenCharacterOverlay)
	bool bHiddenCharacterOverlay;

	UPROPERTY(ReplicatedUsing = OnRep_bHiddenFactionSelection)
	bool bHiddenFactionSelection = true;

	UPROPERTY(ReplicatedUsing = OnRep_ClientSetCarma)
	FString ClientSetCarma;


protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float Time);

	UFUNCTION(Client, Reliable)
	void ClientRequestServerTime(float Time, float TimeServer);

	

	float ClientServerDelta = 0.f;

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeFrequencyUpdates = 5.f;

	float TimeRunningTime = 0.f;
private:
	UPROPERTY()
	class ABaseHUD* BaseHUD;

	UPROPERTY()
	AActor* CurrentCameraActor;
	
	//float MatchTime = 180.f;//对

	uint32 CountdownSecond;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;



	UFUNCTION()
	void OnRep_ClientSetCarma();

	UFUNCTION()
	void OnRep_MatchState();

	UFUNCTION()
	void OnRep_bHiddenCharacterOverlay();

	UFUNCTION()
	void OnRep_bHiddenFactionSelection();

	UFUNCTION()
	void OnRep_PlayerFaction();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	bool bInitializeCharacterOverlay = false;



	float HUDHealth = 100;
	float HUDMaxHealth = 100;
	float HUDScore = 0;
	bool HUDEquipWeapon = false;
	bool HUDMainWeapon = false;
	int32 HUDDefeats = 0;
	int32 HUDAmmo = 0;
	int32 HUDCarryAmmo = 0;
	int32 HUDTimeRemaining = 0;

	float ElimDelay;

	
};
