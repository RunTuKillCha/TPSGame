// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FactionSelection.generated.h"

/**
 * 
 */
UCLASS()
class MYFPSGAME_API UFactionSelection : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY()
	class AFactionReference* CT_FactionReference;

	UPROPERTY()
	class AFactionReference* T_FactionReference;

	void GetActor();
protected:

	virtual bool Initialize() override;

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* CTButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* TButton;
	


	UFUNCTION()
	void CTButtonButtonClicked();

	UFUNCTION()
	void TButtonButtonClicked();



	UFUNCTION()
	void CTButtonButtonHovered();

	UFUNCTION()
	void TButtonButtonHovered();



	UFUNCTION()
	void CTButtonButtonUnHovered();

	UFUNCTION()
	void TButtonButtonUnHovered();
};
