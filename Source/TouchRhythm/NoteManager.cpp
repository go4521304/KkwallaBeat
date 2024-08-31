// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteManager.h"
#include "BGActor.H"
#include "Kkwalla.h"
#include "FMODBlueprintStatics.h"
#include "Kismet/GameplayStatics.h"
#include "KkwallaHUD.h"

const int32 MS_TIME = 1000;

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
	CachePos = FVector2D::ZeroVector;

	GradeCheck = 45;
	BGMBias = 100;

	PreBeatCount = 4;

	bProcessed = false;

	AudioComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

	PlayerNum = 5;
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

	APlayerController* PlayerCon = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerCon))
	{
		HudWidget = PlayerCon->GetHUD<AKkwallaHUD>();
		if (IsValid(HudWidget))
		{
			HudWidget->ChangeBreakWidgetVisibility(false);
		}
	}
	
	PatternArr.Reserve(8);
	PatternArrCheck.Reserve(8);
	for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
	{
		PatternArr.Add(-1);
		PatternArrCheck.Add(-1);
	}
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
				UE_LOG(LogTemp, Error, TEXT("MakeTurn"));
				GameState = ManagerState::MakeTurn;
				HudWidget->ChangeBreakWidgetVisibility(false);
				HudWidget->ShowFailePage(false);
				AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
				bAnyKeyDown = false;
				bProcessed = false;
				OnKeyDownTime = -1;
				CachePos = FVector2D::ZeroVector;
				BeatCount = 0;
				CurTimeSec = 0;

				for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
				{
					PatternArr[Iter] = -1;
					PatternArrCheck[Iter] = -1;
				}
				return;
			}
		}
	}

	else if (GameState == ManagerState::MakeTurn)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("BeatCount %d"), CurTimeSec);

			bAnyKeyDown = false;
			bProcessed = false;
			OnKeyDownTime = -1;
			CachePos = FVector2D::ZeroVector;
			BeatCount++;

			if (BeatCount > 8)
			{
				UE_LOG(LogTemp, Error, TEXT("MakeTurnBreak"));
				GameState = ManagerState::MakeTurnBreak;
				HudWidget->ChangeBreakWidgetVisibility(true);
				BeatCount = 0;
				CurTimeSec = 0;
				PlayerNum = 5; // 이걸 플레이할 사람 수
				PreBeatCount = 4;
				AudioComponent->SetParameter(TEXT("Turn"), 1.0f);

				for (AKkwalla* CharIter : Kkwallas)
				{
					if (IsValid(CharIter))
					{
						CharIter->Reset();
					}
				}
				return;
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

		// 여기서의 BeatCount는 실제 입력을 받을 비트를 의미
		if (bAnyKeyDown && (bProcessed == false) && BeatCount < 7)
		{
			for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
			{
				if (IsValid(Kkwallas[Iter]) && Kkwallas[Iter]->PointCheck(CachePos))
				{
					if (((BeatCount - 1) * BPMTimeMs) - GradeCheck <= OnKeyDownTime && OnKeyDownTime <= ((BeatCount - 1) * BPMTimeMs) + GradeCheck)
					{
						Kkwallas[Iter]->BeerReady();
						PatternArr[(BeatCount - 1)] = Iter;
					}
					bAnyKeyDown = false;
					bProcessed = true;
					OnKeyDownTime = -1;
					CachePos = FVector2D::ZeroVector;
					break;
				}
			}
		}
	}

	else if (GameState == ManagerState::MakeTurnBreak)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			bAnyKeyDown = false;
			bProcessed = false;
			OnKeyDownTime = -1;
			CachePos = FVector2D::ZeroVector;
			BeatCount++;
			PreBeatCount--;

			if (PreBeatCount < 0)
			{
				BeatCount = 0;
				CurTimeSec = 0;
				PreBeatCount = 4;
				UE_LOG(LogTemp, Error, TEXT("PlayTurn"));
				GameState = ManagerState::PlayTurn;
				AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
				return;
			}
			else
			{
				switch (BeatCount)
				{
				case 1:
				{
					HudWidget->PlayAnimCount(3);
				}
				break;

				case 2:
				{
					HudWidget->PlayAnimCount(2);
				}
				break;

				case 3:
				{
					HudWidget->PlayAnimCount(1);
				}
				break;

				case 4:
				{
					HudWidget->ChangeBreakWidgetVisibility(false);
				}
				break;

				default:
					break;
				}
			}
		}
	}

	else if (GameState == ManagerState::PlayTurn)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			bAnyKeyDown = false;
			bProcessed = false;
			OnKeyDownTime = -1;
			CachePos = FVector2D::ZeroVector;
			BeatCount++;

			if (BeatCount > 8)
			{
				BeatCount = 0;
				CurTimeSec = 0;
				PreBeatCount = 4;
				PlayerNum--;
				bool Success = true;
				for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
				{
					if (PatternArr[Iter] != PatternArrCheck[Iter])
					{
						Success = false;
						break;
					}
				}
				if (Success)
				{
					GameState = ManagerState::PlayTurnBreak;
					HudWidget->ChangeBreakWidgetVisibility(true);
					AudioComponent->SetParameter(TEXT("Turn"), 1.0f);
					UE_LOG(LogTemp, Error, TEXT("PlayTurnBreak"));
					for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
					{
						Kkwallas[Iter]->Reset();
						PatternArrCheck[Iter] = -1;
					}
				}
				else
				{
					GameState = ManagerState::Fail;
					HudWidget->ChangeBreakWidgetVisibility(false);
					HudWidget->ShowFailePage(true);
					UE_LOG(LogTemp, Error, TEXT("Fail"));
					for (AKkwalla* CharIter : Kkwallas)
					{
						if (IsValid(CharIter))
						{
							CharIter->Reset();
						}
					}
				}
				return;
			}
			else if (BeatCount == 8)
			{
				for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
				{
					if (PatternArr[Iter] == PatternArrCheck[Iter])
					{
						if (PatternArr[Iter] != -1)
						{
							Kkwallas[PatternArr[Iter]]->BeerSuccess();
						}
					}
					else
					{
						if (PatternArr[Iter] != -1)
						{
							Kkwallas[PatternArr[Iter]]->BeerFail();
						}
						if (PatternArrCheck[Iter] != -1)
						{
							Kkwallas[PatternArrCheck[Iter]]->BeerFail();
						}
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

		// 여기서의 BeatCount는 실제 입력을 받을 비트를 의미
		if (bAnyKeyDown && (bProcessed == false) && BeatCount < 7)
		{
			for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
			{
				if (IsValid(Kkwallas[Iter]) && Kkwallas[Iter]->PointCheck(CachePos))
				{
					if (((BeatCount - 1) * BPMTimeMs) - GradeCheck <= OnKeyDownTime && OnKeyDownTime <= ((BeatCount - 1) * BPMTimeMs) + GradeCheck)
					{
						Kkwallas[Iter]->BeerReady();
						PatternArrCheck[BeatCount-1] = Iter;
					}
					bAnyKeyDown = false;
					bProcessed = true;
					OnKeyDownTime = -1;
					CachePos = FVector2D::ZeroVector;
					break;
				}
			}
		}
	}

	else if (GameState == ManagerState::PlayTurnBreak)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			bAnyKeyDown = false;
			bProcessed = false;
			OnKeyDownTime = -1;
			CachePos = FVector2D::ZeroVector;
			BeatCount++;
			PreBeatCount--;

			if (PreBeatCount < 0)
			{
				BeatCount = 0;
				CurTimeSec = 0;
				PreBeatCount = 4;
				if (PlayerNum <= 0)
				{
					UE_LOG(LogTemp, Error, TEXT("PreBeat"));
					GameState = ManagerState::PreBeat;
					HudWidget->ChangeBreakWidgetVisibility(false);
					HudWidget->ShowFailePage(true);
					AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("PlayTurn"));
					GameState = ManagerState::PlayTurn;
					AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
				}
				return;
			}
			else
			{
				switch (BeatCount)
				{
				case 1:
				{
					HudWidget->PlayAnimCount(3);
				}
				break;

				case 2:
				{
					HudWidget->PlayAnimCount(2);
				}
				break;

				case 3:
				{
					HudWidget->PlayAnimCount(1);
				}
				break;

				case 4:
				{
					HudWidget->ChangeBreakWidgetVisibility(false);
				}
				break;

				default:
					break;
				}
			}
		}
	}
	else if (GameState == ManagerState::Fail)
	{

	}
}

void ANoteManager::TouchInput(const FVector2D& InPos)
{
	if (bAnyKeyDown || bProcessed)
	{
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("%d"), CurTimeSec);
	bAnyKeyDown = true;
	OnKeyDownTime = CurTimeSec;
	CachePos = InPos;
}

