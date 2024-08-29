// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteManager.h"
#include "BGActor.H"
#include "Kkwalla.h"
#include "FMODBlueprintStatics.h"

const int32 MS_TIME = 10000000;

// Sets default values
ANoteManager::ANoteManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BPM = 60;
	BPMTimeMs = 0;
	BeatCount = 0;
	CurTimeSec = 0;
	bAnyKeyDown =false;
	OnKeyDownTime = 0;
	CachePos = {};

	GradeCheck = 45;
	BGMBias = 100;

	PreBeatCount = 4;

	bProcessed = false;

	AudioComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANoteManager::BeginPlay()
{
	Super::BeginPlay();

	AudioComponent->SetEvent(BGMusic);
	BPMTimeMs = MS_TIME / ((float)BPM / 60);
	GradeCheck = GradeCheck * (MS_TIME / 1000);
	BGMBias = BGMBias * (MS_TIME / 1000);
	BeatCount = 0;
	GameState = ManagerState::PreBeat;
	bProcessed = false;
}

// Called every frame
void ANoteManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	int32 DeltaTimeSec = static_cast<int32>(DeltaTime * MS_TIME);
	CurTimeSec += DeltaTimeSec;

	// 배경 업데이트
	if (IsValid(BgActor))
	{
		int32 BeatSec = CurTimeSec % BPMTimeMs;
		float BGPhase = BeatSec / ((float)(BPMTimeMs));
		BgActor->SetPhase(BGPhase);
	}

	if (GameState == ManagerState::PreBeat)
	{
		if (bProcessed == false && BGMBias <= 0)
		{
			AudioComponent->SetParameter(TEXT("Turn"), 1.0f);
			AudioComponent->Play();
			bProcessed = true;
		}
		else
		{
			BGMBias -= DeltaTimeSec;
		}

		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			PreBeatCount--;
			BeatCount++;

			if (PreBeatCount < 0)
			{
				GameState = ManagerState::Start;
				AudioComponent->SetParameter(TEXT("Turn"), 0.0f);

				bAnyKeyDown = false;
				bProcessed = false;
				OnKeyDownTime = -1;
				CachePos = {};
				BeatCount = 0;
			}
		}
	}

	else if (GameState == ManagerState::Start)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			bAnyKeyDown = false;
			bProcessed = false;
			OnKeyDownTime = -1;
			CachePos = {};
			BeatCount++;

			if (BeatCount > 8)
			{
				BeatCount %= 8;
				CurTimeSec = 0;

				for (AKkwalla* CharIter : Kkwallas)
				{
					if (IsValid(CharIter))
					{
						CharIter->Reset();
					}
				}
			}
			else
			{
				for (AKkwalla* CharIter : Kkwallas)
				{
					if (IsValid(CharIter))
					{
						CharIter->Update();
					}
				}
			}
		}

		if (bAnyKeyDown && (bProcessed == false))
		{
			for (AKkwalla* CharIter : Kkwallas)
			{
				if (IsValid(CharIter) && CharIter->PointCheck(CachePos))
				{
					if (((BeatCount - 1) * BPMTimeMs) - GradeCheck <= OnKeyDownTime && OnKeyDownTime <= ((BeatCount - 1) * BPMTimeMs) + GradeCheck)
					{
						CharIter->BeerFill();
					}
					else
					{
						CharIter->BeerFail();
					}
					bAnyKeyDown = false;
					bProcessed = true;
					OnKeyDownTime = -1;
					CachePos = {};
					break;
				}
			}
		}
	}
}

void ANoteManager::TouchInput(const FVector2D& InPos)
{
	if (bAnyKeyDown || bProcessed)
	{
		return;
	}
	bAnyKeyDown = true;
	OnKeyDownTime = CurTimeSec;
	CachePos = InPos;
}

