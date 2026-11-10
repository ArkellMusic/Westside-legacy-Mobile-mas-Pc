#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WLSuspectLook.h"
#include "WLWitnessComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWLOnWitnessCall);

/**
 * Va en cada peaton. Cuando presencia un delito saca el movil y llama durante unos segundos;
 * si termina, da el aviso con la descripcion del jugador. Llama a CancelCall() cuando el
 * peaton muere, cae al suelo o huye para que no llegue a denunciar.
 */
UCLASS(ClassGroup = (Westside), meta = (BlueprintSpawnableComponent))
class WESTSIDECRIME_API UWLWitnessComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWLWitnessComponent();

	/** Ponlo a false en peatones que no deben denunciar (pandilleros, complices...). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime")
	bool bWillReport = true;

	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	bool IsCalling() const { return TimeLeft > 0.f; }

	bool CanWitness() const { return bWillReport && !IsCalling(); }

	void StartCall(const FWLCrimeReport& Report, float Duration);

	/** Interrumpe la llamada (testigo silenciado). */
	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	void CancelCall();

	/** Empieza a llamar: reproduce aqui la animacion del movil y deten al peaton. */
	UPROPERTY(BlueprintAssignable, Category = "Westside|Crime")
	FWLOnWitnessCall OnCallStarted;
	/** Ha colgado y ha dado el aviso: lo normal es que huya. */
	UPROPERTY(BlueprintAssignable, Category = "Westside|Crime")
	FWLOnWitnessCall OnCallFinished;
	UPROPERTY(BlueprintAssignable, Category = "Westside|Crime")
	FWLOnWitnessCall OnCallCancelled;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FWLCrimeReport Pending;
	float TimeLeft = 0.f;
};
