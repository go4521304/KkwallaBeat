// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditPageWidget.generated.h"

/**
 * 
 */
class IManagerInterface;

UCLASS()
class TOUCHRHYTHM_API UCreditPageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void ShowCredit();

	UFUNCTION()
	void OnEndEdit();

private:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* end_edit;

	UPROPERTY()
	TScriptInterface<IManagerInterface> ManagerActor;
};
