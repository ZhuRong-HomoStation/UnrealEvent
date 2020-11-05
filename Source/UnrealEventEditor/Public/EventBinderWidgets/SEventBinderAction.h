#pragma once
#include "CoreMinimal.h"
#include "EventBinder.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SEventItem;

class SEventBinderAction : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEventBinderAction)
		: _TargetBinder(nullptr)
		, _Outer(nullptr)
	{}
		SLATE_ARGUMENT(FEventBinder*, TargetBinder)
		SLATE_ARGUMENT(UObject*, Outer)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SEventBinderAction();

	void RebuildEventList();
private:
	void _OnAddEvent(FName EventName);
	void _OnDeleteEvent(TSharedRef<SEventItem> InItem);
	void _OnReplaceEvent(TSharedRef<SEventItem> InItem);

	void _MarkOuterDirty();
	void _CollectDelegates();
	TSharedRef<SWidget> _GetAddEventMenuContent();
private:
	// settings 
	FEventBinder*		TargetBinder;
	UObject*			Outer;

	// state 
	TMap<FName, FFunctionSignature>		DelegateMap;

	// events
	TSharedPtr<SVerticalBox>		EventItemList;
	TSharedPtr<SComboButton>		AddEventComboBtn;

	static TMap<TWeakObjectPtr<UObject>, TMap<FName,bool>> WinStateMap;
};
