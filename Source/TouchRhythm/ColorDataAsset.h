// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ColorDataAsset.generated.h"

/**
 * 
 */

USTRUCT()
struct FBGColor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FLinearColor ColorA;

	UPROPERTY(EditAnywhere)
	FLinearColor ColorB;
};

UCLASS()
class TOUCHRHYTHM_API UColorDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<FBGColor> ColorData;
};
