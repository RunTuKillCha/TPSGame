// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionSelection.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMesh.h"
#include "MyFPSGame/Character/FactionReference.h"
#include "MyFPSGame/PlayerController/BasePlayerController.h"
#include "MyFPSGame/PlayerState/BasePlayerState.h"
#include "MyFPSGame/Enum/AllEnum.h"
#include "MyFPSGame/GameMode/ServerGameMode.h"

bool UFactionSelection::Initialize()
{
	if (!Super::Initialize()) {
		return false;
	}
	if (CTButton) {
		CTButton->OnClicked.AddDynamic(this, &ThisClass::CTButtonButtonClicked);
		CTButton->OnHovered.AddDynamic(this, &ThisClass::CTButtonButtonHovered);
		CTButton->OnUnhovered.AddDynamic(this, &ThisClass::CTButtonButtonUnHovered);
	}
	if (TButton) {
		TButton->OnClicked.AddDynamic(this, &ThisClass::TButtonButtonClicked);
		TButton->OnHovered.AddDynamic(this, &ThisClass::TButtonButtonHovered);
		TButton->OnUnhovered.AddDynamic(this, &ThisClass::TButtonButtonUnHovered);
	}

	return true;
}

void UFactionSelection::CTButtonButtonClicked(){
	ABasePlayerController* BasePlayerController = GetWorld()->GetFirstPlayerController<ABasePlayerController>();
	ABasePlayerState* PlayerState = BasePlayerController ? Cast<ABasePlayerState>(BasePlayerController->PlayerState) : nullptr;
	if (PlayerState) {
		PlayerState->ServerSetFaction(EFaction::EF_CT);		
		BasePlayerController->SetInputMode(FInputModeGameOnly());
		BasePlayerController->SetShowMouseCursor(false);
	}
}

void UFactionSelection::TButtonButtonClicked(){
	ABasePlayerController* BasePlayerController = GetWorld()->GetFirstPlayerController<ABasePlayerController>();
	ABasePlayerState* PlayerState = BasePlayerController ? Cast<ABasePlayerState>(BasePlayerController->PlayerState) : nullptr;
	if (PlayerState) {
		PlayerState->ServerSetFaction(EFaction::EF_T);
		BasePlayerController->SetInputMode(FInputModeGameOnly());
		BasePlayerController->SetShowMouseCursor(false);
	}
}

void UFactionSelection::CTButtonButtonHovered(){
	if (CT_FactionReference == nullptr) GetActor();
	if (CT_FactionReference) {
		CT_FactionReference->SkeletalMeshComponent->SetRenderCustomDepth(true);
	}
}
void UFactionSelection::CTButtonButtonUnHovered(){
	if (CT_FactionReference == nullptr) GetActor();
	if (CT_FactionReference) {
		CT_FactionReference->SkeletalMeshComponent->SetRenderCustomDepth(false);
	}
}


void UFactionSelection::TButtonButtonHovered(){
	if (T_FactionReference == nullptr) GetActor();
	if (T_FactionReference) {
		T_FactionReference->SkeletalMeshComponent->SetRenderCustomDepth(true);
	}
}
void UFactionSelection::TButtonButtonUnHovered(){
	if (T_FactionReference == nullptr) GetActor();
	if (T_FactionReference) {
		T_FactionReference->SkeletalMeshComponent->SetRenderCustomDepth(false);
	}
}

void UFactionSelection::GetActor()
{
	if (CT_FactionReference == nullptr) {
		TArray<AActor*> ArrActor;
		UGameplayStatics::GetAllActorsOfClassWithTag(this, AActor::StaticClass(), FName(TEXT("CT_FactionReference")), ArrActor);
		for (auto It = ArrActor.begin(); It != ArrActor.end(); ++It) {
			if (UKismetSystemLibrary::GetDisplayName(*It) == TEXT("CT_FactionReference")) {
				CT_FactionReference = Cast<AFactionReference>(*It);
			}
		}
	}

	if (T_FactionReference == nullptr) {
		TArray<AActor*> ArrActor;
		UGameplayStatics::GetAllActorsOfClassWithTag(this, AActor::StaticClass(), FName(TEXT("T_FactionReference")), ArrActor);
		for (auto It = ArrActor.begin(); It != ArrActor.end(); ++It) {
			if (UKismetSystemLibrary::GetDisplayName(*It) == TEXT("T_FactionReference")) {
				T_FactionReference = Cast<AFactionReference>(*It);
			}
		}
	}

}
