// Fill out your copyright notice in the Description page of Project Settings.


#include "BGActorV2.h"
#include "PaperSpriteComponent.h"
#include "Camera/CameraComponent.h"
#include "Materials/MaterialInterface.h"

// Sets default values
ABGActorV2::ABGActorV2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BGSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BGSprite"));
	SetRootComponent(BGSprite);
}

// Called when the game starts or when spawned
void ABGActorV2::BeginPlay()
{
	Super::BeginPlay();
}

void ABGActorV2::InitSet()
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

void ABGActorV2::SetBGColor(FLinearColor InColorA, FLinearColor InColorB)
{
	FVector ColorA = { InColorA.R, InColorA.G, InColorA.B };
	FVector ColorB = { InColorB.R, InColorB.G, InColorB.B };

	MatInst->SetVectorParameterValue(TEXT("colorA"), ColorA);
	MatInst->SetVectorParameterValue(TEXT("colorB"), ColorB);
}

void ABGActorV2::SetPhase(const float InPhase)
{
	MatInst->SetScalarParameterValue(TEXT("Phase"), InPhase);
}
