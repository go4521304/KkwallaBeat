// Fill out your copyright notice in the Description page of Project Settings.


#include "CreditPageWidget.h"
#include "Animation/WidgetAnimation.h"
#include "ManagerInterface.h"
#include "EngineUtils.h"
#include "Animation/WidgetAnimationEvents.h"

void UCreditPageWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	for (AActor* Iter : TActorRange<AActor>(GetWorld()))
	{
		if (IsValid(Iter) && Iter->Implements<UManagerInterface>())
		{
			ManagerActor = Iter;
		}
	}
}

void UCreditPageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCreditPageWidget::ShowCredit()
{
	PlayAnimation(end_edit);

	FWidgetAnimationDynamicEvent FinishEvent;
	FinishEvent.BindDynamic(this, &UCreditPageWidget::OnEndEdit);
	BindToAnimationFinished(end_edit, FinishEvent);
}

void UCreditPageWidget::OnEndEdit()
{
	ManagerActor->Restart();
}
