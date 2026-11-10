#include "WLAppearanceComponent.h"
#include "WLCrimeSubsystem.h"
#include "Engine/World.h"

UWLAppearanceComponent::UWLAppearanceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWLAppearanceComponent::SetOutfit(int32 OutfitId, FText OutfitName)
{
	Look.OutfitId = OutfitId;
	Look.OutfitName = OutfitName;
	Broadcast();
}

void UWLAppearanceComponent::EnterVehicle(FName VehicleId, FLinearColor Color, FText VehicleName)
{
	Look.VehicleId = VehicleId;
	Look.VehicleColor = Color;
	Look.VehicleName = VehicleName;
	Broadcast();
}

void UWLAppearanceComponent::ExitVehicle()
{
	Look.VehicleId = NAME_None;
	Look.VehicleName = FText::GetEmpty();
	Broadcast();
}

void UWLAppearanceComponent::SetVehicleColor(FLinearColor Color)
{
	Look.VehicleColor = Color;
	Broadcast();
}

void UWLAppearanceComponent::Broadcast()
{
	OnLookChanged.Broadcast(Look);
	if (UWorld* World = GetWorld())
	{
		if (UWLCrimeSubsystem* Crime = World->GetSubsystem<UWLCrimeSubsystem>())
		{
			Crime->NotifyLookChanged();
		}
	}
}
