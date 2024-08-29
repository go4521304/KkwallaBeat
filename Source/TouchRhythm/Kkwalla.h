// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kkwalla.generated.h"

class UPaperSprite;
class UPaperSpriteComponent;

UENUM()
enum class EKkwallaState
{
	Idle, Ready, Fill,Success, Fail,
};

UCLASS()
class TOUCHRHYTHM_API AKkwalla : public AActor
{
	GENERATED_BODY()
	
public:	
	AKkwalla();

protected:
	virtual void BeginPlay() override;

public:	
	//virtual void Tick(float DeltaTime) override;

	void Update();
	void Reset();

	bool PointCheck(FVector2D InPos);

	void BeerReady();
	void BeerFill();
	void BeerSuccess();
	void BeerFail();

private:
	EKkwallaState CharaState;

	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* Chara;

	UPROPERTY(EditAnywhere)
	TArray<UPaperSprite*> CharaSprite;

	int32 CurSpriteIndex;
};
