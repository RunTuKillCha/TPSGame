#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYFPSGAME_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void SetUserInfo(int id, FString name, FString Key);

	int GetUserId() { return m_userid; }
	FString GetName() { return m_Name; }
	FString GetLoginKey() { return m_LoginKey; }
private:
	int m_userid;
	FString m_Name;
	FString m_LoginKey;
	
	
};
