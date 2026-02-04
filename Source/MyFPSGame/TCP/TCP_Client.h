// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HAL/Runnable.h"
#include "MyFPSGame/Enum/AllEnum.h"
#include "TCP_Client.generated.h"

/**
 * 
 */
UCLASS()
class MYFPSGAME_API UTCP_Client : public UObject
{
	GENERATED_BODY()
private:
	UTCP_Client();
	UTCP_Client(UTCP_Client&);

public:

	~UTCP_Client();

	UFUNCTION(BlueprintCallable)
	static UTCP_Client* GetTCPClientInterFace();

	UFUNCTION(BlueprintCallable)
	bool Connect(FString ip , int port , class ATCPActor* tcpActor);

	bool ReConnect(FString ip, int port, class ATCPActor* tcpActor);

	UFUNCTION(BlueprintCallable)
	void recv();
	//UFUNCTION(BlueprintCallable)
	bool send(char* szbuf, int nlen);
private:
	class FSocket* FClientSocket;//套接字
	FRunnable* m_RecvRunnable;
	FRunnableThread* m_RecvRunnableThread;
	static UTCP_Client* Tcp;
	class ATCPActor* m_Actor;


	friend class FRecvRunnable;
};



class FRecvRunnable : public FRunnable {
public:
	FRecvRunnable();
	~FRecvRunnable();
	virtual bool Init() override;  // 初始化 runnable 对象
	virtual uint32 Run() override; // 运行 runnable 对象
	virtual void Stop() override;  // 停止 runnable 对象,线程提前终止时被调用
	virtual void Exit() override;  // 退出 runnable 对象
private:
	UTCP_Client* Tcp;
	bool stopping;
};




