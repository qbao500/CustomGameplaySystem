// Fill out your copyright notice in the Description page of Project Settings.


#include "Feedback/NumberPopUp/CustomNumberPopComponent_Niagara.h"

#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomNumberPopComponent_Niagara)

void UCustomNumberPopComponent_Niagara::AddNumberPop(const FNumberPopRequest& NewRequest)
{
	if (!NumberNiagaraSystem) return;

	const int32 DisplayNumber = NewRequest.NumberToDisplay;
	
	// Add a NiagaraComponent if we don't already have one
	if (!NumberNiagaraComp)
	{
		NumberNiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
		
		NumberNiagaraComp->SetAsset(NumberNiagaraSystem);
		NumberNiagaraComp->bAutoActivate = false;
		
		NumberNiagaraComp->SetupAttachment(nullptr);
		
		check(NumberNiagaraComp);
		NumberNiagaraComp->RegisterComponent();
	}
	
	NumberNiagaraComp->Activate(false);
	NumberNiagaraComp->SetWorldLocation(NewRequest.WorldLocation);
	
	// Add Damage information to the current Niagara list - Number information are packed inside a FVector4 where XYZ = Position, W = Damage
	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(NumberNiagaraComp, NiagaraParameterName);
	DamageList.Emplace(FVector4(NewRequest.WorldLocation.X, NewRequest.WorldLocation.Y, NewRequest.WorldLocation.Z, DisplayNumber));
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NumberNiagaraComp, NiagaraParameterName, DamageList);
}
