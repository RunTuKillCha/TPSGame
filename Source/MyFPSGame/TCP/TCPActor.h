#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TCPActor.generated.h"

using PFUN = void (ATCPActor::*)(char* buf, int len);

UCLASS()
class MYFPSGAME_API ATCPActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATCPActor();

	UFUNCTION(BlueprintCallable)
	void Connect_DefServer();

	void Send_Data(char* buf, int len);
	void recv(char* buf, int len);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetTMap();

	void loginRs(char* buf, int len);
	void startRs(char* buf, int len);
	void chatRs(char* buf, int len);


public:
	class UTCP_Client* TCP_Client;
	UPROPERTY()
	class AMenuHUD* m_MenuHUD;
	
	TMap<PackType, PFUN> m_TypeToFunction;
};
