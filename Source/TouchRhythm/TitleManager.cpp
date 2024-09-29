// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleManager.h"
#include "Engine/StaticMeshActor.h"

#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Kismet/GameplayStatics.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "TitleHUD.h"

// Sets default values
ATitleManager::ATitleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

	bPlayingCredit = false;
}

// Called when the game starts or when spawned
void ATitleManager::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerCon = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerCon))
	{
		HudWidget = PlayerCon->GetHUD<ATitleHUD>();
		if (IsValid(HudWidget))
		{
			HudWidget->InitSet();
		}
	}

	Restart();
}

// Called every frame
void ATitleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATitleManager::TouchInput(const FVector& InPos)
{
	if (bPlayingCredit)
	{
		return;
	}

	// YZ축으로 구성된 월드임
	FVector BtnLoc, BtnExt;
	Btn_Start->GetComponentsBoundingBox(false, false).GetCenterAndExtents(BtnLoc, BtnExt);
	if (BtnLoc.Y - BtnExt.Y < InPos.Y && InPos.Y < BtnLoc.Y + BtnExt.Y &&
		BtnLoc.Z - BtnExt.Z < InPos.Z && InPos.Z < BtnLoc.Z + BtnExt.Z)
	{
		// 시작 버튼 눌림
		UE_LOG(LogTemp, Warning, TEXT("Start Button Pressed"));
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("TouchRhythmWorldV2")));
		UFMODBlueprintStatics::PlayEvent2D(GetWorld(), TransitionGame, true);
		AudioComponent->Stop();
		return;
	}

	Btn_Credit->GetComponentsBoundingBox(false, false).GetCenterAndExtents(BtnLoc, BtnExt);
	if (BtnLoc.Y - BtnExt.Y < InPos.Y && InPos.Y < BtnLoc.Y + BtnExt.Y &&
		BtnLoc.Z - BtnExt.Z < InPos.Z && InPos.Z < BtnLoc.Z + BtnExt.Z)
	{
		// 크레딧 버튼 눌림
		bPlayingCredit = true;
		UE_LOG(LogTemp, Warning, TEXT("Credit Button Pressed"));
		AudioComponent->Stop();
		HudWidget->ShowCreditPage(true);
		return;
	}
}

void ATitleManager::Restart()
{
	HudWidget->ShowCreditPage(false);
	bPlayingCredit = false;

	ALevelSequenceActor* SequenceActor;
	FMovieSceneSequencePlaybackSettings SequenceSetting;
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

	AudioComponent->SetEvent(BGMusic);
	AudioComponent->Play();
}

