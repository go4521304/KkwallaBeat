// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteManagerV2.h"
#include "BGActorV2.H"
#include "Kkwalla.h"
#include "FMODBlueprintStatics.h"
#include "Kismet/GameplayStatics.h"
#include "KkwallaHUD.h"
#include "ColorDataAsset.h"

const int32 MS_TIME = 1000;

// Sets default values
ANoteManagerV2::ANoteManagerV2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BPMTimeMs = 0;
	BeatCount = 0;
	CurTimeSec = 0;
	bAnyKeyDown =false;
	OnKeyDownTime = 0;
	CachePos = FVector2D::ZeroVector;

	GradeCheck = 45;

	PreBeatCount = 4;

	AudioComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

	PlayerNum = 0;
	CurTurnIndex = 0;
}

// Called when the game starts or when spawned
void ANoteManagerV2::BeginPlay()
{
	Super::BeginPlay();

	AudioComponent->SetEvent(BGMusic);
	BPMTimeMs = 0;
	GradeCheck = GradeCheck * (MS_TIME / 1000);
	BeatCount = 0;
	GameState = ManagerStateV2::PreBeat;

	LastBeatCheckTime = 0;
	BeatFlipFlop = false;

	// 시작할 때 입력을 안받겠음
	bAnyKeyDown = true;
	OnKeyDownTime = -1;
	CachePos = FVector2D::ZeroVector;

	// 플레이어 세팅
	PlayerNum = 5; // (추후 initset 함수로 분리하고 플레이어 수를 입력받도록)
	CurTurnIndex = 0;

	PlayerTurn.Reserve(PlayerNum);
	for (int32 i = 0; i < PlayerNum; ++i)
	{
		PlayerTurn.Add(i);
	}
	for (int32 i = 0; i < PlayerNum; ++i)
	{
		int32 SwapIndex = FMath::RandRange(0, PlayerNum - 1);
		if (i != SwapIndex)
		{
			PlayerTurn.Swap(i, SwapIndex);
		}
	}

	APlayerController* PlayerCon = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerCon))
	{
		HudWidget = PlayerCon->GetHUD<AKkwallaHUD>();
		if (IsValid(HudWidget))
		{
			int32 CurrentPlayer = PlayerTurn[CurTurnIndex];
			HudWidget->InitSet();
			HudWidget->ChangeBreakWidgetVisibility(true, ColorDataAsset->ColorData[CurrentPlayer].ColorA, ColorDataAsset->ColorData[CurrentPlayer].ColorB);
		}
	}

	// 배경 세팅
	if (IsValid(ColorDataAsset))
	{
		for (int32 BGIndex = 0; BGIndex < BgActors.Num(); ++BGIndex)
		{
			if (IsValid(BgActors[BGIndex]))
			{
				BgActors[BGIndex]->InitSet();
				if (ColorDataAsset->ColorData.IsValidIndex(BGIndex))
				{
					BgActors[BGIndex]->SetBGColor(ColorDataAsset->ColorData[BGIndex].ColorA, ColorDataAsset->ColorData[BGIndex].ColorB);
				}
			}
		}
	}
	
	PatternArr.Reserve(8);
	PatternArrCheck.Reserve(8);
	for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
	{
		PatternArr.Add(-1);
		PatternArrCheck.Add(-1);
	}

	// 노래 바로 시작
	AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
	AudioComponent->Play();
}

// Called every frame
void ANoteManagerV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 비트 체크
	FMOD_DSP_METERING_INFO Info = {};
	if (AudioComponent->StudioInstance)
	{
		FMOD::ChannelGroup* Channel;
		AudioComponent->StudioInstance->getChannelGroup(&Channel);
		if (Channel)
		{
			FMOD::DSP* HeadDSP;
			Channel->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &HeadDSP);
			if (HeadDSP)
			{
				HeadDSP->setMeteringEnabled(false, true);
				HeadDSP->getMeteringInfo(nullptr, &Info);

				// 배경 업데이트
				for (ABGActorV2* BgActor : BgActors)
				{
					if (IsValid(BgActor))
					{
						BgActor->SetPhase(Info.rmslevel[0]);
					}
				}
			}
		}
	}
	else
	{
		ensureAlwaysMsgf(false, TEXT("No Sound Instance!"));
		return;
	}

	int32 DeltaTimeSec = static_cast<int32>(DeltaTime * MS_TIME);
	CurTimeSec += DeltaTimeSec;

	if (GameState == ManagerStateV2::PreBeat)
	{
		UE_LOG(LogTemp, Error, TEXT("Metering: %d channels, %d len.  rms: %.3f  %.3f"), Info.numchannels, Info.numsamples, Info.rmslevel[0], Info.rmslevel[1]);
		if (PreBeatCount <= BeatCount)
		{
			BPMTimeMs /= BeatCount;
			BPMTimeMs += 30; // 측정값에 수동으로 30ms 추가해줌 (조금 빠르게 측정이 되고있음)

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
			GameState = ManagerStateV2::MakeTurn;
			HudWidget->ChangeBreakWidgetVisibility(false);
			HudWidget->ShowFailePage(false);
			AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
		}
		else
		{
			if (BeatFlipFlop == false && Info.rmslevel[0] >= 0.02f)
			{
				BPMTimeMs += CurTimeSec - LastBeatCheckTime;
				LastBeatCheckTime = CurTimeSec;

				BeatCount++;
				BeatFlipFlop = true;

				UE_LOG(LogTemp, Error, TEXT("MakeTurn"));
			}
			else if (BeatFlipFlop && Info.rmslevel[0] < 0.01f)
			{
				BeatFlipFlop = false;
			}
		}
	}

	else if (GameState == ManagerStateV2::MakeTurn)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("BeatCount %d"), CurTimeSec);
			BeatCount++;

			if (BeatCount > 8)
			{
				BeatCount = 0;
				CurTimeSec = 0;
				CurTurnIndex++;
				//PlayerNum = 5; // 이걸 플레이할 사람 수

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
				GameState = ManagerStateV2::MakeTurnBreak;
				AudioComponent->SetParameter(TEXT("Turn"), 1.0f);
				int32 CurrentPlayer = PlayerTurn[CurTurnIndex];
				HudWidget->ChangeBreakWidgetVisibility(true, ColorDataAsset->ColorData[CurrentPlayer].ColorA, ColorDataAsset->ColorData[CurrentPlayer].ColorB);
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

	else if (GameState == ManagerStateV2::MakeTurnBreak)
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

				HudWidget->ChangeBreakWidgetVisibility(false);

				UE_LOG(LogTemp, Error, TEXT("PlayTurn"));
				GameState = ManagerStateV2::PlayTurn;
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
					//HudWidget->ChangeBreakWidgetVisibility(false);
				}
				break;

				default:
					break;
				}
			}
		}
	}

	else if (GameState == ManagerStateV2::PlayTurn)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("BeatCount %d"), CurTimeSec);
			BeatCount++;

			if (BeatCount > 8)
			{
				BeatCount = 0;
				CurTimeSec = 0;
				CurTurnIndex++;
				/*PlayerNum--;*/

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
					if (PlayerNum <= CurTurnIndex)
					{
						bAnyKeyDown = true;

						UE_LOG(LogTemp, Error, TEXT("Success"));
						GameState = ManagerStateV2::Success;
						HudWidget->ShowFailePage(true); // ShowSuceessPage 로 수정 필요
						AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
						return;
					}

					for (int32 Iter = 0; Iter < Kkwallas.Num(); ++Iter)
					{
						Kkwallas[Iter]->Reset();
						PatternArrCheck[Iter] = -1;
					}

					UE_LOG(LogTemp, Error, TEXT("PlayTurnBreak"));
					GameState = ManagerStateV2::PlayTurnBreak;
					AudioComponent->SetParameter(TEXT("Turn"), 1.0f);
					int32 CurrentPlayer = PlayerTurn[CurTurnIndex];
					HudWidget->ChangeBreakWidgetVisibility(true, ColorDataAsset->ColorData[CurrentPlayer].ColorA, ColorDataAsset->ColorData[CurrentPlayer].ColorB);
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
					GameState = ManagerStateV2::Fail;
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

	else if (GameState == ManagerStateV2::PlayTurnBreak)
	{
		if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			BeatCount++;
			if (PreBeatCount < BeatCount)
			{
				BeatCount = 0;
				CurTimeSec = 0;

				OnKeyDownTime = -1;
				HudWidget->ChangeBreakWidgetVisibility(false);

				bAnyKeyDown = false;

				UE_LOG(LogTemp, Error, TEXT("PlayTurn"));
				GameState = ManagerStateV2::PlayTurn;
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
					//HudWidget->ChangeBreakWidgetVisibility(false);
				}
				break;

				default:
					break;
				}
			}
		}
	}

	else if (GameState == ManagerStateV2::Success)
	{
		// 다시 초기 세팅 후 MakeTurn 으로 넘어가기
	}

	else if (GameState == ManagerStateV2::Fail)
	{
		// 풉 누구마셔 전달
	}
}

void ANoteManagerV2::TouchInput(const FVector2D& InPos)
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

