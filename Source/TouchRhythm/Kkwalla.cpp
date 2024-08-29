// Fill out your copyright notice in the Description page of Project Settings.


#include "Kkwalla.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"

// Sets default values
AKkwalla::AKkwalla()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Chara = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Kkwalla"));
	
	static ConstructorHelpers::FObjectFinder<UPaperSprite> CharaSprite0(TEXT("/Script/Paper2D.PaperSprite'/Game/Img/Sprite/A_1_Sprite.A_1_Sprite'"));
	static ConstructorHelpers::FObjectFinder<UPaperSprite> CharaSprite1(TEXT("/Script/Paper2D.PaperSprite'/Game/Img/Sprite/A_2_Sprite.A_2_Sprite'"));
	static ConstructorHelpers::FObjectFinder<UPaperSprite> CharaSprite2(TEXT("/Script/Paper2D.PaperSprite'/Game/Img/Sprite/B_1_Sprite.B_1_Sprite'"));
	static ConstructorHelpers::FObjectFinder<UPaperSprite> CharaSprite3(TEXT("/Script/Paper2D.PaperSprite'/Game/Img/Sprite/B_2_Sprite.B_2_Sprite'"));
	static ConstructorHelpers::FObjectFinder<UPaperSprite> CharaSprite4(TEXT("/Script/Paper2D.PaperSprite'/Game/Img/Sprite/C_Sprite.C_Sprite'"));
	static ConstructorHelpers::FObjectFinder<UPaperSprite> CharaSprite5(TEXT("/Script/Paper2D.PaperSprite'/Game/Img/Sprite/D_Faild_Sprite.D_Faild_Sprite'"));

	if (CharaSprite0.Succeeded())
	{
		CharaSprite.Add(CharaSprite0.Object);
	}
	if (CharaSprite1.Succeeded())
	{
		CharaSprite.Add(CharaSprite1.Object);
	}
	if (CharaSprite2.Succeeded())
	{
		CharaSprite.Add(CharaSprite2.Object);
	}
	if (CharaSprite3.Succeeded())
	{
		CharaSprite.Add(CharaSprite3.Object);
	}
	if (CharaSprite4.Succeeded())
	{
		CharaSprite.Add(CharaSprite4.Object);
	}
	if (CharaSprite5.Succeeded())
	{
		CharaSprite.Add(CharaSprite5.Object);
	}

	CharaState = EKkwallaState::Idle;
}

// Called when the game starts or when spawned
void AKkwalla::BeginPlay()
{
	Super::BeginPlay();
	CurSpriteIndex = 0;
	Chara->SetSprite(CharaSprite[CurSpriteIndex]);
}

void AKkwalla::Update()
{
	switch (CharaState)
	{
		case EKkwallaState::Idle:
		{
			Chara->SetSprite(CharaSprite[CurSpriteIndex]);
			CurSpriteIndex++;
			CurSpriteIndex %= 2;
		}
		break;

		case EKkwallaState::Fill:
		{
			BeerSuccess();
		}

		case EKkwallaState::Ready:
		break;

		case EKkwallaState::Success:
		break;

		case EKkwallaState::Fail:
		break;
	default:
		break;
	}
}

void AKkwalla::Reset()
{
	CurSpriteIndex = 0;
	Chara->SetSprite(CharaSprite[CurSpriteIndex]);
	CharaState = EKkwallaState::Idle;
}

bool AKkwalla::PointCheck(FVector2D InPos)
{
	FVector Origin;
	FVector BoxExtent;

	// 액터의 바운딩 박스를 얻어옵니다.
	GetActorBounds(true, Origin, BoxExtent);

	Origin.Z = 0.0f;
	BoxExtent.Z = 0.0f;

	// FBox 생성 (최소와 최대 코너를 통해 바운딩 박스 생성)
	FBox2D ActorBox {FVector2D{Origin - BoxExtent}, FVector2D{Origin + BoxExtent}};

	if (ActorBox.IsInside(InPos))
	{
		return true;
	}
	return false;
}

void AKkwalla::BeerReady()
{
	Chara->SetSprite(CharaSprite[2]);
	CharaState = EKkwallaState::Ready;
}

void AKkwalla::BeerFill()
{
	Chara->SetSprite(CharaSprite[3]);
	CharaState = EKkwallaState::Fill;
}

void AKkwalla::BeerSuccess()
{
	Chara->SetSprite(CharaSprite[4]);
	CharaState = EKkwallaState::Success;
}

void AKkwalla::BeerFail()
{
	Chara->SetSprite(CharaSprite[5]);
	CharaState = EKkwallaState::Fail;
}
