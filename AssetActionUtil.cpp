// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAssetActionUtil.h"

#include "AssetToolsModule.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"


void UMyAssetActionUtil::AutoCheck()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().OnAssetAdded().AddUObject(this, &UMyAssetActionUtil::OnAssetAdded);
}

void UMyAssetActionUtil::OnAssetAdded(const FAssetData& AssetData)
{
	DebugHeader::ShowNotifyInfo(TEXT("New Asset detected"));
}

void UMyAssetActionUtil::AssetsToDuplicate(int32 NumOfDuplicates)
{
	if(NumOfDuplicates<=0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please enter a VALID number"));
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;

	for(const FAssetData& SelectedAssetData:SelectedAssetsData)
	{
		for(int32 i = 0; i<NumOfDuplicates; i++)
		{
			const FString SourceAssetPath = SelectedAssetData.GetObjectPathString();
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i+1);
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(),NewDuplicatedAssetName);

			if(UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{	
				UEditorAssetLibrary::SaveAsset(NewPathName,false);
				++Counter;
			}
		}
	}

	if(Counter>0)
	{	
		DebugHeader::ShowNotifyInfo(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " files"));
	}
}

void UMyAssetActionUtil::AddPrefixes()
{
	TArray<UObject*>SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;

	for(UObject* SelectedObject:SelectedObjects)
	{
		if(!SelectedObject) continue;

		FString* PrefixFound = PrefixMap.Find(SelectedObject->GetClass());
		if(!PrefixFound||PrefixFound->IsEmpty())
		{
			DebugHeader::Print(TEXT("Failed to find prefix for class ") + SelectedObject->GetClass()->GetName(),FColor::Red);
			continue;
		}
		FString OldName = SelectedObject->GetName();

		if(OldName.StartsWith(*PrefixFound))
		{
			DebugHeader::ShowNotifyInfo(TEXT("already has prefix added"));
			continue;
		}
		if(SelectedObject->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart(TEXT("M_"));
			OldName.RemoveFromEnd(TEXT("_Inst"));
		}
		const FString NewNameWithPrefix = *PrefixFound + OldName;

		UEditorUtilityLibrary::RenameAsset(SelectedObject,NewNameWithPrefix);

		++Counter;
	}

	if(Counter>0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully renamed "+FString::FromInt(Counter)+" assets"));
	}
}

void UMyAssetActionUtil::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;
	FixUpRedirectors();
	for(const FAssetData& SelectedAssetData:SelectedAssetsData)
	{	
		TArray<FString> AssetReferencers =
		UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.GetObjectPathString());

		if(AssetReferencers.Num()==0)
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}
	if(UnusedAssetsData.Num()==0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("No unused asset found among selected assets"),false);
		return;
	}
	const int32 NumOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData);
	if(NumOfAssetsDeleted == 0) return;

	DebugHeader::ShowNotifyInfo(TEXT("Successfully deleted " + FString::FromInt(NumOfAssetsDeleted) + TEXT(" unused assets")));
}



void UMyAssetActionUtil::FixUpRedirectors()
{
	const FAssetRegistryModule& AssetRegistryModule =
	FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Emplace("ObjectRedirector");

	TArray<FAssetData> OutRedirectors;

	AssetRegistryModule.Get().GetAssets(Filter,OutRedirectors);
	
	TArray<UObjectRedirector*> RedirectorsToFixArray;

	for(const FAssetData& RedirectorData:OutRedirectors)
	{
		if(UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	const FAssetToolsModule& AssetToolsModule =
	FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}
