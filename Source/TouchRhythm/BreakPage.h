// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BreakPage.generated.h"

/**
 * 
 */
UCLASS()
class TOUCHRHYTHM_API UBreakPage : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UImage* BGImage;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* A;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Count1;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Count2;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Count3;

	UPROPERTY()
	class UMaterialInstanceDynamic* MatInst;

public:
	virtual void NativeOnInitialized() override;

	void SetBGColor(FLinearColor InColorA, FLinearColor InColorB);
	void PlayAnimA();
	void PlayAnimCount1();
	void PlayAnimCount2();
	void PlayAnimCount3();
};
