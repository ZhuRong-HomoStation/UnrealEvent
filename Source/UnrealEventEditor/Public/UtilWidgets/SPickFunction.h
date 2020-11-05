#pragma once 
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "EventBinder.h"

class SEditableTextBox;

class SPickFunction : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnPickFunction, FName)
	
	SLATE_BEGIN_ARGS(SPickFunction)
        : _TargetObject(nullptr)
        , _FilterSignature(true)
	{}
		SLATE_EVENT(FOnPickFunction, OnPickFunction)
	    SLATE_ARGUMENT(UObject*, TargetObject)
	    SLATE_ARGUMENT(bool, FilterSignature)
	    SLATE_ARGUMENT(FFunctionSignature, FunctionSignature)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SPickFunction();

	void SetTargetObject(UObject* InObj);
	void SetSignature(const TArray<FName>& InSignature);
	
private:
	FReply 	_OnSelectFunction(FName InFunction);
	void	_RebuildFunctionList();
	TSharedRef<SWidget>	_BuildFunctionBtn(FName InFuncName);
private:
	// config 
	UObject*					TargetObject;
	bool						bFilterSignature;
	FFunctionSignature			TargetSignature;
	TMap<FName,	FFunctionSignature>	AllFunctionSignature;
	
	// state
	TArray<FName>				FunctionsPassFilter;
	
	// event 
	FOnPickFunction				OnPickFunction;

	// widgets
	TSharedPtr<SEditableTextBox>	SearchBox;
	TSharedPtr<SVerticalBox>		FuncItemBox;
};

