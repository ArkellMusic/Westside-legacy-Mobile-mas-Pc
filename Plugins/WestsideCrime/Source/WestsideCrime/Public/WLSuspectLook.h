#pragma once

#include "CoreMinimal.h"
#include "WLSuspectLook.generated.h"

/** Aspecto del jugador tal y como lo describiria un testigo: ropa y, si va en coche, el vehiculo. */
USTRUCT(BlueprintType)
struct WESTSIDECRIME_API FWLSuspectLook
{
	GENERATED_BODY()

	/** Indice del conjunto de ropa (el mismo orden que en la version movil). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime")
	int32 OutfitId = 0;

	/** Identificador unico del vehiculo que conduce (matricula). NAME_None = a pie. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime")
	FName VehicleId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime")
	FLinearColor VehicleColor = FLinearColor::Black;

	/** Texto para el HUD ("Sudadera negra", "Veyra Sedan"...). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime")
	FText OutfitName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime")
	FText VehicleName;

	bool InVehicle() const { return !VehicleId.IsNone(); }

	/** El aspecto actual (this) encaja con la descripcion policial? */
	bool MatchesDescription(const FWLSuspectLook& Description) const
	{
		if (InVehicle())
		{
			return VehicleId == Description.VehicleId && VehicleColor.Equals(Description.VehicleColor, 0.02f);
		}
		return OutfitId == Description.OutfitId;
	}
};

/** Un delito pendiente de denuncia. */
USTRUCT(BlueprintType)
struct WESTSIDECRIME_API FWLCrimeReport
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Westside|Crime")
	int32 CrimeId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Westside|Crime")
	FWLSuspectLook Look;

	UPROPERTY(BlueprintReadOnly, Category = "Westside|Crime")
	FVector Location = FVector::ZeroVector;

	/** Estrellas que suma cuando se denuncia. */
	UPROPERTY(BlueprintReadOnly, Category = "Westside|Crime")
	int32 Severity = 1;
};
