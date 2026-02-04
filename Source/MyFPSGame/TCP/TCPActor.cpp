#include "TCPActor.h"
#include "Networking.h"
#include "TCP_Client.h"
#include "MyFPSGame/UI/Menu/MenuHUD.h"
#include "MyFPSGame/UI/MyUserWidget.h"
#include "MyFPSGame/Enum/AllEnum.h"
#include "MyFPSGame/GameInstance/BaseGameInstance.h"

ATCPActor::ATCPActor(){
	PrimaryActorTick.bCanEverTick = false;
	TCP_Client = UTCP_Client::GetTCPClientInterFace();
	m_TypeToFunction.Empty();// = { { PackType::_ENUM_PACK_BASE , nullptr} };
	SetTMap();
}

void ATCPActor::BeginPlay() {
	Super::BeginPlay();
}

void ATCPActor::SetTMap(){
	m_TypeToFunction.Add(PackType::_ENUM_PACK_LOGIN_RS, &ThisClass::loginRs);
}

void ATCPActor::Connect_DefServer(){
	bool res = TCP_Client->Connect("192.168.154.128", 25560 ,this);
	
}

void ATCPActor::Send_Data(char* buf, int len){
	bool res = TCP_Client->send(buf, len);
}

void ATCPActor::recv(char* buf, int len){
	PackType* type = (PackType* )buf;
	//UE_LOG(LogTemp, Warning, TEXT("%d"), (int)*type);
	PFUN pf = m_TypeToFunction.Contains(*type) ? m_TypeToFunction[*type] : nullptr;
	if (pf != nullptr) {//调用
		AsyncTask(ENamedThreads::GameThread, [this, pf , buf ,len]() {	
			(this->*pf)(buf, len);
		});
	}

}

void ATCPActor::loginRs(char* buf, int len){
	STRU_LOGIN_RS* rs = (STRU_LOGIN_RS*)buf;
	if (!m_MenuHUD) {
		m_MenuHUD = Cast<AMenuHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	}
	if (m_MenuHUD) {
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *rs->Name);
		FString name = UTF8_TO_TCHAR(rs->Name);
		UBaseGameInstance* BaseGameInstance = Cast<UBaseGameInstance>(GetGameInstance());
		if(BaseGameInstance)BaseGameInstance->SetUserInfo(rs->userid, name, rs->LoginKey);
		m_MenuHUD->LoginRs(rs->result, rs->userid, name);
		TCP_Client->Connect(rs->ip,rs->port,this);
	}
	
}

void ATCPActor::startRs(char* buf, int len){
}

void ATCPActor::chatRs(char* buf, int len){
}
