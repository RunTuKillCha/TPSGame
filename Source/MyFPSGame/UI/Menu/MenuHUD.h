// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYFPSGAME_API AMenuHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;
	void AddMyUserWidget();
	void AddMenuOverlay();
	void LoginRs(int res, int userid, FString name);

	//方法
	void GoStart();

	UPROPERTY(EditAnywhere, Category = "Menu")
	TSubclassOf<class UUserWidget> UserWidgetClass;
	UPROPERTY()
	class UMyUserWidget* UserWidget;

	UPROPERTY(EditAnywhere, Category = "Menu")
	TSubclassOf<class UUserWidget> MenuOverlayClass;
	UPROPERTY()
	class UMenuOverlay* MenuOverlay;

private:
	void SendData(char* buf, int len);
	void UpDateStartTime();
	
protected:
	virtual void BeginPlay() override;

private:
	int m_id;
	FString m_name;

	UPROPERTY()
	class ATCPActor* m_TCPActor;

	UPROPERTY()
	double StartDownTime = 0.f;

	uint32 CountdownSecond = 0;
};
