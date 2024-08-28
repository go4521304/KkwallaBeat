// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kkwalla.generated.h"

class UPaperSpriteComponent;

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

	bool PointCheck(FVector2D InPos);

	void BeerReady();
	void BeerSuccess();
	void BeerFail();

private:
	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* Chara;
};
