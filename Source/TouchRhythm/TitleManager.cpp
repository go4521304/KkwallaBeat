// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleManager.h"
#include "Engine/StaticMeshActor.h"

#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "MovieSceneSequencePlaybackSettings.h"

// Sets default values
ATitleManager::ATitleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATitleManager::BeginPlay()
{
	Super::BeginPlay();

	ALevelSequenceActor* SequenceActor;
	FMovieSceneSequencePlaybackSettings SequenceSetting;
	//SequenceSetting.LoopCount.Value = -1;
	SequenceSetting.bPauseAtEnd = true;
	ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), TitleSequence, SequenceSetting, SequenceActor);
	if (IsValid(SequencePlayer))
	{
		SequencePlayer->Play();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SequencePlayer is not valid"));
	}
}

// Called every frame
void ATitleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATitleManager::TouchInput(const FVector2D& InPos)
{
	FVector BtnLoc, BtnExt;
	Btn_Start->GetComponentsBoundingBox(false, false).GetCenterAndExtents(BtnLoc, BtnExt);
	
	//if (BtnLoc.X - InPos.X)
}

void ATitleManager::Restart()
{

}

