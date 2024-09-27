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

	IsBGMPlay = false;

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
	IsBGMPlay = false;

	// 시작할 때 입력을 안받겠음
	bAnyKeyDown = true;
	OnKeyDownTime = -1;
	CachePos = FVector2D::ZeroVector;

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
		if (IsBGMPlay == false && BGMBias <= 0)
		{
			AudioComponent->SetParameter(TEXT("Turn"), 1.0f);
			AudioComponent->Play();
			IsBGMPlay = true;
		}
		else
		{
			BGMBias -= DeltaTimeSec;
		}

		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			BeatCount++;
			if (PreBeatCount < BeatCount)
			{
				bAnyKeyDown = false;
				OnKeyDownTime = -1;
				BeatCount = 0;
				CurTimeSec = 0;

				for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
				{
					PatternArr[Iter] = -1;
					PatternArrCheck[Iter] = -1;
				}

				UE_LOG(LogTemp, Error, TEXT("MakeTurn"));
				GameState = ManagerState::MakeTurn;
				HudWidget->ChangeBreakWidgetVisibility(false);
				HudWidget->ShowFailePage(false);
				AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
				return;
			}
		}
	}

	else if (GameState == ManagerState::MakeTurn)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("BeatCount %d"), CurTimeSec);
			BeatCount++;

			if (BeatCount > 8)
			{
				BeatCount = 0;
				CurTimeSec = 0;
				PlayerNum = 5; // 이걸 플레이할 사람 수

				bAnyKeyDown = true;
				OnKeyDownTime = -1;

				for (AKkwalla* CharIter : Kkwallas)
				{
					if (IsValid(CharIter))
					{
						CharIter->Reset();
					}
				}

				UE_LOG(LogTemp, Error, TEXT("MakeTurnBreak"));
				GameState = ManagerState::MakeTurnBreak;
				AudioComponent->SetParameter(TEXT("Turn"), 1.0f);
				HudWidget->ChangeBreakWidgetVisibility(true);
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

		if (bAnyKeyDown)
		{
			int32 CheckBeatCount = CurTimeSec / BPMTimeMs;

			// 빨리 눌렀을 경우 TestBeatCount가 현재보다 느림
			if ((CheckBeatCount * BPMTimeMs) + GradeCheck < OnKeyDownTime)
			{
				CheckBeatCount++;
				UE_LOG(LogTemp, Warning, TEXT("?"));

			}
			// 늦게 눌렀을 경우 CheckBeatCount가 현재와 맞음

			// 입력할 수 있는 비트일 경우
			if (CheckBeatCount < 6)
			{
				UE_LOG(LogTemp, Warning, TEXT("%d %d"), CheckBeatCount, OnKeyDownTime);
				if ((CheckBeatCount * BPMTimeMs) - GradeCheck <= OnKeyDownTime && OnKeyDownTime <= (CheckBeatCount * BPMTimeMs) + GradeCheck)
				{
					for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
					{
						if (IsValid(Kkwallas[Iter]) && Kkwallas[Iter]->PointCheck(CachePos))
						{
							Kkwallas[Iter]->BeerReady();
							PatternArr[CheckBeatCount] = Iter;
							break;
						}
					}
				}
			}

			bAnyKeyDown = false;
			OnKeyDownTime = -1;
		}
	}

	else if (GameState == ManagerState::MakeTurnBreak)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			BeatCount++;
			if (PreBeatCount < BeatCount)
			{
				BeatCount = 0;
				CurTimeSec = 0;

				bAnyKeyDown = false;
				OnKeyDownTime = -1;

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
			UE_LOG(LogTemp, Error, TEXT("BeatCount %d"), CurTimeSec);
			BeatCount++;

			if (BeatCount > 8)
			{
				BeatCount = 0;
				CurTimeSec = 0;
				PlayerNum--;

				bAnyKeyDown = true;
				OnKeyDownTime = -1;

				bool bSuccess = true;
				for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
				{
					if (PatternArr[Iter] != PatternArrCheck[Iter])
					{
						bSuccess = false;
						break;
					}
				}

				if (bSuccess)
				{
					for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
					{
						Kkwallas[Iter]->Reset();
						PatternArrCheck[Iter] = -1;
					}

					UE_LOG(LogTemp, Error, TEXT("PlayTurnBreak"));
					GameState = ManagerState::PlayTurnBreak;
					AudioComponent->SetParameter(TEXT("Turn"), 1.0f);
					HudWidget->ChangeBreakWidgetVisibility(true);
				}
				else
				{
					for (AKkwalla* CharIter : Kkwallas)
					{
						if (IsValid(CharIter))
						{
							CharIter->Reset();
						}
					}

					UE_LOG(LogTemp, Error, TEXT("Fail"));
					GameState = ManagerState::Fail;
					HudWidget->ShowFailePage(true);
				}
				return;
			}
			else if (BeatCount == 8)
			{
				for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
				{
					if (PatternArr[Iter] != -1 && PatternArr[Iter] == PatternArrCheck[Iter])
					{
						Kkwallas[PatternArr[Iter]]->BeerSuccess();
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

		if (bAnyKeyDown)
		{
			int32 CheckBeatCount = CurTimeSec / BPMTimeMs;

			// 빨리 눌렀을 경우 TestBeatCount가 현재보다 느림
			if ((CheckBeatCount * BPMTimeMs) + GradeCheck < OnKeyDownTime)
			{
				CheckBeatCount++;
				UE_LOG(LogTemp, Warning, TEXT("?"));

			}
			// 늦게 눌렀을 경우 CheckBeatCount가 현재와 맞음

			// 입력할 수 있는 비트일 경우
			if (CheckBeatCount < 6)
			{
				UE_LOG(LogTemp, Warning, TEXT("%d %d"), CheckBeatCount, OnKeyDownTime);
				if ((CheckBeatCount * BPMTimeMs) - GradeCheck <= OnKeyDownTime && OnKeyDownTime <= (CheckBeatCount * BPMTimeMs) + GradeCheck)
				{
					for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
					{
						if (IsValid(Kkwallas[Iter]) && Kkwallas[Iter]->PointCheck(CachePos))
						{
							Kkwallas[Iter]->BeerReady();
							PatternArrCheck[CheckBeatCount] = Iter;
							break;
						}
					}
				}
			}

			bAnyKeyDown = false;
			OnKeyDownTime = -1;
		}
	}

	else if (GameState == ManagerState::PlayTurnBreak)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			BeatCount++;
			if (PreBeatCount < BeatCount)
			{
				BeatCount = 0;
				CurTimeSec = 0;

				OnKeyDownTime = -1;

				if (PlayerNum <= 0)
				{
					bAnyKeyDown = true;

					UE_LOG(LogTemp, Error, TEXT("PreBeat"));
					GameState = ManagerState::PreBeat;
					HudWidget->ShowFailePage(true);
					AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
				}
				else
				{
					bAnyKeyDown = false;

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
	if (bAnyKeyDown)
	{
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("%d"), CurTimeSec);
	bAnyKeyDown = true;
	OnKeyDownTime = CurTimeSec;
	CachePos = InPos;
}

void ANoteManager::Restart()
{

}

