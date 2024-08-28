// Fill out your copyright notice in the Description page of Project Settings.


#include "BGActor.h"
#include "PaperSpriteComponent.h"
#include "Materials/MaterialInterface.h"

// Sets default values
ABGActor::ABGActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BGSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BGSprite"));
	SetRootComponent(BGSprite);

}

// Called when the game starts or when spawned
void ABGActor::BeginPlay()
{
	Super::BeginPlay();

	TSoftObjectPtr<UMaterialInterface> MatPtr{ FSoftObjectPath(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/FxResouce/BG/M_BG_Inst.M_BG_Inst'")) };
	UMaterialInterface* Mat = MatPtr.LoadSynchronous();
	if (IsValid(Mat))
	{

		MatInst = BGSprite->CreateDynamicMaterialInstance(0, Mat);
		
		MatInst->SetVectorParameterValue(TEXT("colorA"), FVector(1.0f, 0.102431, 0.03648));
	}
}

// Called every frame
void ABGActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (calc)
	{
		phase += DeltaTime;

	}
	else
	{
		phase -= DeltaTime;

	}

	MatInst->SetScalarParameterValue(TEXT("Phase"), phase);

	if (phase > 1.0f || phase < 0.0f)
	{
		calc = !calc;
	}
}

