// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "fmod.hpp"
#include "ManagerInterface.h"
#include "NoteManagerV2.generated.h"

class ABGActorV2;
class AKkwalla;
class UFMODEvent;
class AKkwallaHUD;
class USoundWave;
class UColorDataAsset;

UENUM()
enum class ManagerStateV2
{
	PreBeat, MakeTurn, MakeTurnBreak, PlayTurn, PlayTurnBreak, Success, Fail
};

UCLASS()
class TOUCHRHYTHM_API ANoteManagerV2 : public AActor, public IManagerInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANoteManagerV2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// IManagerInterface
	virtual void TouchInput(const FVector& InPos) override;
	virtual void Restart() override;

private:
	const int32 MS_TIME = 1000;

	UPROPERTY()
	class UFMODAudioComponent* AudioComponent;
	
	UPROPERTY(EditAnywhere, Category = "Manager")
	TArray<ABGActorV2*> BgActors;

	UPROPERTY(EditAnywhere, Category = "Manager")
	TArray<AKkwalla*> Kkwallas;

	UPROPERTY(EditAnywhere, Category = "Manager")
	int32 GradeCheck;

	UPROPERTY(EditAnywhere, Category = "Manager")
	UFMODEvent* BGMusic;

	UPROPERTY(EditAnywhere, Category = "Manager")
	UFMODEvent* SFXSuccess;
	UPROPERTY(EditAnywhere, Category = "Manager")
	UFMODEvent* SFXFail;

	UPROPERTY(EditAnywhere, Category = "Manager")
	int32 BPM;

	UPROPERTY(EditAnywhere, Category = "Manager")
	int32 PreBeatCount;

	UPROPERTY(EditAnywhere, Category = "Manager")
	TObjectPtr<UColorDataAsset> ColorDataAsset;

	int32 BPMTimeMs;
	int32 BeatCount;
	int32 CurTimeSec;

	bool bAnyKeyDown;
	int32 OnKeyDownTime;
	FVector2D CachePos;

	ManagerStateV2 GameState;
	TArray<int32> PatternArr;
	TArray<int32> PatternArrCheck;

	// 플레이어 턴 관련
	int32 PlayerNum;
	TArray<int32> PlayerTurn;
	int32 CurTurnIndex;

	UPROPERTY()
	AKkwallaHUD* HudWidget;

	bool bGameStart;
};
