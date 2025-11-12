// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatActor.h"

#include "ChatActor.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Networking.h"
//#include "IPv4Address.h"
//#include "IPv4Endpoint.h"


// Sets default values
AChatActor::AChatActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChatActor::BeginPlay()
{
	Super::BeginPlay();
    //CreateSocket();
	
}

// Called every frame
void AChatActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
}


//implementação socket



bool AChatActor::Connect(FString IPAddress, int32 Port)
{
    
    FIPv4Address ParsedIP;
    if (!FIPv4Address::Parse(TEXT("127.0.0.1"), ParsedIP))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid IP Address: %s"), *IPAddress);
        return false;
    }

    RemoteEndpoint = FIPv4Endpoint(ParsedIP, 5000);

    ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("DefaultSocket"), false);
    if (!ClientSocket)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create socket."));
        return false;
    }
    
    // Por exemplo em algum método Init:
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    FSocket* Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("MyClientSocket"), false);


    // Set socket to non-blocking mode (optional, but often desired for responsiveness)
    ClientSocket->SetNonBlocking(true);

    FIPv4Address IP;
    FIPv4Address::Parse(TEXT("127.0.0.1"), IP);
    TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
    Addr->SetIp(IP.Value);
    Addr->SetPort(5000);

   // bool bConnected = ClientSocket->Connect(*RemoteEndpoint.To  InternetAddr());
    bool bConnected = ClientSocket->Connect(*Addr);
    if (!bConnected)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to connect to %s:%d (may be non-blocking)"), *IPAddress, Port);
        // In non-blocking mode, Connect might return false immediately even if connection is pending.
        // You'll need to check GetConnectionState() later.
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully connected to %s:%d"), *IPAddress, Port);
    }

    return bConnected;
    

}
/*
void AChatActor::Connect(FString IPAddress, int32 Port)
{
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

    // Cria socket TCP
    ClientSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("ChatClientSocket"), false);

    // Cria endereço de destino
    FIPv4Address IP;
    if (!FIPv4Address::Parse(IPAddress, IP))
    {
        UE_LOG(LogTemp, Error, TEXT("Endereço IP inválido: %s"), *ServerIP);
        return;
    }

    TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
    Addr->SetIp(IP.Value);
    Addr->SetPort(ServerPort);

    // Tenta conectar
    bool bConnected = ClientSocket->Connect(*Addr);
    if (bConnected)
    {
        UE_LOG(LogTemp, Log, TEXT("✅ Conectado ao servidor em %s:%d"), *ServerIP, ServerPort);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Falha ao conectar ao servidor em %s:%d"), *ServerIP, ServerPort);
    }
}
*/

bool AChatActor::SendData(const TArray<uint8>& DataToSend)
{
    if (!ClientSocket || ClientSocket->GetConnectionState() != SCS_Connected)
    {
        UE_LOG(LogTemp, Error, TEXT("Socket not connected, cannot send data."));
        return false;
    }

    int32 BytesSent = 0;
    bool bSuccess = ClientSocket->Send(DataToSend.GetData(), DataToSend.Num(), BytesSent);

    if (!bSuccess || BytesSent != DataToSend.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to send all data. Sent: %d, Total: %d"), BytesSent, DataToSend.Num());
        return false;
    }

    return true;
}


bool AChatActor::SendDataString(FString DataToSend) {
    FString Message = DataToSend;

    // Converte para bytes UTF-8
    FTCHARToUTF8 Converter(*Message);
    TArray<uint8> ByteArray;
    ByteArray.Append((uint8*)Converter.Get(), Converter.Length());

    // Envia via socket
    int32 BytesSent = 0;
    if (ClientSocket->Send(ByteArray.GetData(), ByteArray.Num(), BytesSent))
    {
        UE_LOG(LogTemp, Log, TEXT("Mensagem enviada: %s (%d bytes)"), *Message, BytesSent);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Falha ao enviar mensagem"));
        return false;
    }

}

bool AChatActor::SendDataUserMessage(FString user, FString DataToSend)
{
    FString Message = user + ": " + DataToSend;

    // Converte para bytes UTF-8
    FTCHARToUTF8 Converter(*Message);
    TArray<uint8> ByteArray;
    ByteArray.Append((uint8*)Converter.Get(), Converter.Length());

    // Envia via socket
    int32 BytesSent = 0;
    if (ClientSocket->Send(ByteArray.GetData(), ByteArray.Num(), BytesSent))
    {
        UE_LOG(LogTemp, Log, TEXT("Mensagem enviada: %s (%d bytes)"), *Message, BytesSent);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Falha ao enviar mensagem"));
        return false;
    }
}
TArray<uint8> AChatActor::ReceiveData()
{
    TArray<uint8> ReceivedData;
    if (!ClientSocket || ClientSocket->GetConnectionState() != SCS_Connected)
    {
        return ReceivedData;
    }

    uint32 Size;
    while (ClientSocket->HasPendingData(Size))
    {
        ReceivedData.SetNumUninitialized(Size);
        int32 BytesRead = 0;
        ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead);
        ReceivedData.SetNum(BytesRead); // Adjust size to actual bytes read

        FString Msg = FString(UTF8_TO_TCHAR((ReceivedData.GetData())));
        OnMessageReceived.Broadcast(Msg);
    }

    return ReceivedData;
}

FString AChatActor::ReceiveDataString()
{
    FString ResultMessage = "";
    TArray<uint8> ReceivedData;
    if (!ClientSocket || ClientSocket->GetConnectionState() != SCS_Connected)
    {
        return "";
    }

    uint32 Size;
    while (ClientSocket->HasPendingData(Size))
    {
        ReceivedData.SetNumUninitialized(Size);
        int32 BytesRead = 0;
        ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead);
        ReceivedData.SetNum(BytesRead); // Adjust size to actual bytes read

        FUTF8ToTCHAR Converter((const ANSICHAR*)ReceivedData.GetData(), BytesRead);
        FString Message(Converter.Get(), Converter.Length());
        UE_LOG(LogTemp, Log, TEXT("📩 %s"), *Message);

        ResultMessage = *Message;
    }

    return ResultMessage;
}

bool AChatActor::SendResurge(FString DataToSend, bool Private, FString NameUser, FString lobbyName)
{
    FString stringPrivate;
        if (Private)
        {
            stringPrivate = "S";
        }
        else
        {
            stringPrivate = "N";
        }
        
        FString Message = NameUser +" :"+DataToSend +":"+stringPrivate + " : " + lobbyName;
        FTCHARToUTF8 Converter(*Message);
        TArray<uint8> ByteArray;
        ByteArray.Append((uint8*)Converter.Get(), Converter.Length());

        // Envia via socket
        int32 BytesSent = 0;
        if (ClientSocket->Send(ByteArray.GetData(), ByteArray.Num(), BytesSent))
        {
            UE_LOG(LogTemp, Log, TEXT("Mensagem enviada: %s (%d bytes)"), *Message, BytesSent);
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Falha ao enviar mensagem"));
            return false;
        }
    
}

void AChatActor::Disconnect()
{
    if (ClientSocket)
    {
        ClientSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
        ClientSocket = nullptr;
        UE_LOG(LogTemp, Log, TEXT("Socket disconnected."));
    }
}





