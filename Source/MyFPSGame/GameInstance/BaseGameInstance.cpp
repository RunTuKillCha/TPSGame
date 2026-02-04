#include "BaseGameInstance.h"

void UBaseGameInstance::SetUserInfo(int id, FString name, FString Key){
	m_userid = id;
	m_Name = name;
	m_LoginKey = Key;
}
