// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteManager.h"
#include "BGActor.H"
#include "Kkwalla.h"

const int32 MS_TIME = 1000;

// Sets default values
ANoteManager::ANoteManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TimeSec = 0;

	bAnyKeyDown =false;
	OnKeyDownTime = 0;
}

// Called when the game starts or when spawned
void ANoteManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANoteManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	int32 DeltaTimeSec = static_cast<int32>(DeltaTime * MS_TIME);
	TimeSec += DeltaTimeSec;


}

void ANoteManager::TouchInput(const FVector2D& InPos)
{
	bAnyKeyDown = true;
}

