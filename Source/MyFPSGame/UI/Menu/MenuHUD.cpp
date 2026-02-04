// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuHUD.h"
#include "MyFPSGame/UI/MyUserWidget.h"
#include "MenuOverlay.h"
#include "MyFPSGame/TCP/TCPActor.h"
#include "Kismet/GameplayStatics.h"
#include "MyFPSGame/Enum/AllEnum.h"
#include "MyFPSGame/GameInstance/BaseGameInstance.h"

void AMenuHUD::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	//FactionSelection->CTButton->IsHovered()

	if (MenuOverlay && MenuOverlay->StartState) {
		UpDateStartTime();
	}
	else if(MenuOverlay){
		MenuOverlay->SetHUDTime(0);
		CountdownSecond = 0;
		StartDownTime = 0;
	}

}

void AMenuHUD::AddMyUserWidget(){
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && UserWidgetClass) {
		UserWidget = CreateWidget<UMyUserWidget>(PlayerController, UserWidgetClass);
		UserWidget->AddToViewport();
		UserWidget->MenuSetUp();
	}
}

void AMenuHUD::AddMenuOverlay(){
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && MenuOverlayClass) {
		MenuOverlay = CreateWidget<UMenuOverlay>(PlayerController, MenuOverlayClass);
		MenuOverlay->AddToViewport();
	}
}

void AMenuHUD::LoginRs(int res, int userid, FString name){
	if (res == 2) {
		//name.ut
		UE_LOG(LogTemp, Warning, TEXT("%s"), *name);
		UserWidget->SetVisibility(ESlateVisibility::Hidden);
		AddMenuOverlay();
		m_id = userid;
		m_name = name;

		MenuOverlay->setInfo(m_id, m_name);
	}
}

void AMenuHUD::GoStart(){
	//UE_LOG(LogTemp, Warning, TEXT("GoStart"));
	_STRU_PLAYER_STARTER_RQ rq;
	UBaseGameInstance* BaseGameInstance = Cast<UBaseGameInstance>(GetGameInstance());
	if (BaseGameInstance) {
		rq.userID = BaseGameInstance->GetUserId();
		memcpy(rq.LoginKey,*BaseGameInstance->GetLoginKey(), BaseGameInstance->GetLoginKey().Len());
		FString astr;
		SendData((char*)&rq, sizeof(rq));
	}
	
	//SendData();
}

void AMenuHUD::SendData(char* buf, int len){
	//UE_LOG(LogTemp, Warning, TEXT("SendData"));
	if (m_TCPActor == nullptr) {
		TArray<AActor*> outActor;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATCPActor::StaticClass(), outActor);
		if (outActor.Num() > 0) {
			m_TCPActor = Cast<ATCPActor>(outActor[0]);
		}
	}
	m_TCPActor->Send_Data(buf, len);
}

void AMenuHUD::UpDateStartTime(){
	uint32 SecondsLeft = 0;
	if (StartDownTime == 0.f) StartDownTime = GetWorld()->GetTimeSeconds();
	SecondsLeft = FMath::CeilToInt(GetWorld()->GetTimeSeconds() - StartDownTime);
	if (CountdownSecond != SecondsLeft) {
		MenuOverlay->SetHUDTime(SecondsLeft);
	}



	CountdownSecond = SecondsLeft;
}

void AMenuHUD::BeginPlay(){
	Super::BeginPlay();
	AddMyUserWidget();
}
