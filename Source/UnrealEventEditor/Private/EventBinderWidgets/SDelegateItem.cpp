#include "EventBinderWidgets/SDelegateItem.h"


#include "EventBinderLibrary.h"
#include "UtilWidgets/SPickActor.h"
#include "UtilWidgets/SPickFunction.h"

void SDelegateItem::Construct(const FArguments& InArgs)
{
	TargetSignature = InArgs._TargetSignature;
	BindActor = InArgs._DefaultActor;
	BindFunction = InArgs._DefaultFunction;
	IsSelected = InArgs._IsSelected;
	OnItemSelected = InArgs._OnItemSelected;
	OnBindActorChanged = InArgs._OnBindActorChanged;
	OnBindFunctionChanged = InArgs._OnBindFunctionChanged;
	Owner = InArgs._Owner;

	// border style 
	SBorder::FArguments Args;
	Args.BorderImage_Raw(this, &SDelegateItem::_GetBorderImage);
	SBorder::Construct(Args);

	static TSharedPtr<FButtonStyle> BtnStyle;
	if (!BtnStyle.IsValid())
	{
		BtnStyle = MakeShared<FButtonStyle>(FEditorStyle::Get().GetWidgetStyle<FButtonStyle>(TEXT("PropertyEditor.AssetComboStyle")));
		BtnStyle->Normal.TintColor = FLinearColor(0.07,0.07,0.07,1);
		BtnStyle->Hovered.TintColor = FLinearColor(0.04,0.04,0.04,1);
		BtnStyle->Pressed.TintColor = FLinearColor(0.03,0.03,0.03,1);
	}

	bool bIsFunctionValid = _IsBindFunctionValid();
	
	// build content
	ChildSlot
	.Padding(10,5,10,5)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(2)
		[
			SAssignNew(PickFunctionComboBtn, SComboButton)
			.ButtonStyle(BtnStyle.Get())
            .OnGetMenuContent_Raw(this, &SDelegateItem::_GetMenuWidget)
            .ButtonContent()
            [
                SAssignNew(PickFunctionText, STextBlock)
                .ColorAndOpacity_Lambda([&] { return _IsBindFunctionValid() ? FLinearColor::White : FLinearColor::Red; })
                .Text_Lambda([&]{ return _IsBindFunctionValid() ? FText::FromName(BindFunction) : FText::FromString(TEXT("InvalidFunction")); })
            ]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(2)
		[
			SAssignNew(PickActorBox, SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1)
			.VAlign(VAlign_Fill)
			.Padding(0,0,5,0)
			[
				SNew(SPickActor)
				.DefaultActor(BindActor)
				.OnShouldPickActor_Lambda([&](const AActor* Actor) { return true; })
				.OnActorSelected_Raw(this, &SDelegateItem::_OnPickedActor)
			]
			+ SHorizontalBox::Slot()
            .FillWidth(1)
            .VAlign(VAlign_Fill)
            [
            	SNew(SSpacer)
            ]
		]
	];
	
}

SDelegateItem::~SDelegateItem()
{
}

FReply SDelegateItem::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SBorder::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		OnItemSelected.ExecuteIfBound(StaticCastSharedRef<SDelegateItem>(this->AsShared()));
	}
	return FReply::Handled();
}

const FSlateBrush* SDelegateItem::_GetBorderImage() const
{
	static FSlateBrush BorderBrush;
	BorderBrush.TintColor = FLinearColor(0.15, 0.15, 0.15, (IsSelected.Get() ? 1 : 0));
	return &BorderBrush;
}

void SDelegateItem::_OnPickedActor(AActor* InActor)
{
	BindActor = InActor;
	OnBindActorChanged.ExecuteIfBound(InActor);
}

void SDelegateItem::_OnPickFunction(FName InFuncName)
{
	PickFunctionComboBtn->SetIsOpen(false);
	PickFunctionText->SetText(FText::FromName(InFuncName));
	BindFunction = InFuncName;
	OnBindFunctionChanged.ExecuteIfBound(InFuncName);
}

bool SDelegateItem::_IsBindFunctionValid()
{
	if (!BindActor) return true;
	if (BindFunction == NAME_None) return true;
	UFunction* Func = BindActor->GetClass()->FindFunctionByName(BindFunction);
	if (!Func) return false;
	return TargetSignature == FEventBinderLibrary::GetFunctionSignature(Func);
}

TSharedRef<SWidget> SDelegateItem::_GetMenuWidget()
{
	return SNew(SPickFunction)
	.OnPickFunction_Raw(this, &SDelegateItem::_OnPickFunction)
	.TargetObject(BindActor)
	.FunctionSignature(TargetSignature);
}
