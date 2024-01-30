// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Editor/Blutility/Classes/AssetActionUtility.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "MyAssetActionUtil.generated.h"


UCLASS()
class CPP_EXAMPLE_API UMyAssetActionUtil : public UAssetActionUtility
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(CallInEditor)
	void AssetsToDuplicate(int32 NumOfDuplicates);
	
	UFUNCTION(CallInEditor)
	void AddPrefixes();

	UFUNCTION(CallInEditor)
	void RemoveUnusedAssets();

	UFUNCTION(CallInEditor)
	void AutoCheck();

	void OnAssetAdded(const FAssetData& AssetData);

private:
	//Map data for the classes & their prefixes
	//Can't be done in Blyueprint due to the Map taking UClass type
	//which can't be accessed in BP
	TMap<UClass*,FString>PrefixMap = 
	{
		{UBlueprint::StaticClass(),TEXT("BP_")},
		{UStaticMesh::StaticClass(),TEXT("SM_")},
		{UMaterial::StaticClass(), TEXT("M_")},
		{UMaterialInstanceConstant::StaticClass(),TEXT("MI_")},
		{UMaterialFunctionInterface::StaticClass(), TEXT("MF_")},
		{UParticleSystem::StaticClass(), TEXT("PS_")},
		{USoundCue::StaticClass(), TEXT("SC_")},
		{USoundWave::StaticClass(), TEXT("SW_")},
		{UTexture::StaticClass(), TEXT("T_")},
		{UTexture2D::StaticClass(), TEXT("T_")},
		{UUserWidget::StaticClass(), TEXT("WBP_")},
		{USkeletalMeshComponent::StaticClass(), TEXT("SK_")},
		{UNiagaraSystem::StaticClass(), TEXT("NS_")},
		{UNiagaraEmitter::StaticClass(), TEXT("NE_")}
	};

	void FixUpRedirectors();
};
