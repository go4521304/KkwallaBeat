// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BGActor.generated.h"

class UPaperSpriteComponent;
class UMaterialInstanceDynamic;

UCLASS()
class TOUCHRHYTHM_API ABGActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABGActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float phase = 0.0f;
	bool calc = false;

	
private:
	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* BGSprite;

	UPROPERTY()
	UMaterialInstanceDynamic* MatInst;
};

