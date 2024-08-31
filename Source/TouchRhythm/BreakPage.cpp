// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakPage.h"
#include "Animation/WidgetAnimation.h"

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
