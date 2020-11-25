#include "EventBinderDetailView.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "EventBinderWidgets/SEventBinderAction.h"

TSharedRef<IPropertyTypeCustomization> EventBinderDetailView::MakeInstance()
{
	return MakeShareable(new EventBinderDetailView());
}

void EventBinderDetailView::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void EventBinderDetailView::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// set sort order 
	IDetailCategoryBuilder& Builder = ChildBuilder.GetParentCategory().GetParentLayout().
	EditCategory(
		TEXT("DataBinder"),
		FText::FromString(TEXT("EventBinder")),
		ECategoryPriority::Important);
	
	// find outers 
	TArray<UObject*>	Objs;
	PropertyHandle->GetOuterObjects(Objs);

	// not support multiedit 
	if (Objs.Num() != 1)
	{
		Builder.AddCustomRow(FText::FromString(TEXT("EventBinder")))
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

	if (!OutActor)
	{
		Builder.AddCustomRow(FText::FromString(TEXT("EventBinder")))
        [
            SNew(STextBlock)
            .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"),20))
            .ColorAndOpacity(FSlateColor(FLinearColor::Red))
            .Text(FText::FromString(TEXT("Event binder is only avaliable for AActor")))
        ];
		return;
	}
	
	// gen widget 
	Builder.AddCustomRow(FText::FromString(TEXT("EventBinder")))
	[
		SNew(SEventBinderAction)
		.TargetActor(OutActor)
	];
}
