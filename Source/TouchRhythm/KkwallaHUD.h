// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "KkwallaHUD.generated.h"

class UBreakPage;
class UUserWidget;

/**
 * 
 */
UCLASS()
class TOUCHRHYTHM_API AKkwallaHUD : public AHUD
{
	GENERATED_BODY()

	AKkwallaHUD();

	virtual void BeginPlay() override;

public:
	void InitSet();

	void ChangeBreakWidgetVisibility(bool bShow, FLinearColor InColorA = FLinearColor::Black, FLinearColor InColorB = FLinearColor::Black);
	void ShowFailePage(bool bShow);
	void PlayAnimCount(int32 InCount);
	
private:
	UPROPERTY()
	TSoftClassPtr<UUserWidget> BreakTimeWidgetAsset;
	UPROPERTY()
	TSoftClassPtr<UUserWidget> FailWidgetAsset;

	UPROPERTY()
	UBreakPage* BreakWidget;

	UPROPERTY()
	UUserWidget* FailWidget;
};
