// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerGameMode.h"
#include "MyFPSGame/Character/BaseCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "MyFPSGame/PlayerController/BasePlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "MyFPSGame/PlayerState/BasePlayerState.h"
#include "MyFPSGame/GameState/BaseGameState.h"

//数学库
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "TimerManager.h"
#include <algorithm> 

#include "MyFPSGame/TCP/TCP_Client.h"
//#include "BaseServerGameMode.h"

namespace MatchState
{
	//const FName WaitingPlayer = FName(TEXT("WaitingPlayer"));//此阶段为服务器等待玩家加入
	const FName WarmupTime = FName(TEXT("WarmupTime"));//全部玩家加入后开始热身
	const FName WarringBegin = FName(TEXT("WarringBegin"));//交战之前
	const FName Warring = FName(TEXT("Warring"));//回合中
	const FName WarringEnd = FName(TEXT("WarringEnd"));//回合中

}


AServerGameMode::AServerGameMode() {
	//bDelayedStart = true;

	DefaultPawnClass = ABaseCharacter::StaticClass();//创建默认类
	//PlayerControllerClass = AMasterPlayerController::StaticClass();
	//GameStateClass = AServerAndClientGameState::StaticClass();
	//HUDClass = AMyHUD::StaticClass();
	//PlayerStateClass = AMyPlayerState::StaticClass();
	//UE_LOG(LogTemp, Warning, TEXT("Game Mode Init"));

	//* TCP = CreateDefaultSubobject<UTCP_Client>("TCP");
	//TCP->run();
	//TCP->recv();
	//TCP->send();
	//TCP->recv();
}

void AServerGameMode::BeginPlay(){
	Super::BeginPlay();//重写时调用父类功能

	//GetWorldTimerManager().SetTimer(ElimTimer, this, &ThisClass::ElimTimerFinished, 1.f, true);
	LevelStartingTime = GetWorld()->GetTimeSeconds();
	BaseGameState = BaseGameState ? GetWorld()->GetGameState<ABaseGameState>() : BaseGameState;
}

void AServerGameMode::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);

	DetectMatchState();

}

void AServerGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason){
	Super::EndPlay(EndPlayReason);
}

void AServerGameMode::OnMatchStateSet(){
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator ite = GetWorld()->GetPlayerControllerIterator(); ite; ite++) {
		ABasePlayerController* BasePlayerController = Cast< ABasePlayerController>(*ite);
		if (BasePlayerController) {
			BasePlayerController->OnMatchStateSet(MatchState);
		}
	}

}

void AServerGameMode::PostLogin(APlayerController* NewPlayer){
	Super::PostLogin(NewPlayer);
	int32 NumberOfPlayer = GameState.Get()->PlayerArray.Num();

	PlayerControllerInformationSynchronization(NewPlayer);

}

void AServerGameMode::PlayerControllerInformationSynchronization(APlayerController* NewPlayer){
	ABasePlayerController* BasePlayerController = Cast< ABasePlayerController>(NewPlayer);
	if (BasePlayerController) {
		BasePlayerController->WaitPlayerDuration = WaitPlayerDuration;
		BasePlayerController->WarmupDuration = WarmupDuration;
		BasePlayerController->RoundStartDuration = RoundStartDuration;
		BasePlayerController->RoundTime = RoundTime;
		BasePlayerController->RoundEndTime = RoundEndTime;

		BasePlayerController->OnMatchStateSet(MatchState);
		BasePlayerController->LevelStartingTime = LevelStartingTime;
	}
}

void AServerGameMode::TryVictory(EFaction VictoryFaction){

}

void AServerGameMode::PlayerEliminated(ABaseCharacter* ElimmedCharacter, ABasePlayerController* VictmController, ABasePlayerController* AttackerController){
	ABasePlayerState* AttackerPlayerState = AttackerController ? Cast<ABasePlayerState>(AttackerController->PlayerState) : nullptr;
	ABasePlayerState* VictmPlayerState = VictmController ? Cast<ABasePlayerState>(VictmController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictmPlayerState) {
		AttackerPlayerState->AddToScore();
	}
	if (VictmPlayerState) {
		VictmPlayerState->AddToDefeats();
	}

	//复活倒计时
	ElimList.push_back(VictmController);
	//VictmController->SetElimDelay(ElimDelay);
	ElimmedCharacter->Elim();
	
	int32 CTCasualtiesNum = 0, TCasualtiesNum = 0;
	for (auto it = ElimList.begin(); it != ElimList.end(); it++) {
		ABasePlayerState* BasePlayerState = (*it)->GetPlayerState<ABasePlayerState>();
		if (BasePlayerState) {
			EFaction Faction = BasePlayerState->GetFaction();
			switch (Faction){
			case EFaction::EF_CT:
				++CTCasualtiesNum;
				break;
			case EFaction::EF_T:
				++TCasualtiesNum;
				break;
			}
		}
	}
	BaseGameState = BaseGameState ? GetWorld()->GetGameState<ABaseGameState>() : BaseGameState;
	if (BaseGameState) {
		if (CTCasualtiesNum >= BaseGameState->CTFactionIndex) {
			TryRoundWin(EFaction::EF_T);
			//UE_LOG(LogTemp, Warning, TEXT("T Win %d"), BaseGameState->CTFactionIndex);
		}
		if (TCasualtiesNum >= BaseGameState->TFactionIndex) {
			TryRoundWin(EFaction::EF_CT);
			//UE_LOG(LogTemp, Warning, TEXT("CT Win"), BaseGameState->TFactionIndex);
		}
	}

}


void AServerGameMode::TryRoundWin(EFaction WinFaction) {
	BaseGameState = BaseGameState ? GetWorld()->GetGameState<ABaseGameState>() : BaseGameState;
	//如果处在Warring中
	if (MatchState == MatchState::Warring && BaseGameState) {
		//立即进入结束
		SetMatchState(MatchState::WarringEnd);
		switch (WinFaction){//获得分数
		case EFaction::EF_CT:
			++BaseGameState->CTFactionScores;
			if (BaseGameState->CTFactionScores >= 13) {
				TryVictory(EFaction::EF_CT);
			}

			break;
		case EFaction::EF_T:
			++BaseGameState->TFactionScores;
			if (BaseGameState->TFactionScores >= 13) {
				TryVictory(EFaction::EF_T);
			}
			break;
		}

	}
}



void AServerGameMode::RequestResPawn(ACharacter* ElimmedCharacter, AController* ElimmedController){
	if (!ElimmedController || !ElimmedCharacter) return;



	ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(ElimmedController);
	if (BasePlayerController) {
		ABasePlayerState* BasePlayerState = BasePlayerController->GetPlayerState<ABasePlayerState>();
		if (BasePlayerState) {
			EFaction Faction = BasePlayerState->GetFaction();
			AActor* StartSpot = nullptr;
			int32 Selection = FMath::RandRange(1, 7);
			switch (Faction)
			{
			case EFaction::EF_NULL:
				return;
				//StartSpot = FindPlayerStart(ElimmedController, TEXT("Start_ALL"));
				break;
			case EFaction::EF_CT:
				StartSpot = FindPlayerStart(ElimmedController, FString::Printf(TEXT("Start_CT_%d"), Selection));
				break;
			case EFaction::EF_T:
				StartSpot = FindPlayerStart(ElimmedController, FString::Printf(TEXT("Start_T_%d"), Selection));
				break;
			}
			if (StartSpot == nullptr) {
				return;
			}
			ElimmedCharacter->Reset();
			ElimmedCharacter->Destroy();
			RestartPlayerAtPlayerStart(ElimmedController, StartSpot);
			ABaseCharacter* Character = Cast<ABaseCharacter>(ElimmedController->GetPawn());
			if (Character) {
				Character->MulticastSetMesh(Faction);
			}
		}
	} 
}

void AServerGameMode::ChooseFaction(ABasePlayerController* PlayerController){
	ABaseCharacter* Character = Cast<ABaseCharacter>(PlayerController->GetCharacter());
	if (Character == nullptr) return;
	Character->bCharacterHidden = true;
	PlayerController->ChooseFaction();

}

void AServerGameMode::On_SetMatchState(FName State){
	SetMatchState(State);
}

void AServerGameMode::ElimTimerFinished(){
	for (auto it = ElimList.begin(); it != ElimList.end();it++) {
		(*it)->SetElimDelay((*it)->GetElimDelay() - 1);
		

		if ((*it)->GetElimDelay() <= 0) {
			RequestResPawn((*it)->GetCharacter(), (*it));
			it = ElimList.erase(it);
		}
	}
}

void AServerGameMode::DetectMatchState(){
	
	if (MatchState == MatchState::InProgress) {//等待玩家倒计时结束进入热身状态
		WaitPlayerCountdown = WaitPlayerDuration - (GetWorld()->GetTimeSeconds() - LevelStartingTime);
		if (WaitPlayerCountdown <= 0.f) {
			for (FConstPlayerControllerIterator ite = GetWorld()->GetPlayerControllerIterator(); ite; ite++) {
				ABasePlayerController* BasePlayerController = Cast< ABasePlayerController>(*ite);
				if (BasePlayerController) {
					BasePlayerController->ReadyTime = ReadyTime = GetWorld()->GetTimeSeconds();
					RequestResPawn(BasePlayerController->GetCharacter(), BasePlayerController);
				}
			}
			
			SetMatchState(MatchState::WarmupTime);
		}
	}
	if (MatchState == MatchState::WarmupTime) {//热身时间
		WarmupCountdown = WarmupDuration - (GetWorld()->GetTimeSeconds() - ReadyTime);
		if (WarmupCountdown <= 0.f) {
			for (FConstPlayerControllerIterator ite = GetWorld()->GetPlayerControllerIterator(); ite; ite++) {
				ABasePlayerController* BasePlayerController = Cast< ABasePlayerController>(*ite);
				if (BasePlayerController) {
					BasePlayerController->ReadyEndTime = ReadyEndTime = GetWorld()->GetTimeSeconds();
					RequestResPawn(BasePlayerController->GetCharacter(), BasePlayerController);
				}
			}
			SetMatchState(MatchState::WarringBegin);
		}
	}
	if (MatchState == MatchState::WarringBegin) {//回合开始等待时间
		RoundStartingCountdown = RoundStartDuration - (GetWorld()->GetTimeSeconds() - ReadyEndTime);
		if (RoundStartingCountdown <= 0.f) {
			for (FConstPlayerControllerIterator ite = GetWorld()->GetPlayerControllerIterator(); ite; ite++) {
				ABasePlayerController* BasePlayerController = Cast< ABasePlayerController>(*ite);
				if (BasePlayerController) {
					BasePlayerController->RoundStartingTime = RoundStartingTime = GetWorld()->GetTimeSeconds();
					RequestResPawn(BasePlayerController->GetCharacter(), BasePlayerController);
				}
			}
			SetMatchState(MatchState::Warring);
		}
	}
	if (MatchState == MatchState::Warring) {
		RoundCountdown = RoundTime - (GetWorld()->GetTimeSeconds() - RoundStartingTime);
		if (RoundCountdown <= 0.f) {
			for (FConstPlayerControllerIterator ite = GetWorld()->GetPlayerControllerIterator(); ite; ite++) {
				ABasePlayerController* BasePlayerController = Cast< ABasePlayerController>(*ite);
				if (BasePlayerController) {
					BasePlayerController->RoundOverTime = RoundOverTime = GetWorld()->GetTimeSeconds();
				}
			}
			SetMatchState(MatchState::WarringEnd);
		}
	}
	if (MatchState == MatchState::WarringEnd) {
		RoundOverCountdown = RoundEndTime - (GetWorld()->GetTimeSeconds() - RoundOverTime);
		if (RoundOverCountdown <= 0.f) {
			for (FConstPlayerControllerIterator ite = GetWorld()->GetPlayerControllerIterator(); ite; ite++) {
				ABasePlayerController* BasePlayerController = Cast< ABasePlayerController>(*ite);
				if (BasePlayerController) {
					BasePlayerController->ReadyEndTime = ReadyEndTime = GetWorld()->GetTimeSeconds();
					RequestResPawn(BasePlayerController->GetCharacter(), BasePlayerController);
				}
			}

			SetMatchState(MatchState::WarringBegin);
		}
	}
}






//FMap_Information* AServerGameMode::GetMap_Information()
//{
//	return nullptr;//FMap_InforPtr;
//}
