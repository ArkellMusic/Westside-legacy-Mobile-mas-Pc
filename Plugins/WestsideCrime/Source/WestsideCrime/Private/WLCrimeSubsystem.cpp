#include "WLCrimeSubsystem.h"
#include "WLAppearanceComponent.h"
#include "WLWitnessComponent.h"
#include "GameFramework/Actor.h"

void UWLCrimeSubsystem::SetSuspect(UWLAppearanceComponent* Appearance)
{
	Suspect = Appearance;
}

FWLSuspectLook UWLCrimeSubsystem::GetCurrentLook() const
{
	return Suspect.IsValid() ? Suspect->GetLook() : FWLSuspectLook();
}

bool UWLCrimeSubsystem::SuspectMatchesDescription() const
{
	return bHasDescription && GetCurrentLook().MatchesDescription(Description);
}

float UWLCrimeSubsystem::GetRecognitionRange() const
{
	const bool bInVehicle = GetCurrentLook().InVehicle();
	if (SuspectMatchesDescription())
	{
		return bInVehicle ? RangeMatchInVehicle : RangeMatchOnFoot;
	}
	return bInVehicle ? RangeNoMatchInVehicle : RangeNoMatchOnFoot;
}

int32 UWLCrimeSubsystem::CommitCrime(FVector Location, int32 Severity, bool bLoud, bool bSeenByPolice)
{
	if (bLoud)
	{
		// Una rafaga de disparos cuenta como un solo delito.
		if (LoudCooldown > 0.f)
		{
			return 0;
		}
		LoudCooldown = 10.f;
	}

	FWLCrimeReport Report;
	Report.CrimeId = NextCrimeId++;
	Report.Look = GetCurrentLook();
	Report.Location = Location;
	Report.Severity = FMath::Max(1, Severity);

	if (bSeenByPolice)
	{
		ReportCrime(Report);
		return 0;
	}

	// Los testigos libres mas cercanos empiezan a llamar.
	const float Radius = bLoud ? WitnessRadiusLoud : WitnessRadius;
	TArray<UWLWitnessComponent*> Candidates;
	for (const TWeakObjectPtr<UWLWitnessComponent>& Weak : Witnesses)
	{
		UWLWitnessComponent* W = Weak.Get();
		if (W && W->CanWitness() && W->GetOwner() &&
			FVector::Dist(W->GetOwner()->GetActorLocation(), Location) <= Radius)
		{
			Candidates.Add(W);
		}
	}
	Candidates.Sort([Location](const UWLWitnessComponent& A, const UWLWitnessComponent& B)
	{
		return FVector::DistSquared(A.GetOwner()->GetActorLocation(), Location) <
			FVector::DistSquared(B.GetOwner()->GetActorLocation(), Location);
	});

	int32 Started = 0;
	for (UWLWitnessComponent* W : Candidates)
	{
		if (Started >= MaxWitnessesPerCrime)
		{
			break;
		}
		W->StartCall(Report, WitnessCallTime + 0.8f * Started);
		++Started;
	}
	return Started;
}

void UWLCrimeSubsystem::ReportCrime(const FWLCrimeReport& Report)
{
	Description = Report.Look;
	bHasDescription = true;
	LastKnownLocation = Report.Location;
	OnDescriptionChanged.Broadcast(Description);

	if (!ReportedCrimes.Contains(Report.CrimeId))
	{
		ReportedCrimes.Add(Report.CrimeId);
		SetWantedLevel(WantedLevel + Report.Severity);
	}
	else
	{
		WantedTimer = WantedCooldown;
	}
}

void UWLCrimeSubsystem::NotifyRecognised(FVector Location)
{
	if (WantedLevel <= 0)
	{
		return;
	}
	LastKnownLocation = Location;
	KnownTime = KnownMemory;
	WantedTimer = FMath::Max(WantedTimer, WantedCooldown);
	// Lo estan viendo: la descripcion pasa a ser lo que lleva ahora.
	Description = GetCurrentLook();
	bHasDescription = true;
	if (!bWasKnown)
	{
		bWasKnown = true;
		OnDescriptionChanged.Broadcast(Description);
		OnKnownChanged.Broadcast(true);
	}
}

void UWLCrimeSubsystem::NotifyLookChanged()
{
	if (WantedLevel > 0 && !IsSuspectKnown() && bHasDescription && !SuspectMatchesDescription())
	{
		OnNoLongerMatches.Broadcast();
	}
}

void UWLCrimeSubsystem::SetWantedLevel(int32 NewLevel)
{
	NewLevel = FMath::Clamp(NewLevel, 0, MaxWantedLevel);
	WantedTimer = WantedCooldown;
	if (NewLevel == WantedLevel)
	{
		return;
	}
	WantedLevel = NewLevel;
	if (WantedLevel == 0)
	{
		bHasDescription = false;
		ReportedCrimes.Reset();
		KnownTime = 0.f;
		LoudCooldown = 0.f;
		if (bWasKnown)
		{
			bWasKnown = false;
			OnKnownChanged.Broadcast(false);
		}
	}
	OnWantedChanged.Broadcast(WantedLevel);
}

void UWLCrimeSubsystem::ClearHeat()
{
	for (const TWeakObjectPtr<UWLWitnessComponent>& Weak : Witnesses)
	{
		if (UWLWitnessComponent* W = Weak.Get())
		{
			W->CancelCall();
		}
	}
	SetWantedLevel(0);
}

void UWLCrimeSubsystem::RegisterWitness(UWLWitnessComponent* Witness)
{
	Witnesses.AddUnique(Witness);
}

void UWLCrimeSubsystem::UnregisterWitness(UWLWitnessComponent* Witness)
{
	Witnesses.Remove(Witness);
}

void UWLCrimeSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LoudCooldown = FMath::Max(0.f, LoudCooldown - DeltaTime);

	if (KnownTime > 0.f)
	{
		KnownTime -= DeltaTime;
		if (KnownTime <= 0.f && bWasKnown)
		{
			bWasKnown = false;
			OnKnownChanged.Broadcast(false);
		}
	}

	// Las estrellas solo bajan mientras nadie reconoce al jugador.
	if (WantedLevel > 0 && !IsSuspectKnown())
	{
		WantedTimer -= DeltaTime;
		if (WantedTimer <= 0.f)
		{
			SetWantedLevel(WantedLevel - 1);
		}
	}
}

TStatId UWLCrimeSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UWLCrimeSubsystem, STATGROUP_Tickables);
}
