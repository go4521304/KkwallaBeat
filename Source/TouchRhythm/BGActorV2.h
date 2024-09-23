// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BGActorV2.generated.h"

class UCameraComponent;
class UPaperSpriteComponent;
class UMaterialInstanceDynamic;

UCLASS()
class TOUCHRHYTHM_API ABGActorV2 : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABGActorV2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void InitSet();

	void SetBGColor(FLinearColor InColorA, FLinearColor InColorB);

	void SetPhase(const float InPhase);

	float phase = 0.0f;
	bool calc = false;

	
private:
	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* BGSprite;

	UPROPERTY()
	UMaterialInstanceDynamic* MatInst;
};

