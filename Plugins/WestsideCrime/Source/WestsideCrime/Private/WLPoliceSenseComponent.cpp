#include "WLPoliceSenseComponent.h"
#include "WLCrimeSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

UWLPoliceSenseComponent::UWLPoliceSenseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UWLPoliceSenseComponent::ShouldChase() const
{
	const UWorld* World = GetWorld();
	const UWLCrimeSubsystem* Crime = World ? World->GetSubsystem<UWLCrimeSubsystem>() : nullptr;
	return Crime && Crime->IsSuspectKnown();
}

FVector UWLPoliceSenseComponent::GetSearchLocation() const
{
	const UWorld* World = GetWorld();
	const UWLCrimeSubsystem* Crime = World ? World->GetSubsystem<UWLCrimeSubsystem>() : nullptr;
	return (Crime ? Crime->GetLastKnownLocation() : FVector::ZeroVector) + SearchOffset;
}

void UWLPoliceSenseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Nuevo punto de busqueda cada pocos segundos.
	SearchTimer -= DeltaTime;
	if (SearchTimer <= 0.f)
	{
		SearchTimer = FMath::FRandRange(4.f, 7.f);
		const float Angle = FMath::FRandRange(0.f, 2.f * PI);
		SearchOffset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * FMath::FRandRange(300.f, SearchRadius);
	}

	SenseTimer -= DeltaTime;
	if (SenseTimer > 0.f)
	{
		return;
	}
	SenseTimer = SenseInterval;

	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	UWLCrimeSubsystem* Crime = World ? World->GetSubsystem<UWLCrimeSubsystem>() : nullptr;
	APawn* Player = World ? UGameplayStatics::GetPlayerPawn(World, 0) : nullptr;
	bSeesSuspect = false;
	if (!Crime || !Player || !Owner)
	{
		return;
	}

	const FVector Eyes = Owner->GetActorLocation() + FVector(0.f, 0.f, EyeHeight);
	const FVector Target = Player->GetActorLocation();
	const float Distance = FVector::Dist(Eyes, Target);
	if (Distance > SightRange)
	{
		return;
	}

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(WLPoliceSense), false, Owner);
	Params.AddIgnoredActor(Player);
	const bool bBlocked = World->LineTraceSingleByChannel(Hit, Eyes, Target, ECC_Visibility, Params);
	bSeesSuspect = !bBlocked;

	if (bSeesSuspect && Crime->GetWantedLevel() > 0 && Distance <= Crime->GetRecognitionRange())
	{
		Crime->NotifyRecognised(Target);
	}
}
