// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuOverlay.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ListView.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "MyFPSGame/UI/Menu/MenuHUD.h"


bool UMenuOverlay::Initialize(){
	if (!Super::Initialize()) {
		return false;
	}
	if (bt_friend) {
		bt_friend->OnClicked.AddDynamic(this, &ThisClass::FriendButtonButtonClicked);
	}
	if (bt_start) {
		bt_start->OnClicked.AddDynamic(this, &ThisClass::PlayerStartButtonClicked);
	}

	return true;
}

void UMenuOverlay::FriendButtonButtonClicked(){
	UCanvasPanelSlot* a = Cast<UCanvasPanelSlot>(cp_friend->Slot);
	FVector2D currleft = a->GetPosition();
	FVector2D currsize = a->GetSize();
	if (currleft.X != -100 || currsize.X != 100) {
		currleft.X = -100;
		currsize.X = 100;
	}
	else {
		currleft.X = -620;
		currsize.X = 620;
	}
	a->SetPosition(currleft);//1300/1820 , 100
	a->SetSize(currsize);
	

	UItem_Data * pData = NewObject<UItem_Data>();
	pData->Name = FString("666");
	pData->IconID = FString("1");
	//lv_friend->construce
	friendArry.Add(pData);
	lv_friend->AddItem(pData);
}

void UMenuOverlay::PlayerStartButtonClicked(){
	//UE_LOG(LogTemp, Warning, TEXT("PlayerStartButtonClicked"));
	StartState = !StartState;
	if (StartState) {
		if (!m_HUD && GetWorld()) {
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController) {
				m_HUD = Cast<AMenuHUD>(PlayerController->GetHUD());
			}
		}
		if (m_HUD)m_HUD->GoStart();
	}
	else if(tb_start){
		
	}

}

void UMenuOverlay::SetHUDTime(int32 TimeRuning){
	int Minuts = FMath::FloorToInt(TimeRuning / 60.f);
	int Seconds = TimeRuning - Minuts * 60;
	if (tb_start && TimeRuning > 0) {
		tb_start->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minuts, Seconds)));
	}
	else if(tb_start){
		FString StrStart = UTF8_TO_TCHAR("开始");
		tb_start->SetText(FText::FromString(StrStart));
	}
}

void UMenuOverlay::setInfo(int id, FString name){
	tb_name->SetText(FText::FromString(name));

}
