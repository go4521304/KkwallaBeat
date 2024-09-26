// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakPage.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"


void UBreakPage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UMaterialInterface* Mat = Cast<UMaterialInterface>(BGImage->GetBrush().GetResourceObject());
	if (IsValid(Mat))
	{
		MatInst = UMaterialInstanceDynamic::Create(Mat, nullptr, TEXT("MatInst"));
		BGImage->Brush.SetResourceObject(MatInst);
	}
}

void UBreakPage::SetBGColor(FLinearColor InColorA, FLinearColor InColorB)
{
	if (IsValid(MatInst))
	{
		FVector ColorA = { InColorA.R, InColorA.G, InColorA.B };
		FVector ColorB = { InColorB.R, InColorB.G, InColorB.B };

		MatInst->SetVectorParameterValue(TEXT("BG A"), ColorA);
		MatInst->SetVectorParameterValue(TEXT("BG B"), ColorB);
	}
}

void UBreakPage::PlayAnimA()
{
	if (IsValid(A))
	{
		PlayAnimation(A);
		UE_LOG(LogTemp, Warning, TEXT("A"));
	}
}

void UBreakPage::PlayAnimCount1()
{
	if (IsValid(Count1))
	{
		PlayAnimation(Count1);
		UE_LOG(LogTemp, Warning, TEXT("1"));
	}
}

void UBreakPage::PlayAnimCount2()
{
	if (IsValid(Count2))
	{
		PlayAnimation(Count2);
		UE_LOG(LogTemp, Warning, TEXT("2"));
	}
}

void UBreakPage::PlayAnimCount3()
{
	if (IsValid(Count3))
	{
		PlayAnimation(Count3);
		UE_LOG(LogTemp, Warning, TEXT("3"));
	}
}
