// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuOverlay.generated.h"

/**
 * 
 */
UCLASS()
class MYFPSGAME_API UMenuOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* bt_quit;
	UPROPERTY(meta = (BindWidget))
	UButton* bt_setting;
	UPROPERTY(meta = (BindWidget))
	UButton* bt_;
	UPROPERTY(meta = (BindWidget))
	UButton* bt_start;
	UPROPERTY(meta = (BindWidget))
	UButton* bt_friend;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* tb_start;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* tb_name;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* cp_friend;

	UPROPERTY(meta = (BindWidget))
	class UListView* lv_friend;

	UPROPERTY(BlueprintReadWrite)
	TArray<class UItem_Data*> friendArry;

	UPROPERTY()
	bool StartState = false;

protected:
	virtual bool Initialize() override;

private:
	UFUNCTION()
	void FriendButtonButtonClicked();

	UFUNCTION()
	void PlayerStartButtonClicked();


public:
	void setInfo(int id, FString name);

	UFUNCTION()
	void SetHUDTime(int32 TimeRuning);

private:
	UPROPERTY()
	AMenuHUD* m_HUD;


};

UCLASS(BlueprintType)
class MYFPSGAME_API UItem_Data : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FString IconID;
	UPROPERTY(BlueprintReadOnly)
	FString Name;
};
