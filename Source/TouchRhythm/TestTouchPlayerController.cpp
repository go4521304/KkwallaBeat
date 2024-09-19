// Fill out your copyright notice in the Description page of Project Settings.


#include "TestTouchPlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "NoteManager.h"
#include "NoteManagerV2.h"
#include "EngineUtils.h"

ATestTouchPlayerController::ATestTouchPlayerController()
{
	SetShowMouseCursor(true);
	bIsTouch = false;
}

void ATestTouchPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsValid(DefaultMappingContext) == false)
	{
		TSoftObjectPtr<UInputMappingContext> DefaultMappingContextPtr{ FSoftObjectPath(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/TopDown/Input/IMC_Default.IMC_Default'")) };
		DefaultMappingContext = DefaultMappingContextPtr.LoadSynchronous();
	}

	if (IsValid(ClickedAction) == false)
	{
		TSoftObjectPtr<UInputAction> ClickedActionPtr{ FSoftObjectPath(TEXT("/Script/EnhancedInput.InputAction'/Game/TopDown/Input/Actions/IA_SetDestination_Click.IA_SetDestination_Click'")) };
		ClickedAction = ClickedActionPtr.LoadSynchronous();
	}

	if (IsValid(TouchAction) == false)
	{
		TSoftObjectPtr<UInputAction> TouchActionPtr{ FSoftObjectPath(TEXT("/Script/EnhancedInput.InputAction'/Game/TopDown/Input/Actions/IA_SetDestination_Touch.IA_SetDestination_Touch'")) };
		TouchAction = TouchActionPtr.LoadSynchronous();
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ClickedAction, ETriggerEvent::Triggered, this, &ATestTouchPlayerController::OnClickedTriggered);
		EnhancedInputComponent->BindAction(TouchAction, ETriggerEvent::Triggered, this, &ATestTouchPlayerController::OnTouchTriggered);
	}
}

void ATestTouchPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(TouchParticle) == false)
	{
		TSoftObjectPtr<UNiagaraSystem> TouchParticlePtr{ FSoftObjectPath(TEXT("/Script/Niagara.NiagaraSystem'/Game/FxResouce/Touch/NS_Hit_A.NS_Hit_A'")) };
		TouchParticle = TouchParticlePtr.LoadSynchronous();
	}

	for (ANoteManager* Iter : TActorRange<ANoteManager>(GetWorld()))
	{
		if (IsValid(Iter))
		{
			NoteManager = Iter;
			break;
		}
	}

	for (ANoteManagerV2* Iter : TActorRange<ANoteManagerV2>(GetWorld()))
	{
		if (IsValid(Iter))
		{
			NoteManagerV2 = Iter;
			break;
		}
	}

	SetShowMouseCursor(true);
}

void ATestTouchPlayerController::OnClickedTriggered()
{
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}
	bIsTouch = false;

	if (bHitSuccessful == false)
	{
		return;
	}

	FVector HitPos = Hit.Location;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TouchParticle, HitPos);

	//UE_LOG(LogTemp, Error, TEXT("%f, %f, %f"), HitPos.X, HitPos.Y, HitPos.Z);

	if (IsValid(NoteManager))
	{
		NoteManager->TouchInput(FVector2D(HitPos));
	}

	if (IsValid(NoteManagerV2))
	{
		NoteManagerV2->TouchInput(FVector2D(HitPos));
	}
}

void ATestTouchPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnClickedTriggered();
}
