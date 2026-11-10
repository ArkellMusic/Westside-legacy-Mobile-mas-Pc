#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WLSuspectLook.h"
#include "WLCrimeSubsystem.generated.h"

class UWLWitnessComponent;
class UWLAppearanceComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWLOnWantedChanged, int32, WantedLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWLOnDescriptionChanged, const FWLSuspectLook&, Description);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWLOnKnownChanged, bool, bPoliceKnowsWhereYouAre);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWLOnNoLongerMatches);

/**
 * Delitos, testigos y descripcion policial. Misma logica que scripts/world/crime.gd de la
 * version movil (Godot):
 *  - Si un agente ve el delito, identifica al jugador al momento.
 *  - Si no, los testigos cercanos llaman; si se les interrumpe antes de colgar no hay aviso.
 *  - El aviso lleva la ropa y el vehiculo. Los agentes buscan en el ultimo lugar conocido y
 *    reconocen de lejos a quien encaja y solo de muy cerca a quien se ha cambiado.
 *  - Las estrellas bajan de una en una mientras ningun agente reconoce al jugador.
 */
UCLASS()
class WESTSIDECRIME_API UWLCrimeSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// --- Ajustes (distancias en centimetros) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	float WitnessCallTime = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	float WitnessRadius = 2400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	float WitnessRadiusLoud = 4200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	int32 MaxWitnessesPerCrime = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	float RangeMatchOnFoot = 3400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	float RangeNoMatchOnFoot = 700.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	float RangeMatchInVehicle = 4200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	float RangeNoMatchInVehicle = 900.f;
	/** Segundos sin ser reconocido para perder una estrella. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	float WantedCooldown = 18.f;
	/** Segundos que la patrulla sigue sabiendo donde estas tras el ultimo avistamiento. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	float KnownMemory = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Westside|Crime|Tuning")
	int32 MaxWantedLevel = 6;

	// --- API ---
	/** Registra el componente de aspecto del jugador (lo hace solo el componente? no: llamalo en BeginPlay del personaje). */
	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	void SetSuspect(UWLAppearanceComponent* Appearance);

	/**
	 * El jugador comete un delito. bSeenByPolice = un agente lo esta viendo (denuncia inmediata).
	 * Devuelve cuantos testigos han empezado a llamar.
	 */
	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	int32 CommitCrime(FVector Location, int32 Severity = 1, bool bLoud = false, bool bSeenByPolice = false);

	/** Un testigo (o un agente) da el aviso. Lo llaman los UWLWitnessComponent al colgar. */
	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	void ReportCrime(const FWLCrimeReport& Report);

	/** Un agente reconoce al jugador en Location (lo llama UWLPoliceSenseComponent). */
	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	void NotifyRecognised(FVector Location);

	/** El jugador ha cambiado de ropa, de coche o de color. */
	void NotifyLookChanged();

	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	void SetWantedLevel(int32 NewLevel);

	/** Detencion, hospital o nueva partida: sin estrellas ni descripcion. */
	UFUNCTION(BlueprintCallable, Category = "Westside|Crime")
	void ClearHeat();

	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	int32 GetWantedLevel() const { return WantedLevel; }

	/** La policia tiene una descripcion y el jugador encaja con ella ahora? */
	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	bool SuspectMatchesDescription() const;

	/** Distancia (cm) a la que un agente con linea de vision reconoce al jugador. */
	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	float GetRecognitionRange() const;

	/** true = algun agente sabe donde esta el jugador (persecucion); false = lo buscan. */
	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	bool IsSuspectKnown() const { return KnownTime > 0.f && WantedLevel > 0; }

	/** Donde deben buscar los agentes que no ven al jugador. */
	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	FVector GetLastKnownLocation() const { return LastKnownLocation; }

	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	bool HasDescription() const { return bHasDescription; }

	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	const FWLSuspectLook& GetDescription() const { return Description; }

	UFUNCTION(BlueprintPure, Category = "Westside|Crime")
	FWLSuspectLook GetCurrentLook() const;

	void RegisterWitness(UWLWitnessComponent* Witness);
	void UnregisterWitness(UWLWitnessComponent* Witness);

	// --- Eventos para HUD e IA ---
	UPROPERTY(BlueprintAssignable, Category = "Westside|Crime")
	FWLOnWantedChanged OnWantedChanged;
	UPROPERTY(BlueprintAssignable, Category = "Westside|Crime")
	FWLOnDescriptionChanged OnDescriptionChanged;
	UPROPERTY(BlueprintAssignable, Category = "Westside|Crime")
	FWLOnKnownChanged OnKnownChanged;
	/** El jugador se ha cambiado y ya no encaja con la descripcion. */
	UPROPERTY(BlueprintAssignable, Category = "Westside|Crime")
	FWLOnNoLongerMatches OnNoLongerMatches;

	// UTickableWorldSubsystem
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

private:
	UPROPERTY()
	TWeakObjectPtr<UWLAppearanceComponent> Suspect;

	UPROPERTY()
	TArray<TWeakObjectPtr<UWLWitnessComponent>> Witnesses;

	FWLSuspectLook Description;
	bool bHasDescription = false;
	FVector LastKnownLocation = FVector::ZeroVector;
	int32 WantedLevel = 0;
	float WantedTimer = 0.f;
	float KnownTime = 0.f;
	float LoudCooldown = 0.f;
	bool bWasKnown = false;
	int32 NextCrimeId = 1;
	TSet<int32> ReportedCrimes;
};
