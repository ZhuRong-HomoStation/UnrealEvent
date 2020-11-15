#pragma once 
#include "CoreMinimal.h"
#include "EventBinder.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SPickFunction;

class SDelegateItem : public SBorder
{
public:
	DECLARE_DELEGATE_OneParam(FOnBindActorChanged, AActor*)
	DECLARE_DELEGATE_OneParam(FOnBindFunctionChanged, FName)
	DECLARE_DELEGATE_OneParam(FOnItemSelected, TSharedRef<SDelegateItem>)
	
	SLATE_BEGIN_ARGS(SDelegateItem)
		: _DefaultActor(nullptr)
		, _Owner(nullptr)
	{}
		SLATE_ARGUMENT(FFunctionSignature, TargetSignature)
		SLATE_ARGUMENT(AActor*, DefaultActor)
		SLATE_ARGUMENT(AActor*, Owner)
		SLATE_ARGUMENT(FName, DefaultFunction)
		SLATE_ATTRIBUTE(bool, IsSelected)
		SLATE_EVENT(FOnItemSelected, OnItemSelected)
		SLATE_EVENT(FOnBindActorChanged, OnBindActorChanged)
		SLATE_EVENT(FOnBindFunctionChanged, OnBindFunctionChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SDelegateItem();
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	AActor* GetBindActor() { return BindActor; }
	FName GetBindFunction() { return BindFunction; }
private:
	const FSlateBrush* _GetBorderImage() const;
	void _OnPickedActor(AActor* InActor);
	void _OnPickFunction(FName InFuncName);
	bool _IsBindFunctionValid();
	TSharedRef<SWidget> _GetMenuWidget();
public:
	TAttribute<bool>	IsSelected;
private:
	// config 
	FFunctionSignature	TargetSignature;
	AActor*				Owner;

	// state
	AActor*				BindActor;
	FName				BindFunction;

	// delegate 
	FOnBindActorChanged		OnBindActorChanged;
	FOnBindFunctionChanged	OnBindFunctionChanged;
	FOnItemSelected			OnItemSelected;

	// widget
	TSharedPtr<SComboButton>	PickFunctionComboBtn;
	TSharedPtr<STextBlock>		PickFunctionText;
	TSharedPtr<SHorizontalBox>	PickActorBox;
};