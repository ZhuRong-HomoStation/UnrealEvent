#include "UtilWidgets/SSelectActorDialog.h"



#include "AssetRegistryModule.h"
#include "Editor.h"
#include "SceneOutlinerFwd.h"
#include "SceneOutlinerModule.h"
#include "SceneOutlinerPublicTypes.h"
#include "Engine/Selection.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "SelectActorDialog"

void SSelectActorDialog::Construct(const FArguments& InArgs)
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	MenuBuilder.BeginSection(NAME_None, LOCTEXT("CurrentActorOperationsHeader", "Current Actor"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("UseSelected", "Use Selected"), 
			LOCTEXT("UseSelected_Tooltip", "Use the currently selected Actor"),
			FSlateIcon(),
			FUIAction( FExecuteAction::CreateSP( this, &SSelectActorDialog::_HandleUseSelected ) ) );
	
		if(CurrentActor)
		{
			MenuBuilder.AddMenuEntry(
				LOCTEXT("EditAsset", "Edit"), 
				LOCTEXT("EditAsset_Tooltip", "Edit this asset"),
				FSlateIcon(),
				FUIAction( FExecuteAction::CreateSP( this, &SSelectActorDialog::_OnEdit ) ) );
		}
	
		MenuBuilder.AddMenuEntry(
			LOCTEXT("CopyAsset", "Copy"),
			LOCTEXT("CopyAsset_Tooltip", "Copies the asset to the clipboard"),
			FSlateIcon(),
			FUIAction( FExecuteAction::CreateSP( this, &SSelectActorDialog::_OnCopy ) )
		);
	
		MenuBuilder.AddMenuEntry(
			LOCTEXT("PasteAsset", "Paste"),
			LOCTEXT("PasteAsset_Tooltip", "Pastes an asset from the clipboard to this field"),
			FSlateIcon(),
			FUIAction( 
				FExecuteAction::CreateSP( this, &SSelectActorDialog::_OnPaste ),
				FCanExecuteAction::CreateSP( this, &SSelectActorDialog::_CanPaste ) )
		);
	
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ClearAsset", "Clear"),
			LOCTEXT("ClearAsset_ToolTip", "Clears the asset set on this field"),
			FSlateIcon(),
			FUIAction( FExecuteAction::CreateSP( this, &SSelectActorDialog::_OnClear ) )
			);
	}
	MenuBuilder.EndSection();
	
	MenuBuilder.BeginSection(NAME_None, LOCTEXT("BrowseHeader", "Browse"));
	{
		TSharedPtr<SWidget> MenuContent;
	
		FSceneOutlinerModule& SceneOutlinerModule = FModuleManager::Get().LoadModuleChecked<FSceneOutlinerModule>(TEXT("SceneOutliner"));
	
		SceneOutliner::FInitializationOptions InitOptions;
		InitOptions.Mode = ESceneOutlinerMode::ActorPicker;
		InitOptions.Filters->AddFilterPredicate(
			FOnShouldFilterActor::CreateLambda([](const AActor*){ return true; }));
		InitOptions.bFocusSearchBoxWhenOpened = true;
	
		InitOptions.ColumnMap.Add(SceneOutliner::FBuiltInColumnTypes::Label(), SceneOutliner::FColumnInfo(SceneOutliner::EColumnVisibility::Visible, 0) );
		InitOptions.ColumnMap.Add(SceneOutliner::FBuiltInColumnTypes::ActorInfo(), SceneOutliner::FColumnInfo(SceneOutliner::EColumnVisibility::Visible, 10) );
		
		MenuContent =
			SNew(SBox)
			.WidthOverride(400)
			.HeightOverride(400)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SceneOutlinerModule.CreateSceneOutliner(
					InitOptions,
					FOnActorPicked::CreateSP(this, &SSelectActorDialog::_OnActorSelected))
			];
	
		MenuBuilder.AddWidget(MenuContent.ToSharedRef(), FText::GetEmpty(), true);
	}
	MenuBuilder.EndSection();

	// init window 
	SWindow::Construct( SWindow::FArguments()
        .Title(FText::FromString(TEXT("PickActor")))
        .SizingRule(ESizingRule::Autosized)
        .SupportsMaximize(false)
        .SupportsMinimize(false)
	[
		MenuBuilder.MakeWidget()
	]);
}

SSelectActorDialog::~SSelectActorDialog()
{
}

AActor* SSelectActorDialog::ShowModal()
{
	FSlateApplication::Get().AddModalWindow(
        StaticCastSharedRef<SWindow>(this->AsShared()),
        FGlobalTabmanager::Get()->GetRootWindow());
	return CurrentActor;
}

void SSelectActorDialog::_HandleUseSelected()
{
	CurrentActor = Cast<AActor>(GEditor->GetSelectedActors()->GetSelectedObject(0));
	FSlateApplication::Get().DestroyWindowImmediately(StaticCastSharedRef<SWindow>(this->AsShared()));
}

void SSelectActorDialog::_OnEdit()
{
	if(CurrentActor)
	{
		GEditor->EditObject( CurrentActor );
	}
	FSlateApplication::Get().DestroyWindowImmediately(StaticCastSharedRef<SWindow>(this->AsShared()));
}

void SSelectActorDialog::_OnCopy()
{
	FAssetData CurrentAssetData(CurrentActor);

	if( CurrentAssetData.IsValid() )
	{
		FPlatformApplicationMisc::ClipboardCopy(*CurrentAssetData.GetExportTextName());
	}
	FSlateApplication::Get().DestroyWindowImmediately(StaticCastSharedRef<SWindow>(this->AsShared()));
}

void SSelectActorDialog::_OnPaste()
{
	FString DestPath;
	FPlatformApplicationMisc::ClipboardPaste(DestPath);

	if(DestPath == TEXT("None"))
	{
		_OnActorSelected(NULL);
	}
	else
	{
		AActor* Actor = LoadObject<AActor>(NULL, *DestPath);
		if(Actor && (!ActorFilter.IsBound() || ActorFilter.Execute(Actor)))
		{
			_OnActorSelected(Actor);
		}
	}
	FSlateApplication::Get().DestroyWindowImmediately(StaticCastSharedRef<SWindow>(this->AsShared()));
}

bool SSelectActorDialog::_CanPaste()
{
	FString ClipboardText;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardText);

	FString Class;
	FString PossibleObjectPath = ClipboardText;
	if( ClipboardText.Split( TEXT("'"), &Class, &PossibleObjectPath, ESearchCase::CaseSensitive) )
	{
		// Remove the last item
		PossibleObjectPath.LeftChopInline( 1, false );
	}

	bool bCanPaste = false;

	if( PossibleObjectPath == TEXT("None") )
	{
		bCanPaste = true;
	}
	else
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		bCanPaste = PossibleObjectPath.Len() < NAME_SIZE && AssetRegistryModule.Get().GetAssetByObjectPath( *PossibleObjectPath ).IsValid();
	}

	return bCanPaste;
}

void SSelectActorDialog::_OnClear()
{
	_OnActorSelected(nullptr);
}

void SSelectActorDialog::_OnActorSelected(AActor* InActor)
{
	CurrentActor = InActor;
	FSlateApplication::Get().DestroyWindowImmediately(StaticCastSharedRef<SWindow>(this->AsShared()));
}

#undef LOCTEXT_NAMESPACE