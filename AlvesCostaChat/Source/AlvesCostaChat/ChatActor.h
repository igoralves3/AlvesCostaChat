// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"


//socket lib
#include "UObject/NoExportTypes.h"
#include "Networking.h" // Required for FSocket

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ChatActor.generated.h"

// Delegate para Blueprint
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSocketMessageReceived, const FString&, Message);


UCLASS()
class ALVESCOSTACHAT_API AChatActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChatActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


    //socket methods
public:
    //void CreateSocket();

    // Connects to a specified IP address and port
    UFUNCTION(BlueprintCallable, Category = "Socket")
    bool Connect(FString IPAddress, int32 Port);

    // Sends data over the socket
    UFUNCTION(BlueprintCallable, Category = "Socket")
    bool SendData(const TArray<uint8>& DataToSend);

    UFUNCTION(BlueprintCallable, Category = "Socket")
    bool SendDataString(FString DataToSend);

    UFUNCTION(BlueprintCallable, Category = "Socket")
    bool SendDataUserMessage(FString user, FString DataToSend);

    // Receives data from the socket
    UFUNCTION(BlueprintCallable, Category = "Socket")
    TArray<uint8> ReceiveData();

    // Receives data from the socket with String
    UFUNCTION(BlueprintCallable, Category = "Socket")
    FString ReceiveDataString();
   
    UFUNCTION(blueprintCallable, Category = "Socket")
    bool SendResurge(FString DataToSend,bool Private, FString NameUser,FString lobbyName);

    

    // Disconnects the socket
    UFUNCTION(BlueprintCallable, Category = "Socket")
    void Disconnect();

    // Evento visível no Blueprint
    UPROPERTY(BlueprintAssignable, Category = "Socket")
    FOnSocketMessageReceived OnMessageReceived;

protected:
    FSocket* ClientSocket;
    FIPv4Endpoint RemoteEndpoint;



};
