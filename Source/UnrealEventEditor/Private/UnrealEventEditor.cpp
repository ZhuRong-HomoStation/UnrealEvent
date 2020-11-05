#include "UnrealEventEditor.h"
#include "DetailViews/EventBinderDetailView.h"

#define LOCTEXT_NAMESPACE "FUnrealEventEditorModule"

void FUnrealEventEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(
        FName("EventBinder"),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&EventBinderDetailView::MakeInstance));
}

void FUnrealEventEditorModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.UnregisterCustomPropertyTypeLayout(FName("EventBinder"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealEventEditorModule, UnrealEventEditor)