#include "UtilWidgets/SPickFunction.h"
#include "EventBinderLibrary.h"
#include "Widgets/Input/SEditableTextBox.h"

void SPickFunction::Construct(const FArguments& InArgs)
{
	OnPickFunction = InArgs._OnPickFunction;
	bFilterSignature = InArgs._FilterSignature;
	TargetObject = InArgs._TargetObject;
	TargetSignature = InArgs._FunctionSignature;
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.AutoHeight()
		[
			SAssignNew(SearchBox, SEditableTextBox)
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(FuncItemBox, SVerticalBox)
		]
	];

	_RebuildFunctionList();
}

SPickFunction::~SPickFunction()
{
}

FReply SPickFunction::_OnSelectFunction(FName InFunction)
{
	OnPickFunction.ExecuteIfBound(InFunction);
	return FReply::Handled();
}

void SPickFunction::_RebuildFunctionList()
{
	// clean up
	FunctionsPassFilter.Reset();
	AllFunctionSignature.Reset();
	FuncItemBox->ClearChildren();
	
	// early out
	if (!TargetObject)
	{
		return;
	}

	// collect signatures
	for(TFieldIterator<UFunction> FuncIt(
        TargetObject->GetClass(),
            EFieldIteratorFlags::IncludeSuper,
            EFieldIteratorFlags::ExcludeDeprecated,
            EFieldIteratorFlags::IncludeInterfaces); FuncIt; ++FuncIt)
	{
		// skip
		if ((FuncIt->FunctionFlags & EFunctionFlags::FUNC_Delegate) ||
            (FuncIt->FunctionFlags & EFunctionFlags::FUNC_Static) ||
            (FuncIt->FunctionFlags & EFunctionFlags::FUNC_EditorOnly))
		{
			continue;
		}

		// fill signature
		AllFunctionSignature.Add(FName(FuncIt->GetName()), FEventBinderLibrary::GetFunctionSignature(*FuncIt));
	}

	// filter function signature
	for (auto & Signature : AllFunctionSignature)
	{
		if (bFilterSignature && Signature.Value != TargetSignature)
		{
			continue;
		}
		FunctionsPassFilter.Add(Signature.Key);
	}

	// build user interface
	for (auto & FunctionName : FunctionsPassFilter)
	{
		FuncItemBox->AddSlot()
		.AutoHeight()
		[
			_BuildFunctionBtn(FunctionName)
		];
	}
}

TSharedRef<SWidget> SPickFunction::_BuildFunctionBtn(FName InFuncName)
{
	static TSharedPtr<FButtonStyle> BtnStyle;
	if (!BtnStyle.IsValid())
	{
		BtnStyle = MakeShared<FButtonStyle>(FCoreStyle::Get().GetWidgetStyle<FButtonStyle>(ISlateStyle::Join(TEXT("Menu"), ".Button")));
		FSlateBrush Brush;
		Brush.TintColor = FLinearColor(0.1,0.1,0.1,1);
		BtnStyle->Hovered = Brush;
	}
	
	return SNew(SButton)
	    .ButtonStyle(BtnStyle.Get())
	    .OnClicked_Raw(this, &SPickFunction::_OnSelectFunction, InFuncName)
	    [
	        SNew(STextBlock)
	        .Text(FText::FromName(InFuncName))
	        .ColorAndOpacity(FLinearColor::White)
	    ];
}
