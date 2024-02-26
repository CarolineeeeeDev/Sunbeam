// Copyright Project SunBeam. All Rights Reserved.


#include "Actor/BeamFlowerBase.h"

#include "ActorComponent/BeamEnergyStorageComponent.h"
#include "Beam/BeamActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BeamPawn.h"

// Sets default values
ABeamFlowerBase::ABeamFlowerBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BeamEnergyStorageComponent = CreateDefaultSubobject<UBeamEnergyStorageComponent>(TEXT("BeamEnergyStorageComponent"));
	BeamEnergyStorageComponent->MaxEnergy = EnergyValue;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(150.0f);
}

void ABeamFlowerBase::GetInteractableTags_Implementation(FGameplayTagContainer& OutTagContainer)
{
	OutTagContainer = InteractableTags;
}

void ABeamFlowerBase::OnBeginInteract_Implementation(FHitResult BeamHitResult, const ABeamActor* BeamActor)
{
	CurBeamSourceTag = BeamActor->GetBeamSourceTag();
	if (!CanInteractWithBeam()) return;
	bIsChangingStatus = true;
	bHasBloomed ? PlayBloomAnimReverse() : PlayBloomAnimForward();
}

void ABeamFlowerBase::OnEndInteract_Implementation()
{
	if (!bIsChangingStatus) return;
	bHasBloomed ? PlayBloomAnimForward() : PlayBloomAnimReverse();
}

// Called when the game starts or when spawned
void ABeamFlowerBase::BeginPlay()
{
	Super::BeginPlay();
}

// When the bloom animation reaches the end, the flower is in bloom. When the bloom animation was played in reverse and reaches the start, the flower is not in bloom.
void ABeamFlowerBase::SetBloomStatus(const bool bBloomed)
{
	bHasBloomed = bBloomed;
	bIsChangingStatus = false;

	if (bHasBloomed == bLastBloomStatus)
	{
		return;
	}
	bLastBloomStatus = bHasBloomed;

	const ABeamPawn* BeamPawn = Cast<ABeamPawn>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());

	if (!IsValid(BeamPawn))
	{
		return;
	}

	const bool bSuccess = bHasBloomed ? ConsumeEnergyFromPlayer() : AddEnergyToPlayer();

	// Energy was checked before transfer, assert here to help capture boundary cases
	check(bSuccess);
}

bool ABeamFlowerBase::ConsumeEnergyFromPlayer() const
{
	const ABeamPawn* PlayerPawn = Cast<ABeamPawn>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
	UBeamEnergyStorageComponent* PlayerEnergyStorageComponent = PlayerPawn->GetBeamEnergyStorageComponent();
	if (!IsValid(PlayerEnergyStorageComponent) || !IsValid(BeamEnergyStorageComponent))
	{
		return false;
	}

	bool bSuccess = PlayerEnergyStorageComponent->ConsumeEnergy(BeamEnergyStorageComponent->GetMaxEnergy(), CurBeamSourceTag);

	// For flowers, energy is not stored per tag, so we need to add energy for all interactable tags the flower has
	for (auto& InteractableTag : InteractableTags)
	{
		bSuccess &= BeamEnergyStorageComponent->AddEnergy(BeamEnergyStorageComponent->GetMaxEnergy(), InteractableTag);
	}
	
	return bSuccess;
}

bool ABeamFlowerBase::AddEnergyToPlayer() const
{
	const ABeamPawn* PlayerPawn = Cast<ABeamPawn>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
	UBeamEnergyStorageComponent* PlayerEnergyStorageComponent = PlayerPawn->GetBeamEnergyStorageComponent();
	if (!IsValid(PlayerEnergyStorageComponent) || !IsValid(BeamEnergyStorageComponent))
	{
		return false;
	}

	bool bSuccess = PlayerEnergyStorageComponent->AddEnergy(BeamEnergyStorageComponent->GetMaxEnergy(), CurBeamSourceTag);

	// For flowers, energy is not stored per tag, so we need to consume energy for all interactable tags the flower has
	for (auto& InteractableTag : InteractableTags)
	{
		bSuccess &= BeamEnergyStorageComponent->ConsumeEnergy(BeamEnergyStorageComponent->GetMaxEnergy(), InteractableTag);
	}
	
	return bSuccess;
}

// Check if the flower can interact with the beam from the energy perspective
bool ABeamFlowerBase::CanInteractWithBeam() const
{
	const ABeamPawn* PlayerPawn = Cast<ABeamPawn>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
	const UBeamEnergyStorageComponent* PlayerEnergyStorageComponent = PlayerPawn->GetBeamEnergyStorageComponent();
	if (!IsValid(PlayerEnergyStorageComponent) || !IsValid(BeamEnergyStorageComponent))
	{
		return false;
	}

	if (bHasBloomed)
	{
		// About to add energy to the player and consume energy from the flower
		return BeamEnergyStorageComponent->GetCurEnergy(CurBeamSourceTag) > 0 && PlayerEnergyStorageComponent->GetCurEnergy(CurBeamSourceTag) + BeamEnergyStorageComponent->GetCurEnergy(CurBeamSourceTag) < PlayerEnergyStorageComponent->GetMaxEnergy();
	}
	else
	{
		// About to consume energy from the player and add energy to the flower
		return BeamEnergyStorageComponent->GetCurEnergy(CurBeamSourceTag) < BeamEnergyStorageComponent->GetMaxEnergy() && PlayerEnergyStorageComponent->GetCurEnergy(CurBeamSourceTag) > BeamEnergyStorageComponent->GetMaxEnergy();
	}
}

