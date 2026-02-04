// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_HUD_Widget.h"
#include "Components/TextBlock.h"

void UBase_HUD_Widget::SetDisplayText(FString TextToDisplay){
	if (DisplayText) {
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UBase_HUD_Widget::ShowPlayerNetRole(APawn* InPawn){
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;
	switch (LocalRole)
	{
	case ROLE_Authority:
		Role = FString("Server");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("MyClient");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("OtherClient");
		break;
	case ROLE_None:
		Role = FString("ERROR: ROLE_None");
		break;
	case ROLE_MAX:
		Role = FString("ERROR: ROLE_MAX");
		break;
	default:
		Role = FString("ERROR: ??????????");
		break;
	}
	FString LocalRoleString = FString::Printf(TEXT("角色权限: %s"), *Role);
	SetDisplayText(LocalRoleString);
}

void UBase_HUD_Widget::NativeDestruct(){
	RemoveFromParent();
	Super::NativeDestruct();
}
