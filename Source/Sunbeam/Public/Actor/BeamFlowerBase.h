// Copyright Project SunBeam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/EnergyStorage.h"
#include "Interface/Interactable.h"
#include "BeamFlowerBase.generated.h"

class USphereComponent;
class UBeamEnergyStorageComponent;

UCLASS()
class SUNBEAM_API ABeamFlowerBase : public AActor, public IInteractable, public IEnergyStorage
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeamFlowerBase();

	/* IInteractable interface */
	virtual void GetInteractableTags_Implementation(FGameplayTagContainer& OutTagContainer) override;
	virtual void OnBeginInteract_Implementation(FHitResult BeamHitResult, const ABeamActor* BeamActor) override;
	virtual void OnEndInteract_Implementation() override;
	/* End IInteractable interface */

	/* IEnergyStorage interface */
	virtual UBeamEnergyStorageComponent* GetBeamEnergyStorageComponent() const override { return BeamEnergyStorageComponent; }
	/* End IEnergyStorage interface */

	UFUNCTION(BlueprintImplementableEvent, Category = "Flower")
	void PlayBloomAnimForward();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Flower")
	void PlayBloomAnimReverse();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * @brief Call this from blueprint to set the bloom status of the flower.
	 *
	 * Add energy to the player if the flower is not in bloom, subtract energy from the player if the flower is in bloom.
	 * 
	 * @param bBloomed The bloom status of the flower. True if bloom animation reaches the end, False if bloom animation was played in reverse and reaches the start.
	 */
	UFUNCTION(BlueprintCallable, Category = "Flower")
	void SetBloomStatus(bool bBloomed);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flower")
	int32 EnergyValue = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flower")
	FGameplayTagContainer InteractableTags;

	UPROPERTY(BlueprintReadOnly, Category = "Flower")
	TObjectPtr<UBeamEnergyStorageComponent> BeamEnergyStorageComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flower")
	TObjectPtr<USphereComponent> SphereComponent;

private:
	bool ConsumeEnergyFromPlayer() const;
	bool AddEnergyToPlayer() const;
	bool CanInteractWithBeam() const;
	
	bool bHasBloomed = false;
	bool bLastBloomStatus = false;

	bool bIsChangingStatus = false;

	FGameplayTag CurBeamSourceTag;

	
};
