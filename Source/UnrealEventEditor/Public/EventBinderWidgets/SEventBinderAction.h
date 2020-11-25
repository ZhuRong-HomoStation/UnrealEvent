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
	{}
		SLATE_ARGUMENT(AActor*, TargetActor)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SEventBinderAction();

	void RebuildEventList();
private:
	FEventBinderActor& _GetTargetBinder();
	void _TryRemoveTargetBinder();
	
	void _OnAddEvent(FName EventName);
	void _OnDeleteEvent(TSharedRef<SEventItem> InItem);
	void _OnReplaceEvent(TSharedRef<SEventItem> InItem);

	void _MarkOuterDirty();
	void _CollectDelegates();
	TSharedRef<SWidget> _GetAddEventMenuContent();
private:
	// settings 
	UEventBinderAssetUserData*		TargetBinder;
	AActor*							TargetActor;

	// state 
	TMap<FName, FFunctionSignature>		DelegateMap;

	// events
	TSharedPtr<SVerticalBox>		EventItemList;
	TSharedPtr<SComboButton>		AddEventComboBtn;

	static TMap<TWeakObjectPtr<UObject>, TMap<FName,bool>> WinStateMap;
};
