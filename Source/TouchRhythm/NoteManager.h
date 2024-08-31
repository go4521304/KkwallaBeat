// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoteManager.generated.h"

class ABGActor;
class AKkwalla;
class UFMODEvent;
class AKkwallaHUD;

UENUM()
enum class ManagerState
{
	PreBeat, MakeTurn, MakeTurnBreak, PlayTurn, PlayTurnBreak, Fail
};

UCLASS()
class TOUCHRHYTHM_API ANoteManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANoteManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TouchInput(const FVector2D& InPos);

private:
	UPROPERTY()
	class UFMODAudioComponent* AudioComponent;
	
	UPROPERTY(EditAnywhere, Category = "Manager")
	ABGActor* BgActor;

	UPROPERTY(EditAnywhere, Category = "Manager")
	TArray<AKkwalla*> Kkwallas;

	UPROPERTY(EditAnywhere, Category = "Manager")
	int32 BPM;

	UPROPERTY(EditAnywhere, Category = "Manager")
	int32 GradeCheck;

	UPROPERTY(EditAnywhere, Category = "Manager")
	UFMODEvent* BGMusic;

	UPROPERTY(EditAnywhere, Category = "Manager")
	int32 BGMBias;

	UPROPERTY(EditAnywhere, Category = "Manager")
	int32 PreBeatCount;

	int32 BPMTimeMs;
	int32 BeatCount;
	int32 CurTimeSec;

	bool bAnyKeyDown;
	int32 OnKeyDownTime;
	FVector2D CachePos;

	bool IsBGMPlay;

	ManagerState GameState;
	UPROPERTY()
	TArray<int32> PatternArr;
	UPROPERTY()
	TArray<int32> PatternArrCheck;
	int32 PlayerNum;

	UPROPERTY()
	AKkwallaHUD* HudWidget;
};
