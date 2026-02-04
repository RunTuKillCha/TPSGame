// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "list"
#include "../Enum/AllEnum.h"
#include "ServerGameMode.generated.h"

namespace MatchState
{
	//extern const FName WaitingPlayer;
	extern const FName WarmupTime;//全部玩家加入后开始热身
	extern const FName WarringBegin;//交战之前
	extern const FName Warring;//回合中
	extern const FName WarringEnd;//回合结束
}

UCLASS()
class MYFPSGAME_API AServerGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AServerGameMode();

public:
	virtual void BeginPlay() override;//游戏运行时执行BeginPlay
	virtual void Tick(float DeltaSeconds) override;//游戏运行时每一帧都在执行Tick
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;//退出游戏或者卸载关卡的时候游戏时执行EndPlay
	virtual void OnMatchStateSet() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;//当有玩家登录时会调用此函数

	virtual void PlayerEliminated(class ABaseCharacter* ElimmedCharacter, class ABasePlayerController* VictmController, ABasePlayerController* AttackerController);

	virtual void RequestResPawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	void ChooseFaction(ABasePlayerController* PlayerController);

	void On_SetMatchState(FName State);
	//FMap_Information* GetMap_Information();


	UPROPERTY(EditDefaultsOnly)
	float WaitPlayerDuration = 100.f; //服务器等待玩家加入时长

	UPROPERTY(EditDefaultsOnly)
	float WarmupDuration = 10.f; //游戏开始热身倒计时时长

	UPROPERTY(EditDefaultsOnly)
	float RoundStartDuration = 10.f; //回合开始倒计时时长

	UPROPERTY(EditDefaultsOnly)
	float RoundTime = 115.f; //回合时长

	UPROPERTY(EditDefaultsOnly)
	float RoundEndTime = 10.f; //下一回合等待时长

protected:



private:
	std::list<ABasePlayerController*> ElimList;

	FTimerHandle ElimTimer;

	int32 RunTime = 0;

	UPROPERTY()
	class ABaseGameState* BaseGameState;

	UPROPERTY(EditAnywhere)
	float ElimDelay = 5.f;

	float LevelStartingTime = 0.f;//关卡开启时间
	
	float ReadyTime = 0.f;//玩家全部就绪游戏开始时间

	float ReadyEndTime = 0.f;//热身结束时间

	float RoundStartingTime = 0.f;//回合开始时间

	float RoundOverTime = 0.f;//回合结束时间

	/*倒计时*/
	float WaitPlayerCountdown = 0.f;//等待玩家倒计时

	float WarmupCountdown = 0.f; //热身倒计时

	float RoundStartingCountdown = 0.f;//回合开始倒计时

	float RoundCountdown = 0.f;//回合倒计时

	float RoundOverCountdown = 0.f;//回合结束倒计时



	void ElimTimerFinished();
	void DetectMatchState();
	void TryRoundWin(EFaction WinFaction);
	void PlayerControllerInformationSynchronization(APlayerController* NewPlayer);

	void TryVictory(EFaction VictoryFaction);
};
