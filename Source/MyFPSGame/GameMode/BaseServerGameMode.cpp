#include "BaseServerGameMode.h"




ABaseServerGameMode::ABaseServerGameMode()
{
}

void ABaseServerGameMode::BeginPlay(){
	Super::BeginPlay();//重写时调用父类功能
}

void ABaseServerGameMode::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);
}

void ABaseServerGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason){
	Super::EndPlay(EndPlayReason);
}

void ABaseServerGameMode::PostLogin(APlayerController* NewPlayer){
	Super::PostLogin(NewPlayer);
}

void ABaseServerGameMode::RequestResPawn(ACharacter* ElimmedCharacter, AController* ElimmedController) {
	
}