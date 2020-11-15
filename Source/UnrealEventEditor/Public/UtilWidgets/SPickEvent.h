#pragma once 
#include "CoreMinimal.h"
#include "EventBinder.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SPickEvent : public SCompoundWidget
{
	using FSourceMap = TMap<FName, FFunctionSignature>;
public:
	DECLARE_DELEGATE_OneParam(FOnPickEvent, FName)
	DECLARE_DELEGATE_RetVal_OneParam(bool, FGetShouldPickEvent, FName)
	
	SLATE_BEGIN_ARGS(SPickEvent)
	{}
		SLATE_ARGUMENT(FSourceMap*, Source)
		SLATE_EVENT(FGetShouldPickEvent, GetShouldPickEvent)
		SLATE_EVENT(FOnPickEvent, OnPickEvent)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SPickEvent();

private:
	FReply _OnSelectEvent(FName InEventName);
	
	void _RebuildEventSignatureList();
	TSharedRef<SWidget>	_BuildEventBtn(FName InEventName);
private:
	// setting 
	TMap<FName, FFunctionSignature>*	SourceMap;

	// events
	FOnPickEvent		OnPickEvent;
	FGetShouldPickEvent GetShouldPickEvent;
	
	// widgets
	TSharedPtr<SEditableTextBox>	SearchBox;
	TSharedPtr<SVerticalBox>		EventSignatureList;
};

