// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"
#include "MyFPSGame/TCP/TCPActor.h"
#include "MyFPSGame/Enum/AllEnum.h"


UMyUserWidget::~UMyUserWidget(){
	
}

void UMyUserWidget::MenuSetUp(){
	//AddToViewport();
	//SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
	if (GetWorld()) {
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
	MapName = GetWorld()->GetMapName();
	GoToNewMapDelay = FLatentActionInfo(0, FMath::Rand(), TEXT("FGoToNewMapDelay"), this);//延时
	JoinLocalButton->SetVisibility(ESlateVisibility::Hidden);
	JoinPublicButton->SetVisibility(ESlateVisibility::Hidden);
	TextBoxIP->SetVisibility(ESlateVisibility::Hidden);
}

bool UMyUserWidget::Initialize()
{
	if (!Super::Initialize()) {
		return false;
	}
	if (JoinLocalButton) {
		JoinLocalButton->OnClicked.AddDynamic(this, &ThisClass::JoinLocalButtonClicked);
	}
	if (JoinPublicButton) {
		JoinPublicButton->OnClicked.AddDynamic(this, &ThisClass::JoinPublicButtonClicked);
	}
	if (JoinLocalButton) {
		LoginButton->OnClicked.AddDynamic(this, &ThisClass::LoginServer);
	}

	return true;
}

void UMyUserWidget::JoinLocalButtonClicked(){
	if (GetWorld()) {
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeGameOnly InputModeData;
			InputModeData.SetConsumeCaptureMouseDown(true);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
		GetWorld()->GetFirstPlayerController()->ClientTravel("127.0.0.1", TRAVEL_Absolute);
		UKismetSystemLibrary::Delay(this, 10.f, GoToNewMapDelay);
	}
	
	//GEngine->Exec(GetWorld(), TEXT("open 127.0.0.1"));
}

void UMyUserWidget::JoinPublicButtonClicked(){
	if (GetWorld()) {
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeGameOnly InputModeData;
			InputModeData.SetConsumeCaptureMouseDown(true);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
			if (TextBoxIP) {
				FString ServerIP = TextBoxIP->GetText().ToString();
				FString JoinTEXT("Join Server IP:");
				GEngine->AddOnScreenDebugMessage(
					-1, 3.f, FColor::Green,
					JoinTEXT + ServerIP
				);

				GetWorld()->GetFirstPlayerController()->ClientTravel(ServerIP, TRAVEL_Absolute);
				UKismetSystemLibrary::Delay(this, 10.f, GoToNewMapDelay);
			}
		}
		
	}
	
}

void UMyUserWidget::FGoToNewMapDelay(){
	if (GetWorld()) {
		if (GetWorld()->GetMapName() == MapName) {//如果加入失败
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController) {
				FInputModeUIOnly InputModeData;
				InputModeData.SetWidgetToFocus(TakeWidget());
				InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PlayerController->SetInputMode(InputModeData);
				PlayerController->SetShowMouseCursor(true);
			}

			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(
					-1, 3.f, FColor::Green,
					TEXT("Error: Failed to JoinServer!!")
				);
			}
		}
	}
}

void UMyUserWidget::LoginServer(){
	if (TextBoxTel && TextBoxPassWord) {
		FString Tel = TextBoxTel->GetText().ToString();
		FString Password = TextBoxPassWord->GetText().ToString();
		if (!m_TCPActor && !GetActor()) {
			return;
		}
		STRU_LOGIN_RQ rq;
		memcpy(rq.tel, TCHAR_TO_UTF8(*Tel), Tel.Len());
		memcpy(rq.password, TCHAR_TO_UTF8(*Password), Password.Len());
		m_TCPActor->Send_Data((char*)&rq,sizeof(rq));
	}
}

bool UMyUserWidget::GetActor(){
	if (m_TCPActor == nullptr) {
		TArray<AActor*> ArrActor;
		UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), ArrActor);
		for (AActor* actor : ArrActor){
			UKismetSystemLibrary::PrintString(GetWorld(), actor->GetName());
			m_TCPActor = Cast<ATCPActor>(actor);
			if (m_TCPActor) {
				//m_TCPActor->
				return true;
			}
		}
	}
	return false;
}


