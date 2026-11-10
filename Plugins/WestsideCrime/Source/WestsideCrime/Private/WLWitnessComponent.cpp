#include "WLWitnessComponent.h"
#include "WLCrimeSubsystem.h"
#include "Engine/World.h"

UWLWitnessComponent::UWLWitnessComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UWLWitnessComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		if (UWLCrimeSubsystem* Crime = World->GetSubsystem<UWLCrimeSubsystem>())
		{
			Crime->RegisterWitness(this);
		}
	}
}

void UWLWitnessComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UWLCrimeSubsystem* Crime = World->GetSubsystem<UWLCrimeSubsystem>())
		{
			Crime->UnregisterWitness(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void UWLWitnessComponent::StartCall(const FWLCrimeReport& Report, float Duration)
{
	Pending = Report;
	TimeLeft = FMath::Max(0.1f, Duration);
	SetComponentTickEnabled(true);
	OnCallStarted.Broadcast();
}

void UWLWitnessComponent::CancelCall()
{
	if (!IsCalling())
	{
		return;
	}
	TimeLeft = 0.f;
	SetComponentTickEnabled(false);
	OnCallCancelled.Broadcast();
}

void UWLWitnessComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TimeLeft <= 0.f)
	{
		return;
	}
	TimeLeft -= DeltaTime;
	if (TimeLeft > 0.f)
	{
		return;
	}
	TimeLeft = 0.f;
	SetComponentTickEnabled(false);
	if (UWorld* World = GetWorld())
	{
		if (UWLCrimeSubsystem* Crime = World->GetSubsystem<UWLCrimeSubsystem>())
		{
			Crime->ReportCrime(Pending);
		}
	}
	OnCallFinished.Broadcast();
}
