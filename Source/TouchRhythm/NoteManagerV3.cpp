// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteManagerV3.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "BGActorV2.h"
#include "Kismet/GameplayStatics.h"
#include "KkwallaHUD.h"
#include "ColorDataAsset.h"
#include "Kkwalla.h"

ANoteManagerV3::ANoteManagerV3()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AudioComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
}

void ANoteManagerV3::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(BGMusic))
	{
		AudioComponent->SetEvent(BGMusic);
	}
	InitManager();
}

void ANoteManagerV3::Tick(float DeltaTime)
{
	if (GameState == ManagerStateV3::None)
	{
		return;
	}

	Super::Tick(DeltaTime);

	// ��Ʈ üũ
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

				// ��� ������Ʈ
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

	// CurTimeSec ���
	CurTimeSec += FMath::FloorToInt32(DeltaTime * MS_TIME);

	if (GameState == ManagerStateV3::PlayerSet)
	{
		//if (bGameStart)
		{
			// �÷��̾� ����
			for (int32 i = 0; i < PlayerNum; ++i)
			{
				int32 SwapIndex = FMath::RandRange(0, PlayerNum - 1);
				if (i != SwapIndex)
				{
					PlayerTurnArr.Swap(i, SwapIndex);
				}
			}

			int32 CurrentPlayer = PlayerTurnArr[CurTurnIndex];
			HudWidget->ChangeBreakWidgetVisibility(true, ColorDataAsset->ColorData[CurrentPlayer].ColorA, ColorDataAsset->ColorData[CurrentPlayer].ColorB);

			AudioComponent->SetParameter(TEXT("Turn"), 1.0f);
			AudioComponent->Play();

			GameState = ManagerStateV3::StandBy;
		}
	}
	if (GameState == ManagerStateV3::StandBy)
	{
		if (AudioComponent->StudioInstance)
		{
			FMOD::ChannelGroup* Channel;
			AudioComponent->StudioInstance->getChannelGroup(&Channel);
			if (Channel)
			{
				CurTimeSec = 0;
				GameState = ManagerStateV3::PreBeat;
			}
		}
	}
	if (GameState == ManagerStateV3::PreBeat)
	{
		if (4 * BPMTimeMs <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("@@@@@@"));
			BeatCount = 0;
			CurTimeSec -= 4 * BPMTimeMs;
			bAnyKeyDown = false;
			OnKeyDownTime = -1;
			AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
			HudWidget->ChangeBreakWidgetVisibility(false);
			GameState = ManagerStateV3::MakeTurn;
			return;
		}
		else if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("@@@"));
			switch (BeatCount)
			{
			case 0:
				HudWidget->PlayAnimCount(3);
			break;
			case 1:
				HudWidget->PlayAnimCount(2);
			break;
			case 2:
				HudWidget->PlayAnimCount(1);
			break;
			case 3:
				HudWidget->PlayAnimCount(4);
			break;
			default:
				break;
			}

			BeatCount++;
		}
	}
	else if (GameState == ManagerStateV3::MakeTurn)
	{
		// ���� ���� ������ ��
		if (8 * BPMTimeMs <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("TTT"));
			for (AKkwalla* Kkwalla : Kkwallas)
			{
				Kkwalla->Reset();
			}
			BeatCount = 0;
			CurTimeSec -= 8 * BPMTimeMs;
			// ���� ���� �Է��� ����
			bAnyKeyDown = true;
			OnKeyDownTime = -1;
			// ���� �÷��̾��
			CurTurnIndex++;
			int32 CurrentPlayer = PlayerTurnArr[CurTurnIndex];
			AudioComponent->SetParameter(TEXT("Turn"), 1.0f);
			HudWidget->ChangeBreakWidgetVisibility(true, ColorDataAsset->ColorData[CurrentPlayer].ColorA, ColorDataAsset->ColorData[CurrentPlayer].ColorB);
			GameState = ManagerStateV3::TurnBreak;
			return;
		}
		else if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("!!!"));

			for (AKkwalla* kkwalla : Kkwallas)
			{
				kkwalla->Update();
			}

			if (BeatCount == 6)
			{
				for (int32 i = 0; i < Kkwallas.Num(); ++i)
				{
					if (NoteChecker.IsValidIndex(i) && NoteChecker[i] != -1 && IsValid(Kkwallas[i]))
					{
						Kkwallas[i]->BeerFill();
					}
				}
			}
			else if (BeatCount == 7)
			{
				for (int32 i = 0; i < Kkwallas.Num(); ++i)
				{
					if (NoteChecker.IsValidIndex(i) && NoteChecker[i] != -1 && IsValid(Kkwallas[i]))
					{
						Kkwallas[i]->BeerSuccess();
					}
				}
			}

			BeatCount++;
		}

		// �Է� ó��
		if (bAnyKeyDown && CurTimeSec < 6 * BPMTimeMs)
		{
			// ����) �̰� ������ ���� MakeTurn �̶� Checker �� ����� ��
			// PlayTurn �� ���� NoteEntered �� ����ؾ���
			for (int32 i = 0; i < Kkwallas.Num(); ++i)
			{
				// �ػ� �ε����� ���ų�, �̹� ���õ� ���̶�� ����
				if (NoteChecker.IsValidIndex(i) == false || NoteChecker[i] != -1)
				{
					continue;
				}
				else if (IsValid(Kkwallas[i]) && Kkwallas[i]->PointCheck(CachePos))
				{
					Kkwallas[i]->BeerReady();
					NoteChecker[i] = OnKeyDownTime;
				}
			}

			bAnyKeyDown = false;
			OnKeyDownTime = -1;
			CachePos = FVector2D::ZeroVector;
		}
	}
	else if (GameState == ManagerStateV3::PlayTurn)
	{
		// ���� ���� ������ ��
		if (8 * BPMTimeMs <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("TTT"));
			for (AKkwalla* Kkwalla : Kkwallas)
			{
				Kkwalla->Reset();
			}
			NoteEntered.Init(-1, 6);

			BeatCount = 0;
			CurTimeSec -= 8 * BPMTimeMs;
			// ���� ���� �Է��� ����
			bAnyKeyDown = true;
			OnKeyDownTime = -1;
			// ���� �÷��̾��
			CurTurnIndex++;
			int32 CurrentPlayer = PlayerTurnArr[CurTurnIndex];
			AudioComponent->SetParameter(TEXT("Turn"), 1.0f);
			HudWidget->ChangeBreakWidgetVisibility(true, ColorDataAsset->ColorData[CurrentPlayer].ColorA, ColorDataAsset->ColorData[CurrentPlayer].ColorB);
			GameState = ManagerStateV3::TurnBreak;
			return;
		}
		else if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("!!!"));

			for (AKkwalla* kkwalla : Kkwallas)
			{
				kkwalla->Update();
			}

			if (BeatCount == 6)
			{
				for (int32 i = 0; i < Kkwallas.Num(); ++i)
				{
					if (NoteChecker.IsValidIndex(i) && NoteEntered.IsValidIndex(i))
					{
						if (NoteChecker[i] == -1 || NoteEntered[i] == -1)
						{
							continue;
						}
						else if (NoteChecker[i] - CheckBias <= NoteEntered[i] && NoteEntered[i] <= NoteChecker[i] + CheckBias)
						{
							Kkwallas[i]->BeerFill();
						}
					}
				}
			}
			else if (BeatCount == 7)
			{
				for (int32 i = 0; i < Kkwallas.Num(); ++i)
				{
					if (NoteChecker.IsValidIndex(i) && NoteEntered.IsValidIndex(i))
					{
						if (NoteChecker[i] == -1 && NoteEntered[i] == -1)
						{
							continue;
						}
						else if (NoteChecker[i] == -1 || NoteEntered[i] == -1)
						{
							Kkwallas[i]->BeerFail();
						}
						else if (NoteChecker[i] - CheckBias <= NoteEntered[i] && NoteEntered[i] <= NoteChecker[i] + CheckBias)
						{
							Kkwallas[i]->BeerSuccess();
						}
						else
						{
							Kkwallas[i]->BeerFail();
						}
					}
				}
			}

			BeatCount++;
		}

		// �Է� ó��
		if (bAnyKeyDown && CurTimeSec < 6 * BPMTimeMs)
		{
			// ����) PlayTurn �̶� NoteEntered �� ���
			// 
			for (int32 i = 0; i < Kkwallas.Num(); ++i)
			{
				// �ػ� �ε����� ���ų�, �̹� ���õ� ���̶�� ����
				if (NoteEntered.IsValidIndex(i) == false || NoteEntered[i] != -1)
				{
					continue;
				}
				else if (IsValid(Kkwallas[i]) && Kkwallas[i]->PointCheck(CachePos))
				{
					Kkwallas[i]->BeerReady();
					NoteEntered[i] = OnKeyDownTime;
				}
			}

			bAnyKeyDown = false;
			OnKeyDownTime = -1;
			CachePos = FVector2D::ZeroVector;
		}
	}
	else if (GameState == ManagerStateV3::TurnBreak)
	{
		// ���� ���� 8��
		if (8 * BPMTimeMs <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("333"));
			BeatCount = 0;
			CurTimeSec -= 8 * BPMTimeMs;
			bAnyKeyDown = false;
			OnKeyDownTime = -1;
			AudioComponent->SetParameter(TEXT("Turn"), 0.0f);
			HudWidget->ChangeBreakWidgetVisibility(false);
			GameState = ManagerStateV3::PlayTurn;
			return;
		}
		else if ((BeatCount * BPMTimeMs) <= CurTimeSec)
		{
			UE_LOG(LogTemp, Error, TEXT("222"));
			switch (BeatCount)
			{
			case 4:
				HudWidget->PlayAnimCount(3);
				break;
			case 5:
				HudWidget->PlayAnimCount(2);
				break;
			case 6:
				HudWidget->PlayAnimCount(1);
				break;
			case 7:
				HudWidget->PlayAnimCount(4);
				break;
			default:
				break;
			}
			BeatCount++;
		}
	}
	else if (GameState == ManagerStateV3::Success)
	{

	}
	else if (GameState == ManagerStateV3::Fail)
	{
		
	}
}

void ANoteManagerV3::TouchInput(const FVector& InPos)
{
	// �ߺ� �Է� ���� (Tick���� �Һ� ������ �� �Է��� ����)
	if (bAnyKeyDown)
	{
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("%d"), CurTimeSec);
	bAnyKeyDown = true;
	OnKeyDownTime = CurTimeSec;
	CachePos = FVector2D(InPos);
}

void ANoteManagerV3::Restart()
{
	
}

void ANoteManagerV3::SetPlayerNum(const int32 InPlayerNum)
{
	PlayerNum = InPlayerNum;

	// ���� ����
	bGameStart = true;
}

void ANoteManagerV3::InitManager()
{
	APlayerController* PlayerCon = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerCon))
	{
		HudWidget = PlayerCon->GetHUD<AKkwallaHUD>();
	}

	if (IsValid(HudWidget))
	{
		HudWidget->InitSet();
	}

	if (IsValid(ColorDataAsset))
	{
		for (int32 BgIndex = 0; BgIndex < BgActors.Num(); ++BgIndex)
		{
			if (IsValid(BgActors[BgIndex]))
			{
				BgActors[BgIndex]->InitSet();

				if (ColorDataAsset->ColorData.IsValidIndex(BgIndex))
				{
					BgActors[BgIndex]->SetBGColor(ColorDataAsset->ColorData[BgIndex].ColorA, ColorDataAsset->ColorData[BgIndex].ColorB);
				}
			}
		}
	}
	for (AKkwalla* KkwallaIter : Kkwallas)
	{
		if (IsValid(KkwallaIter))
		{
			KkwallaIter->Reset();
		}
	}

	// �ʱ�ȭ �� �Է��� ��������
	bAnyKeyDown = true;
	OnKeyDownTime = -1;
	CachePos = FVector2D::ZeroVector;

	GameState = ManagerStateV3::PlayerSet;
	NoteChecker.Init(-1, 6);
	NoteEntered.Init(-1, 6);

	PlayerTurnArr.Reserve(8);
	PlayerNum = -1;
	for (int32 i = 0; i < 8; ++i)
	{
		if (PlayerTurnArr.IsValidIndex(i) == false)
		{
			PlayerTurnArr.Add(-1);
		}
		PlayerTurnArr[i] = i; 
	}
	CurTurnIndex = 0;

	BPMTimeMs = MS_TIME / ((float)BPM / 60);
	CurTimeSec = 0;
	BeatCount = 0;
	bGameStart = false;

	// #todo Show Player Num Set Page
}
