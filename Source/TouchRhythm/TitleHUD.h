// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TitleHUD.generated.h"

/**
 * 
 */
UCLASS()
class TOUCHRHYTHM_API ATitleHUD : public AHUD
{
	GENERATED_BODY()
	
	ATitleHUD();

public:
	void InitSet();
	void ShowCreditPage(bool bShow);

public:
	UPROPERTY()
	TSoftClassPtr<UUserWidget> CreditPageWidgetAsset;

	class UCreditPageWidget* CreditPageWidget;
};
