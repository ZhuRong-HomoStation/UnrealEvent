#include "UnrealEventEditor.h"

#include "EventBinder.h"
#include "DetailViews/EventBinderDetailView.h"

#define LOCTEXT_NAMESPACE "FUnrealEventEditorModule"

void FUnrealEventEditorModule::StartupModule()
{
	if (!AActor::StaticClass()->FindPropertyByName(TEXT("EventBinderDummy")))
	{
		FStructProperty* DummyProperty = new FStructProperty(AActor::StaticClass()
			, TEXT("EventBinderDummy"), EObjectFlags::RF_NoFlags);
		DummyProperty->Struct = FEventBinderDummy::StaticStruct();
		DummyProperty->PropertyFlags |= EPropertyFlags::CPF_Transient;
		DummyProperty->PropertyFlags |= EPropertyFlags::CPF_EditConst;
		DummyProperty->PropertyFlags |= EPropertyFlags::CPF_Edit;
		DummyProperty->SetMetaData(TEXT("Category"), TEXT("EventBinder"));
		AActor::StaticClass()->AddCppProperty(DummyProperty);
	}
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(
        TEXT("EventBinderDummy"),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&EventBinderDetailView::MakeInstance));

	PropertyEditorModule.NotifyCustomizationModuleChanged();
}

void FUnrealEventEditorModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.UnregisterCustomPropertyTypeLayout(FName("EventBinder"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealEventEditorModule, UnrealEventEditor)