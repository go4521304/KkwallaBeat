// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ManagerInterface.h"
#include "TitleManager.generated.h"

class AStaticMeshActor;
class ULevelSequence;

UCLASS()
class TOUCHRHYTHM_API ATitleManager : public AActor, public IManagerInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATitleManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// IManagerInterface
	virtual void TouchInput(const FVector2D& InPos) override;
	virtual void Restart() override;

private:
	UPROPERTY(EditAnywhere, Category = "Manager")
	AStaticMeshActor* Btn_Start;

	UPROPERTY(EditAnywhere, Category = "Manager")
	AStaticMeshActor* Btn_Credit;

	UPROPERTY(EditAnywhere, Category = "Manager")
	ULevelSequence* TitleSequence;
};
