#pragma once 
#include "CoreMinimal.h"
#include "EventBinder.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SDelegateItem;

class SEventItem : public SVerticalBox 
{
public:
	DECLARE_DELEGATE(FOnEventModify)
	DECLARE_DELEGATE_OneParam(FOnDeleteEvent, TSharedRef<SEventItem>)
	DECLARE_DELEGATE_OneParam(FOnReplaceEvent, TSharedRef<SEventItem>)
	
	SLATE_BEGIN_ARGS(SEventItem)
		: _Source(nullptr)
		, _IsValid(true)
	{}
		SLATE_ARGUMENT(FEventBindInfo*, Source)
		SLATE_ARGUMENT(FFunctionSignature, TargetSignature)
		SLATE_ARGUMENT(FName, EventName)
		SLATE_ARGUMENT(bool, IsValid)
		SLATE_EVENT(FOnEventModify, OnEventModify)
		SLATE_EVENT(FOnDeleteEvent, OnDeleteEvent)
		SLATE_EVENT(FOnReplaceEvent, OnReplaceEvent)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SEventItem();

	FName GetEventName() { return EventName; }
	FEventBindInfo* GetSource() { return EventBindInfoSource; }
	void SetSource(FEventBindInfo* InSource);
	FFunctionSignature GetSignature() { return TargetSignature; }
	void SetSignature(FFunctionSignature InSignature);
	bool GetIsCollapsed() { return bIsCollapsed; }
	void SetIsCollapsed(bool bInIsCollapsed);
private:
	FReply 	_OnHeadBtnClicked();
	FReply 	_OnAddDelegate();
	FReply 	_OnDeleteDelegate();
	void	_RefreshEventInfoWidgets();
	void 	_RefreshDelegateItemList();
	
	TSharedRef<SWidget> _AddDelegateItem(FDelegateBindInfo& InBindInfo);
private:
	// config
	FName				EventName;
	FFunctionSignature	TargetSignature;
	FEventBindInfo*		EventBindInfoSource;
	bool				bIsValid;
	
	// state
	bool				bIsCollapsed;
	TSharedPtr<SDelegateItem>	SelectedItem;

	// delegate
	FOnEventModify		OnEventModify;
	FOnDeleteEvent		OnDeleteEvent;
	FOnReplaceEvent		OnReplaceEvent;
	
	// widgets
	TSharedPtr<SButton>			HeadButton;
	TSharedPtr<STextBlock>		HeadBtnText;
	TSharedPtr<SBorder>			DelegateItemListArea;
	TSharedPtr<SVerticalBox>	DelegateItemList;
	TSharedPtr<SHorizontalBox>	BottomBar;
};

