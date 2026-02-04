// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BaseServerGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYFPSGAME_API ABaseServerGameMode : public AGameMode
{
	GENERATED_BODY()
	
	
public:
	ABaseServerGameMode();

	virtual void BeginPlay() override;//游戏运行时执行BeginPlay
	virtual void Tick(float DeltaSeconds) override;//游戏运行时每一帧都在执行Tick
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;//退出游戏或者卸载关卡的时候游戏时执行EndPlay

	virtual void PostLogin(APlayerController* NewPlayer) override;//当有玩家登录时会调用此函数

	virtual void RequestResPawn(ACharacter* ElimmedCharacter, AController* ElimmedController);


protected:




private:

	//UPROPERTY()
	//class ABaseGameState* BaseGameState;





	
};
