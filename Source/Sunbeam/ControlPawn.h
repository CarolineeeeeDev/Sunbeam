// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ControlPawn.generated.h"

class UInputMappingContext;
class UInputComponent;
class UInputAction;
struct FInputActionValue;

UCLASS()
class SUNBEAM_API AControlPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AControlPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _X = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _Y = 0; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _Pitch = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _Yaw = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _Roll = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseHardware = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SelectedHardware = 0;         //0:JoyCon, 1:Gyro

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPPSettings|Input Setting")
	UInputMappingContext* MappingContext;

	UFUNCTION(BlueprintImplementableEvent, Category = "CPPSettings|CatchBlueprintFunction")
	void BPReadDate();


private:
	//Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPPSettings|Input Setting", meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPPSettings|Input Setting", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPPSettings|Input Setting", meta = (AllowPrivateAccess = "true"))
	UInputAction* HardwareSelectAction;

	void RotateWithEnhancedInput(const FInputActionValue& Value);
	void Switch();
	void HardwareSelect(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void RotateWithHardware_JoyCon();
	UFUNCTION(BlueprintCallable)
	void RotateWithHardware_Gyro();

	class ADirectionalLight* SunLight;

	FTimerHandle RotateTimeHandle;

};
