#include "TCP_Client.h"
#include "Networking.h"
#include "Async/ParallelFor.h"
#include "HAL/RunnableThread.h"
#include "MyFPSGame/TCP/TCPActor.h"

UTCP_Client* UTCP_Client::Tcp = nullptr;

UTCP_Client::UTCP_Client(){
    m_Actor = nullptr;
    FClientSocket = nullptr;
    m_RecvRunnable = nullptr;
    m_RecvRunnableThread = nullptr;
}

UTCP_Client::~UTCP_Client(){
    Tcp->RemoveFromRoot();

    if (FClientSocket) {
        FClientSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(FClientSocket);
        FClientSocket = nullptr;
    }
    if (m_RecvRunnableThread) {
        m_RecvRunnableThread->Kill();
        delete m_RecvRunnableThread;
        m_RecvRunnableThread = nullptr;
    }
    m_Actor = nullptr;
}

UTCP_Client* UTCP_Client::GetTCPClientInterFace(){
    if (Tcp == nullptr) { 
        Tcp = NewObject<UTCP_Client>(); 
        Tcp->AddToRoot();
    }
    return Tcp;
}

bool UTCP_Client::Connect(FString ip, int port , ATCPActor* tcpActor){
    if (FClientSocket != nullptr || m_RecvRunnableThread != nullptr || m_RecvRunnable!= nullptr) {
        return ReConnect(ip, port, tcpActor);
    }
    m_Actor = tcpActor;
    FIPv4Address Addrip;
    FIPv4Address::Parse(ip, Addrip);     //将传入的IPStr转为IPv4地址
    //创建一个addr存放ip地址和端口
    TSharedPtr<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    addr->SetIp(Addrip.Value);
    addr->SetPort(port);

    //创建客户端socket
    //if(!FClientSocket)
        FClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

    //连接成功
    if (FClientSocket->Connect(*addr)){
        // 收数据 -- 创建线程 CreateThread  WinAPI  strcpy  C/C++ RunTime 库函数 创建内存块
        //m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);     //( CreateThread 创建内存块 )
        //ParallelFor(1,[this](const int TaskId) {recv();});
        m_RecvRunnable = new FRecvRunnable();
        m_RecvRunnableThread = FRunnableThread::Create(m_RecvRunnable, TEXT("RecvThred"));

        return true;
    }
    //连接失败
    else{
        UE_LOG(LogTemp, Error, TEXT("Connect Failed!"));
        return false;
    }
    return false;
}

bool UTCP_Client::ReConnect(FString ip, int port, class ATCPActor* tcpActor){
    if (FClientSocket) {
        FClientSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(FClientSocket);
        FClientSocket = nullptr;
    }
    if (m_RecvRunnableThread != nullptr) {
        m_RecvRunnableThread->Kill();
        delete m_RecvRunnableThread;
        m_RecvRunnableThread = nullptr;
    }
    if (m_RecvRunnable != nullptr) {
        delete m_RecvRunnable;
        m_RecvRunnable = nullptr;
    }

    return Connect(ip, port, tcpActor);
}

void UTCP_Client::recv(){

    
}

//#include "iostream"
bool UTCP_Client::send(char * szbuf, int nlen){
    if (!szbuf || nlen <= 0) return false;

    int DataLen = nlen + 4;//包+大小的大小
    //TArray<char> vecbuf;
    //vecbuf.Reserve(DataLen);
    //vecbuf.SetNum(DataLen);
    char* buf = (char*)malloc(DataLen);

    //char* buf = &*vecbuf.begin();
    char* tmp = buf;
    *(int*)tmp = nlen;
    tmp += sizeof(int);

    memcpy(tmp, szbuf, nlen);
    int res;

    bool fg = FClientSocket->Send((uint8*)buf, DataLen, res);

    free(buf);
    return true;
}

FRecvRunnable::FRecvRunnable(){
    stopping = false;
}

FRecvRunnable::~FRecvRunnable(){
    //Tcp->FClientSocket->Close();
    //ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Tcp->FClientSocket);
    //Tcp->FClientSocket = nullptr;
}

bool FRecvRunnable::Init(){
    Tcp = UTCP_Client::GetTCPClientInterFace();
    if (!Tcp) {
        return false;
    }
    stopping = false;
    return true;
}

uint32 FRecvRunnable::Run(){
    //Tcp->recv();
    int nPackSize = 0; // 存储包大小
    int32 nRes = 0;
    while (!stopping) {
        //接收先接收包大小 在接受数据包
        if (!Tcp->FClientSocket->Recv((uint8*)&nPackSize, sizeof(int), nRes)) {
            UE_LOG(LogTemp, Warning, TEXT("recv fial"));
            return -1;
        }
        //从接收缓冲区拷贝包大小
        int offset = 0;
        char* buf = new char[nPackSize];
        while (nPackSize)
        {
            Tcp->FClientSocket->Recv((uint8*)(buf + offset), nPackSize, nRes);
            if (nRes > 0) {
                nPackSize -= nRes;
                offset += nRes;
            }
        }
        if (Tcp->m_Actor) {
            Tcp->m_Actor->recv(buf, offset);  //需要在这个函数回收 堆区 空间
        }

    }
    return 0;
}

void FRecvRunnable::Stop(){
    stopping = true;
}

void FRecvRunnable::Exit(){
    stopping = true;
}
