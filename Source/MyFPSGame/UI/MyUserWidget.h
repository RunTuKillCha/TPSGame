// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYFPSGAME_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	~UMyUserWidget();

	UFUNCTION(BlueprintCallable)
	void MenuSetUp();
	
protected:

	virtual bool Initialize() override;

private:

	FString MapName;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinLocalButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinPublicButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextBoxIP;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TextBoxTel;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TextBoxPassWord;

	UPROPERTY(meta = (BindWidget))
	class UImage* BlackGround;

	FLatentActionInfo GoToNewMapDelay;

	//BlackGround
	UPROPERTY()
	class ATCPActor* m_TCPActor;

	UFUNCTION()
	void JoinLocalButtonClicked();

	UFUNCTION()
	void JoinPublicButtonClicked();

	UFUNCTION()
	void FGoToNewMapDelay();

	UFUNCTION()
	void LoginServer();

	UFUNCTION()
	bool GetActor();
	
};
