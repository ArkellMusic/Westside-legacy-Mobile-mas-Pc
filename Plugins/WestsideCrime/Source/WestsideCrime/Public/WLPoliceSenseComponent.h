#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WLPoliceSenseComponent.generated.h"

/**
 * Va en cada agente. Cada SenseInterval segundos comprueba si ve al jugador (distancia y
 * linea de vision) y, si esta dentro del alcance de reconocimiento del UWLCrimeSubsystem,
 * avisa a toda la patrulla. La IA (Behavior Tree o Blueprint) solo tiene que leer:
 *   - ShouldChase(): perseguir / disparar al jugador.
 *   - GetSearchLocation(): si no, donde buscar.
 */
UCLASS(ClassGroup = (Westside), meta = (BlueprintSpawnableComponent))
class WESTSIDECRIME_API UWLPoliceSenseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWLPoliceSenseComponent();

	/** Distancia maxima (cm) a la que el agente puede ver algo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime")
	float SightRange = 4800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime")
	float SenseInterval = 0.3f;

	/** Altura de los ojos sobre el origen del actor (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime")
	float EyeHeight = 70.f;

	/** Radio (cm) de busqueda alrededor del ultimo lugar conocido. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime")
	float SearchRadius = 1600.f;

	/** Ve al jugador ahora (aunque no lo haya reconocido). Usalo como bSeenByPolice en CommitCrime. */
	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	bool SeesSuspect() const { return bSeesSuspect; }

	/** La patrulla sabe donde esta el jugador: perseguir. */
	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	bool ShouldChase() const;

	/** Punto al que ir cuando no se sabe donde esta (cambia cada pocos segundos). */
	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	FVector GetSearchLocation() const;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool bSeesSuspect = false;
	float SenseTimer = 0.f;
	float SearchTimer = 0.f;
	FVector SearchOffset = FVector::ZeroVector;
};
