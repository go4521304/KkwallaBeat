// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ManagerInterface.h"
#include "NoteManagerV3.generated.h"

UENUM()
enum class ManagerStateV3
{
	None, PlayerSet, StandBy, PreBeat, MakeTurn, PlayTurn, TurnBreak, Success, Fail
};

class UFMODAudioComponent;
class ABGActorV2;
class AKkwalla;
class UFMODEvent;
class UColorDataAsset;
class AKkwallaHUD;

UCLASS()
class TOUCHRHYTHM_API ANoteManagerV3 : public AActor, public IManagerInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANoteManagerV3();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// IManagerInterface
	virtual void TouchInput(const FVector& InPos) override;
	virtual void Restart() override;
	virtual void SetPlayerNum(const int32 InPlayerNum) override;

private:
	void InitManager();

private:
	const int32 MS_TIME = 1000;

	UPROPERTY()
	AKkwallaHUD* HudWidget;

	// Sound 
	UPROPERTY()
	UFMODAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, Category = "Manager")
	UFMODEvent* BGMusic;

	UPROPERTY(EditAnywhere, Category = "Manager")
	int32 BPM;

	// BG 
	UPROPERTY(EditAnywhere, Category = "Manager")
	TArray<ABGActorV2*> BgActors;

	UPROPERTY(EditAnywhere, Category = "Manager")
	TObjectPtr<UColorDataAsset> ColorDataAsset;

	// Chara Btn 
	UPROPERTY(EditAnywhere, Category = "Manager")
	TArray<AKkwalla*> Kkwallas;

	// SFX 
	UPROPERTY(EditAnywhere, Category = "Manager")
	UFMODEvent* SFXSuccess;

	UPROPERTY(EditAnywhere, Category = "Manager")
	UFMODEvent* SFXFail;

	// Touch Input Val
	bool bAnyKeyDown;
	int32 OnKeyDownTime;
	FVector2D CachePos;

	// Note
	ManagerStateV3 GameState;
	TArray<int32> NoteChecker;
	TArray<int32> NoteEntered;
	UPROPERTY(EditAnywhere, Category = "Manager")
	int32 CheckBias = 1000;

	// Player
	int32 PlayerNum;
	TArray<int32> PlayerTurnArr;
	int32 CurTurnIndex;

	// Play
	int32 BPMTimeMs;
	int32 CurTimeSec;
	int32 BeatCount;
	bool bGameStart;
};
