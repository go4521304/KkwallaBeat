// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteManagerV2.h"
#include "BGActor.H"
#include "Kkwalla.h"
#include "FMODBlueprintStatics.h"
#include "Kismet/GameplayStatics.h"
#include "KkwallaHUD.h"

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

	PlayerNum = 5;
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

	// ������ �� �Է��� �ȹް���
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

	// �뷡 �ٷ� ����
	AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
	AudioComponent->Play();

	if (AudioComponent->StudioInstance)
	{
		FMOD::ChannelGroup* Channel;
		AudioComponent->StudioInstance->getChannelGroup(&Channel);
		if (Channel)
		{
			Channel->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &HeadDSP);
			if (HeadDSP)
			{

				HeadDSP->setMeteringEnabled(false, true);
			}
		}
	}
	else
	{
		ensureAlwaysMsgf(false, TEXT("No Sound Instance!"));
	}
}

// Called every frame
void ANoteManagerV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	int32 DeltaTimeSec = static_cast<int32>(DeltaTime * MS_TIME);
	CurTimeSec += DeltaTimeSec;

	if (AudioComponent->StudioInstance)
	{
		FMOD::ChannelGroup* Channel;
		AudioComponent->StudioInstance->getChannelGroup(&Channel);
		if (Channel)
		{
			Channel->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &HeadDSP);
			if (HeadDSP)
			{

				HeadDSP->setMeteringEnabled(false, true);
			}
		}
	}

	// ��Ʈ üũ
	FMOD_DSP_METERING_INFO Info = {};
	if (HeadDSP)
	{
		HeadDSP->getMeteringInfo(nullptr, &Info);

		// ��� ������Ʈ
		for (ABGActor* BgActor : BgActors)
		{
			if (IsValid(BgActor))
			{
				BgActor->SetPhase(Info.rmslevel[0]);
			}
		}
	}

	if (GameState == ManagerStateV2::PreBeat)
	{
		UE_LOG(LogTemp, Error, TEXT("Metering: %d channels, %d len.  rms: %.3f  %.3f"), Info.numchannels, Info.numsamples, Info.rmslevel[0], Info.rmslevel[1]);
		if (PreBeatCount < BeatCount)
		{
			BPMTimeMs /= (BeatCount - 1);

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
			static int32 LastBeatCheckTime = 0;
			static bool BeatFlipFlop = false;
			if (BeatFlipFlop == false && Info.rmslevel[0] >= 0.3f)
			{
				BPMTimeMs += CurTimeSec - LastBeatCheckTime;
				LastBeatCheckTime = CurTimeSec;

				BeatCount++;
				BeatFlipFlop = true;
			}
			else if (BeatFlipFlop && Info.rmslevel[0] < 0.1f)
			{
				BeatFlipFlop = true;
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
				PlayerNum = 5; // �̰� �÷����� ��� ��

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

			// ���� ������ ��� TestBeatCount�� ���纸�� ����
			if ((CheckBeatCount * BPMTimeMs) + GradeCheck < OnKeyDownTime)
			{
				CheckBeatCount++;
				UE_LOG(LogTemp, Warning, TEXT("?"));

			}
			// �ʰ� ������ ��� CheckBeatCount�� ����� ����

			// �Է��� �� �ִ� ��Ʈ�� ���
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
					HudWidget->ChangeBreakWidgetVisibility(false);
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
					GameState = ManagerStateV2::PlayTurnBreak;
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

			// ���� ������ ��� TestBeatCount�� ���纸�� ����
			if ((CheckBeatCount * BPMTimeMs) + GradeCheck < OnKeyDownTime)
			{
				CheckBeatCount++;
				UE_LOG(LogTemp, Warning, TEXT("?"));

			}
			// �ʰ� ������ ��� CheckBeatCount�� ����� ����

			// �Է��� �� �ִ� ��Ʈ�� ���
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

				if (PlayerNum <= 0)
				{
					bAnyKeyDown = true;

					UE_LOG(LogTemp, Error, TEXT("PreBeat"));
					GameState = ManagerStateV2::PreBeat;
					HudWidget->ShowFailePage(true);
					AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
				}
				else
				{
					bAnyKeyDown = false;

					UE_LOG(LogTemp, Error, TEXT("PlayTurn"));
					GameState = ManagerStateV2::PlayTurn;
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

	else if (GameState == ManagerStateV2::Fail)
	{

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
