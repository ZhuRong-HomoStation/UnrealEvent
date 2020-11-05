#include "EventBinderDetailView.h"
#include "DetailWidgetRow.h"
#include "EventBinderComponent.h"
#include "IDetailChildrenBuilder.h"
#include "EventBinderWidgets/SEventBinderAction.h"

TSharedRef<IPropertyTypeCustomization> EventBinderDetailView::MakeInstance()
{
	return MakeShareable(new EventBinderDetailView());
}

void EventBinderDetailView::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// TODO: add paste action and copy action 
}

void EventBinderDetailView::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{	
	// find outers 
	TArray<UObject*>	Objs;
	PropertyHandle->GetOuterObjects(Objs);
	
	if (Objs.Num() != 1)
	{
		ChildBuilder.AddCustomRow(FText::FromString(TEXT("EventBinder")))
        [
            SNew(STextBlock)
            .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"),20))
            .ColorAndOpacity(FSlateColor(FLinearColor::Red))
            .Text(FText::FromString(TEXT("Please select single EventBinder")))
        ];
		return;
	}
	
	// find out actor 
	AActor* OutActor = Cast<AActor>(Objs[0]);
	UEventBinderComponent* OutBinderComp = Cast<UEventBinderComponent>(Objs[0]);

	if (!OutActor && !OutBinderComp)
	{
		ChildBuilder.AddCustomRow(FText::FromString(TEXT("EventBinder")))
        [
            SNew(STextBlock)
            .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"),20))
            .ColorAndOpacity(FSlateColor(FLinearColor::Red))
            .Text(FText::FromString(TEXT("Event binder is only avaliable for AActor")))
        ];
		return;
	}
	else if (OutBinderComp)
	{
		OutActor = OutBinderComp->GetOwner();
		if (!OutActor)
		{
			ChildBuilder.AddCustomRow(FText::FromString(TEXT("EventBinder")))
	        [
	            SNew(STextBlock)
	            .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"),20))
	            .ColorAndOpacity(FSlateColor(FLinearColor::Red))
	            .Text(FText::FromString(TEXT("Can't edit EventBinder here")))
	        ];
			return;
		}
	}
	
	// get address 
	void* TargetAddress;
	FPropertyAccess::Result Result = PropertyHandle->GetValueData(TargetAddress);
	check(Result == FPropertyAccess::Success);
	
	// gen widget 
	ChildBuilder.AddCustomRow(FText::FromString(TEXT("EventBinder")))
	[
		SNew(SEventBinderAction)
		.TargetBinder(static_cast<FEventBinder*>(TargetAddress))
		.Outer(OutActor)
	];
}
