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
	
}

// Called when the game starts or when spawned
void AKkwalla::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AKkwalla::PointCheck(FVector2D InPos)
{
	FVector Origin;
	FVector BoxExtent;

	// ������ �ٿ�� �ڽ��� ���ɴϴ�.
	GetActorBounds(true, Origin, BoxExtent);

	Origin.Z = 0.0f;
	BoxExtent.Z = 0.0f;

	// FBox ���� (�ּҿ� �ִ� �ڳʸ� ���� �ٿ�� �ڽ� ����)
	FBox2D ActorBox {FVector2D{Origin - BoxExtent}, FVector2D{Origin + BoxExtent}};

	if (ActorBox.IsInside(InPos))
	{
		BeerSuccess();
		return true;
	}
	return false;
}

//// Called every frame
//void AKkwalla::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AKkwalla::BeerReady()
{
}

void AKkwalla::BeerSuccess()
{
	UE_LOG(LogTemp, Error, TEXT("Success"));

}

void AKkwalla::BeerFail()
{
	UE_LOG(LogTemp, Error, TEXT("Fail"));

}
