// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "MyFPSGame/UI/BaseHUD.h"
#include "MyFPSGame/UI/CharacterOverlay.h"
#include "MyFPSGame/UI/Announcement.h"
#include "MyFPSGame/UI/FactionSelection.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "Net/UnrealNetwork.h"
#include "MyFPSGame/GameMode/ServerGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MyFPSGame/Character/BaseCharacter.h"
#include "MyFPSGame/Character/FactionReference.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

void ABasePlayerController::ServerDebug_Implementation(){
	GetWorld()->GetAuthGameMode<AServerGameMode>()->ChooseFaction(this);
}

void ABasePlayerController::BeginPlay(){
	Super::BeginPlay();

	BaseHUD = Cast<ABaseHUD>(GetHUD());
	
}

void ABasePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState);

	DOREPLIFETIME(ThisClass, LevelStartingTime);
	DOREPLIFETIME(ThisClass, ReadyTime);
	DOREPLIFETIME(ThisClass, ReadyEndTime);
	DOREPLIFETIME(ThisClass, RoundStartingTime);
	DOREPLIFETIME(ThisClass, RoundOverTime);


	DOREPLIFETIME(ThisClass, WaitPlayerDuration);
	DOREPLIFETIME(ThisClass, WarmupDuration);
	DOREPLIFETIME(ThisClass, RoundStartDuration);
	DOREPLIFETIME(ThisClass, RoundTime);
	DOREPLIFETIME(ThisClass, RoundEndTime);

	DOREPLIFETIME(ThisClass, bHiddenCharacterOverlay);
	DOREPLIFETIME(ThisClass, bHiddenFactionSelection);
	DOREPLIFETIME(ThisClass, ClientSetCarma);
}

void ABasePlayerController::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	
	SetHUDTime();

	TimeRunningTime += DeltaTime;
	if (IsLocalController() && TimeRunningTime > TimeFrequencyUpdates) {//每5s与服务器同步一次
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeRunningTime = 0.f;
	}

	//ParameterCorrections();

	PollInit();
}


void ABasePlayerController::PollInit() {
	if (CharacterOverlay == nullptr) {
		if (BaseHUD && BaseHUD->CharacterOverlay) {
			CharacterOverlay = BaseHUD->CharacterOverlay;
			if (CharacterOverlay) {
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
				SetHUDWeapon(HUDEquipWeapon, HUDMainWeapon);
				SetHUDWeaponAmmo(HUDAmmo);
				SetHUDCarryAmmo(HUDCarryAmmo);
				SetHUDTimeRemaining(HUDTimeRemaining);
			}
		}
	}
}

void ABasePlayerController::SetElimDelay(float Delay) {
	ElimDelay = Delay;
}

float ABasePlayerController::GetElimDelay() {
	return ElimDelay;
}

void ABasePlayerController::SetHUDHealth(float Health, float MaxHealth) {
	if (BaseHUD && BaseHUD->CharacterOverlay && 
		BaseHUD->CharacterOverlay->HealthBar && BaseHUD->CharacterOverlay->HealthText) {

		BaseHUD->CharacterOverlay->HealthBar->SetPercent(Health / MaxHealth);
		BaseHUD->CharacterOverlay->HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d"), (int)(Health + 0.5f))));
	}
	else {
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
	
}

void ABasePlayerController::SetHUDScore(float Score){
	if (BaseHUD && BaseHUD->CharacterOverlay && 
		BaseHUD->CharacterOverlay->ScoreAmount) {

		BaseHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(FString::Printf(TEXT("%d"), (int)(Score + 0.5f))));
	}
	else {
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void ABasePlayerController::SetHUDWeapon(bool EquipWeapon, bool MainWeapon){
	if (BaseHUD && BaseHUD->CharacterOverlay && 
		BaseHUD->CharacterOverlay->EquippedWeapon && BaseHUD->CharacterOverlay->MainWeapon) {
			
		BaseHUD->CharacterOverlay->EquippedWeapon->SetText(EquipWeapon ? FText::FromString(TEXT("AK47")) : FText::FromString(TEXT("NULL")));
		BaseHUD->CharacterOverlay->MainWeapon->SetText(MainWeapon ? FText::FromString(TEXT("AK47")) : FText::FromString(TEXT("NULL")));
	}
	else {
		bInitializeCharacterOverlay = true;
		HUDEquipWeapon = EquipWeapon;
		HUDMainWeapon = MainWeapon;
	}
}

void ABasePlayerController::SetHUDDefeats(int32 Defeats){
	if (BaseHUD && BaseHUD->CharacterOverlay && 
		BaseHUD->CharacterOverlay->DefeatsAmount) {

		BaseHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(FString::Printf(TEXT("%d"), Defeats)));
	}
	else {
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

void ABasePlayerController::SetHUDWeaponAmmo(int32 Ammo){
	if (BaseHUD && BaseHUD->CharacterOverlay && 
		BaseHUD->CharacterOverlay->WeaponAmmoAmount) {

		BaseHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(FString::Printf(TEXT("%d"), Ammo)));
	}
	else {
		bInitializeCharacterOverlay = true;
		HUDAmmo = Ammo;
	}

}

void ABasePlayerController::SetHUDCarryAmmo(int32 CarryAmmo){
	if (BaseHUD && BaseHUD->CharacterOverlay && 
		BaseHUD->CharacterOverlay->CarryAmmoAmount) {

		BaseHUD->CharacterOverlay->CarryAmmoAmount->SetText(FText::FromString(FString::Printf(TEXT("%d"), CarryAmmo)));
	}
	else {
		bInitializeCharacterOverlay = true;
		HUDCarryAmmo = CarryAmmo;
	}
}

void ABasePlayerController::SetHUDTimeRemaining(int32 TimeRemaining){

	int Minuts = FMath::FloorToInt(TimeRemaining / 60.f);
	int Seconds = TimeRemaining - Minuts * 60;

	if (MatchState == MatchState::InProgress) {
		if (BaseHUD && BaseHUD->Announcement) {
			if (BaseHUD->Announcement->WarmupTime) {
				BaseHUD->Announcement->AnnouncementText->SetText(FText::FromString(FString::Printf(TEXT("等待其他玩家加入"))));
				BaseHUD->Announcement->WarmupTime->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minuts, Seconds)));
			}
		}
	}else{
		if (BaseHUD && BaseHUD->CharacterOverlay) {
			if (BaseHUD->CharacterOverlay->TimeRemaining) {
				BaseHUD->CharacterOverlay->TimeRemaining->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minuts, Seconds)));
			}
		}
		else {
			bInitializeCharacterOverlay = true;
			HUDTimeRemaining = TimeRemaining;
		}
	}
}

void ABasePlayerController::SetHUDTime(){
	uint32 SecondsLeft = 0;
	if (MatchState == MatchState::InProgress) {//等待玩家倒计时结束进入热身状态
		SecondsLeft = FMath::CeilToInt(WaitPlayerDuration - (GetServerTime() - LevelStartingTime));
	}
	if (MatchState == MatchState::WarmupTime) {
		SecondsLeft = FMath::CeilToInt(WarmupDuration - (GetServerTime() - ReadyTime));
	}
	if (MatchState == MatchState::WarringBegin) {//回合开始等待时间
		SecondsLeft = FMath::CeilToInt(RoundStartDuration - (GetServerTime() - ReadyEndTime));
	}
	if (MatchState == MatchState::Warring) {
		SecondsLeft = FMath::CeilToInt(RoundTime - (GetServerTime() - RoundStartingTime));
	}
	if (MatchState == MatchState::WarringEnd) {
		SecondsLeft = FMath::CeilToInt(RoundEndTime - (GetServerTime() - RoundOverTime));
	}

	if (CountdownSecond != SecondsLeft) {
		SetHUDTimeRemaining(SecondsLeft);
	}



	CountdownSecond = SecondsLeft;
}





void ABasePlayerController::ServerRequestServerTime_Implementation(float Time)
{
	float ServerTime = GetWorld()->GetTimeSeconds();
	ClientRequestServerTime(Time, ServerTime);
}

void ABasePlayerController::ClientRequestServerTime_Implementation(float Time, float TimeServer)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - Time;//数据延迟时长
	float CurrentServerTime = TimeServer + (0.5f * RoundTripTime);//计算真正的服务器时间
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();//计算出服务器先运行的时间
}

float ABasePlayerController::GetServerTime()
{
	if (HasAuthority()) {
		return GetWorld()->GetTimeSeconds();
	}
	else {
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
}




void ABasePlayerController::ChooseFaction(){

	bHiddenCharacterOverlay = true;
	bHiddenFactionSelection = false;
	ClientSetCarma = TEXT("FactionSelectsCameras");
	
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
}

void ABasePlayerController::OnRep_ClientSetCarma() {
	if (ClientSetCarma == TEXT("MyCamera") && GetCharacter()) {
		ABaseCharacter* BaseChar = Cast<ABaseCharacter>(GetPawn());
		UCameraComponent * Camera = Cast<UCameraComponent>(BaseChar->GetCamera());
		CurrentCameraActor = BaseChar;
		if (Camera) {
			// 3. 激活相机组件（关键：确保该相机是当前生效的视角）
			Camera->Activate(); // 若之前有其他相机激活，先禁用其他相机
			// 4. 将角色Actor作为视角目标（SetViewTargetWithBlend仅支持AActor*）
			SetViewTargetWithBlend(BaseChar, 0.2f); // 0.2秒淡入，流畅切换
			
			//SetViewTarget(CurrentCamera);
			//SetViewTargetWithBlend(CurrentCamera, 0.2f);
		}
		//CurrentCameraActor = nullptr;
		return;
	}
	TArray<AActor*> PlayerStart;
	UGameplayStatics::GetAllActorsOfClass(this, ACameraActor::StaticClass(), PlayerStart);
	for (auto It = PlayerStart.begin(); It != PlayerStart.end(); ++It) {
		if (UKismetSystemLibrary::GetDisplayName(*It) == ClientSetCarma) {
			SetViewTarget(*It);
			CurrentCameraActor = (*It);
		}
	}
}
void ABasePlayerController::ParameterCorrections() {
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *ClientSetCarma);
	//if (CurrentCameraActor && CurrentCameraActor != GetViewTarget()) {
		//SetViewTarget(CurrentCamera);
	//}
	//if(GetPawn() && Cast<ABaseCharacter>(GetPawn())->GetCamera() && !CurrentCameraActor) {
	//	AActor* Camera = Cast<AActor>(Cast<ABaseCharacter>(GetPawn())->GetCamera());
	//	if (Camera) {
	//		SetViewTarget(Camera);
	//		CurrentCamera = Camera;
	//	}
	//	
	//}
}


void ABasePlayerController::OnRep_bHiddenCharacterOverlay(){
	BaseHUD = BaseHUD ? Cast<ABaseHUD>(GetHUD()) : BaseHUD;
	if (BaseHUD && BaseHUD->CharacterOverlay) {
		if (bHiddenCharacterOverlay) {
			BaseHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
		else {
			BaseHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
		}
		
	}
}

void ABasePlayerController::OnRep_bHiddenFactionSelection(){
	BaseHUD = BaseHUD ? Cast<ABaseHUD>(GetHUD()) : BaseHUD;
	if (BaseHUD && BaseHUD->FactionSelection) {
		if (bHiddenFactionSelection) {
			BaseHUD->FactionSelection->SetVisibility(ESlateVisibility::Hidden);
			BaseHUD->FactionSelection->GetActor();
			BaseHUD->FactionSelection->CT_FactionReference->SetActorHiddenInGame(true);
			BaseHUD->FactionSelection->T_FactionReference->SetActorHiddenInGame(true);
		}
		else {
			BaseHUD->FactionSelection->SetVisibility(ESlateVisibility::Visible);
			BaseHUD->FactionSelection->GetActor();
			BaseHUD->FactionSelection->CT_FactionReference->SetActorHiddenInGame(false);
			BaseHUD->FactionSelection->T_FactionReference->SetActorHiddenInGame(false);
		}

	}
}

void ABasePlayerController::OnRep_PlayerFaction()
{
}

void ABasePlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController()) {
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABasePlayerController::OnMatchStateSet(FName State){
	MatchState = State;

	if (MatchState == MatchState::InProgress) {//等待玩家倒计时结束进入热身状态
	}
	if (MatchState == MatchState::WarmupTime) {
		HandleMatchHasStarted();
		ReadyTime = GetServerTime();

		

	}
	if (MatchState == MatchState::WarringBegin) {//回合开始等待时间
		ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(GetCharacter());
		if (MyCharacter && MyCharacter->GetCombat()) {
			MyCharacter->bDisableCharacterMove = true;
			MyCharacter->GetCombat()->FireButtonPressed(false);
		}
	}
	else {
		ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(GetCharacter());
		if (MyCharacter && MyCharacter->GetCombat()) {
			MyCharacter->bDisableCharacterMove = false;
		}
	}
	if (MatchState == MatchState::Warring) {
	}
	if (MatchState == MatchState::WarringEnd) {
	}


}

void ABasePlayerController::OnRep_MatchState(){

	if (MatchState == MatchState::WarmupTime) {
		HandleMatchHasStarted();
	}
}



void ABasePlayerController::HandleMatchHasStarted(){
	BaseHUD = BaseHUD == nullptr ? Cast<ABaseHUD>(GetHUD()) : BaseHUD;
	if (BaseHUD && BaseHUD->Announcement) {
			BaseHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
			BaseHUD->CharacterOverlay->TimeRemaining->SetVisibility(ESlateVisibility::Visible);
	}
}

