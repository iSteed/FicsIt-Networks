﻿#include "FINIndicatorPole.h"

#include "ProxyInstancedStaticMeshComponent.h"

AFINIndicatorPole::AFINIndicatorPole() {
	Indicator = CreateDefaultSubobject<UProxyInstancedStaticMeshComponent>("Indicator");
	Indicator->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	Connector = CreateDefaultSubobject<UFINNetworkConnector>("Connector");
	Connector->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AFINIndicatorPole::OnConstruction(const FTransform& transform) {
	Super::OnConstruction(transform);

#if WITH_EDITOR
	CreatePole();
#endif
}

void AFINIndicatorPole::BeginPlay() {
	Super::BeginPlay();

	if (Indicator->GetMaterials().Num() > 0) {
		IndicatorInstance = Indicator->CreateDynamicMaterialInstance(0);
		Indicator->SetMaterial(0, IndicatorInstance);
	}

#if !WITH_EDITOR
	CreatePole();
#endif
	
	UpdateEmessive();
}

bool AFINIndicatorPole::ShouldSave_Implementation() const {
	return true;
}

int32 AFINIndicatorPole::GetDismantleRefundReturnsMultiplier() const {
	return Height + 6;
}

void AFINIndicatorPole::CreatePole() {
	// Clean up
	Poles.Empty();

	// Construction
	for (int i = 0; i < Height; ++i) {
		UStaticMeshComponent* Pole = NewObject<UStaticMeshComponent>(this);
		check(Pole);
		Pole->AttachToComponent(Indicator, FAttachmentTransformRules::KeepRelativeTransform);
		Pole->SetRelativeLocation(FVector(0,0, -(i) * 100.0));
		Pole->RegisterComponent();
		Pole->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		Pole->SetStaticMesh(LongPole);
		Pole->SetMobility(EComponentMobility::Static);
		Poles.Add(Pole);
	}
}

void AFINIndicatorPole::UpdateEmessive() {
	if (IndicatorInstance) {
		IndicatorInstance->SetVectorParameterValue("Emissive Color", IndicatorColor);
		IndicatorInstance->SetScalarParameterValue("Emissive Strenght", EmessiveStrength);
	}
}

void AFINIndicatorPole::netFunc_setColor(float r, float g, float b, float e) {
	FLinearColor oldColor = IndicatorColor;
	float oldEmissive = EmessiveStrength;
	IndicatorColor.R = FMath::Clamp(r, 0.0f, 1.0f);
	IndicatorColor.G = FMath::Clamp(g, 0.0f, 1.0f);
	IndicatorColor.B = FMath::Clamp(b, 0.0f, 1.0f);
	EmessiveStrength = FMath::Clamp(e, 0.0f, 5.0f);
	UpdateEmessive();
	netSig_ColorChanged(oldColor.R, oldColor.G, oldColor.B, oldEmissive);
}

void AFINIndicatorPole::netFunc_getColor(float& r, float& g, float& b, float& e) {
	r = IndicatorColor.R;
	g = IndicatorColor.G;
	b = IndicatorColor.B;
	e = EmessiveStrength;
}

void AFINIndicatorPole::netSig_ColorChanged(float r, float g, float b, float e) {}

AFINIndicatorPole* AFINIndicatorPole::netFunc_getTopPole() {
	return TopConnected.Get();
}

AFINIndicatorPole* AFINIndicatorPole::netFunc_getBottomPole() {
	return BottomConnected.Get();
}