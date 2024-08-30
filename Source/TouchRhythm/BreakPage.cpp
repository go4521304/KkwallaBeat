// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakPage.h"

void UBreakPage::PlayAnimA()
{
	PlayAnimation(A, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("A"));
}

void UBreakPage::PlayAnimCount1()
{
	PlayAnimation(Count1, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("1"));
}

void UBreakPage::PlayAnimCount2()
{
	PlayAnimation(Count2, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("2"));
}

void UBreakPage::PlayAnimCount3()
{
	PlayAnimation(Count3, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("3"));
}
