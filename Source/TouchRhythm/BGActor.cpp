// Fill out your copyright notice in the Description page of Project Settings.


#include "BGActor.h"
#include "PaperSpriteComponent.h"
#include "Camera/CameraComponent.h"
#include "Materials/MaterialInterface.h"

// Sets default values
ABGActor::ABGActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BGSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BGSprite"));
	SetRootComponent(BGSprite);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(GetRootComponent());
	CameraComp->SetRelativeLocationAndRotation(FVector(0.0f, -500.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f));
	CameraComp->SetProjectionMode(ECameraProjectionMode::Orthographic);
	CameraComp->SetOrthoWidth(1920.0f);
	CameraComp->SetConstraintAspectRatio(true);
	CameraComp->SetAspectRatio(1920.0f / 1080.f);
}

// Called when the game starts or when spawned
void ABGActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABGActor::InitSet()
{
	TSoftObjectPtr<UMaterialInterface> MatPtr{ FSoftObjectPath(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/FxResouce/BG/M_BG_Inst.M_BG_Inst'")) };
	UMaterialInterface* Mat = MatPtr.LoadSynchronous();
	if (IsValid(Mat))
	{

		MatInst = BGSprite->CreateDynamicMaterialInstance(0, Mat);
		MatInst->SetScalarParameterValue(TEXT("Phase"), 0.0f);

		MatInst->SetVectorParameterValue(TEXT("colorA"), FVector(0.99215f, 0.45882f, 0.43921f));
		MatInst->SetVectorParameterValue(TEXT("colorB"), FVector(1.0f, 1.0f, 1.0f));
	}
}

void ABGActor::SetBGColor(FLinearColor InColorA, FLinearColor InColorB)
{
	FVector ColorA = { InColorA.R, InColorA.G, InColorA.B };
	FVector ColorB = { InColorB.R, InColorB.G, InColorB.B };

	MatInst->SetVectorParameterValue(TEXT("colorA"), ColorA);
	MatInst->SetVectorParameterValue(TEXT("colorB"), ColorB);
}

void ABGActor::SetPhase(const float InPhase)
{
	MatInst->SetScalarParameterValue(TEXT("Phase"), InPhase);
}
