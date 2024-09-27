// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FailPageWidget.generated.h"

/**
 * 
 */
class IManagerInterface;

UCLASS()
class TOUCHRHYTHM_API UFailPageWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;

	UPROPERTY(meta=(BindWidget))
	class UImage* Img_fail;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Re;

	UPROPERTY()
	class UMaterialInstanceDynamic* FailMatInst;

	UPROPERTY()
	TScriptInterface<IManagerInterface> ManagerActor;

public:
	void SetFailPlayer(int32 InPlayerIndex);
	
	UFUNCTION()
	void RetryGame();
};
