// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestTouchPlayerController.generated.h"

/**
 * 
 */

 class UInputMappingContext;
 class UInputAction;
 class UNiagaraSystem;
 class IManagerInterface;

UCLASS()
class TOUCHRHYTHM_API ATestTouchPlayerController : public APlayerController
{
	GENERATED_BODY()

	ATestTouchPlayerController();

protected:
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

private:
	void OnClickedTriggered();
	void OnTouchTriggered();

private:
	// ¿¡¼Âµé
	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* ClickedAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* TouchAction;
	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* TouchParticle;

	UPROPERTY()
	TScriptInterface<IManagerInterface> ManagerActor;

	bool bIsTouch;
};
