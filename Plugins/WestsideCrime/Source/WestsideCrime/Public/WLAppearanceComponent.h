#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WLSuspectLook.h"
#include "WLAppearanceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWLOnLookChanged, const FWLSuspectLook&, NewLook);

/**
 * Va en el personaje del jugador. Guarda que ropa lleva y que vehiculo conduce.
 * Llama a SetOutfit al cambiarse de ropa, a EnterVehicle / ExitVehicle al subir o bajar de un
 * coche y a SetVehicleColor al repintarlo.
 */
UCLASS(ClassGroup = (Westside), meta = (BlueprintSpawnableComponent))
class WESTSIDECRIME_API UWLAppearanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWLAppearanceComponent();

	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	void SetOutfit(int32 OutfitId, FText OutfitName);

	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	void EnterVehicle(FName VehicleId, FLinearColor Color, FText VehicleName);

	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	void ExitVehicle();

	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	void SetVehicleColor(FLinearColor Color);

	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	const FWLSuspectLook& GetLook() const { return Look; }

	/** Se emite al cambiar de ropa, de vehiculo o de color (actualiza la malla o el HUD aqui). */
	UPROPERTY(BlueprintAssignable, Category = "Westside|Crime")
	FWLOnLookChanged OnLookChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Westside|Crime")
	FWLSuspectLook Look;

	void Broadcast();
};
