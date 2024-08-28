// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoteManager.generated.h"

class AKkwalla;

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
	UPROPERTY(EditDefaultsOnly)
	TArray<AKkwalla*> Kkwallas;

	int32 TimeSec;

	bool bAnyKeyDown;
	int32 OnKeyDownTime;
};
