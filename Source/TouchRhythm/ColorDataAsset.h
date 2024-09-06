// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ColorDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TOUCHRHYTHM_API UColorDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<FLinearColor> ColorData;
};
