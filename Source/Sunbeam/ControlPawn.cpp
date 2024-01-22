// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"

#define _USE_MATH_DEFINES
#include <cmath>

// Sets default values
AControlPawn::AControlPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



}

// Called when the game starts or when spawned
void AControlPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}

	UClass* DirectionalLightClass = ADirectionalLight::StaticClass();
	SunLight = Cast<ADirectionalLight>(UGameplayStatics::GetActorOfClass(GetWorld(), DirectionalLightClass));

	GetWorldTimerManager().SetTimer(RotateTimeHandle, this, &AControlPawn::BPReadDate, 0.1f, true);

}

// Called every frame
void AControlPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AControlPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (InputComp) {
		InputComp->BindAction(RotateAction, ETriggerEvent::Triggered, this, &AControlPawn::RotateWithEnhancedInput);
		InputComp->BindAction(SwitchAction, ETriggerEvent::Started, this, &AControlPawn::Switch);
		InputComp->BindAction(HardwareSelectAction, ETriggerEvent::Started, this, &AControlPawn::HardwareSelect);
	}
}

void AControlPawn::RotateWithEnhancedInput(const FInputActionValue& Value) {
	FVector2D Input = Value.Get<FVector2D>();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Rotate: %f, %f"), Input.X, Input.Y));

	if (SunLight) {
		//Calculate the Angle
		FVector2D Base(0, -1);
		double Yaw = FMath::Atan2(Input.Y, Input.X) - FMath::Atan2(Base.Y, Base.X);
		Yaw = FMath::RadiansToDegrees(Yaw);
		if (Yaw <= 0) Yaw = FMath::Abs(Yaw);
		//else if (Yaw <= 90) Yaw = 360.0f - Yaw;
		else Yaw = 360.0f - Yaw;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Rotate Angle: %f"), Yaw));

		//Calculate thr Height Of Sun. (SunHeight == 0: Highest, SunHeight == 1: Lowest)
		double SunHeight = FMath::Sqrt(FMath::Square(Input.X) + FMath::Square(Input.Y));
		SunHeight = FMath::Min(SunHeight, 1.0f);
		double YRotation;
		//if (Input.Y >= 0) 
			YRotation = 270 + SunHeight * 90;
		//else YRotation = 270 + SunHeight * 90;
		FRotator rotator(YRotation, Yaw, 0);

		SunLight->SetActorRotation(rotator);
	}
}

void AControlPawn::RotateWithHardware_JoyCon() {
	//if _X > 1.f, it means can no get input from hardware
	if (_X > 1.f) return;
	
	
	if (SunLight) {
		//Calculate the Angle
		FVector2D Base(0, -1);
		double Yaw = FMath::Atan2(_Y, _X) - FMath::Atan2(Base.Y, Base.X);
		Yaw = FMath::RadiansToDegrees(Yaw);
		if (Yaw <= 0) Yaw = FMath::Abs(Yaw);
		//else if (Yaw <= 90) Yaw = 360.0f - Yaw;
		else Yaw = 360.0f - Yaw;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Rotate Angle: %f"), Yaw));

		//Calculate thr Height Of Sun. (SunHeight == 0: Highest, SunHeight == 1: Lowest)
		double SunHeight = FMath::Sqrt(FMath::Square(_X) + FMath::Square(_Y));
		SunHeight = FMath::Min(SunHeight, 1.0f);
		double YRotation;
		//if (Input.Y >= 0) 
		YRotation = 270 + SunHeight * 90;
		//else YRotation = 270 + SunHeight * 90;
		FRotator rotator(YRotation, Yaw, 0);

		SunLight->SetActorRotation(rotator);
	}
	
	
}

void AControlPawn::RotateWithHardware_Gyro() {
	//if _Yaw < -500.f, it means can no get input from hardware
	if (_Yaw < -500.f) return;

	FRotator CurRotation = SunLight->GetActorRotation();
	CurRotation.Yaw = _Yaw;
	SunLight->SetActorRotation(CurRotation);
}

void AControlPawn::Switch(){
	UseHardware = !UseHardware;
	if (UseHardware) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Hardware control open")));
	else GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Hardware control close")));
}

void AControlPawn::HardwareSelect(const FInputActionValue& Value) {
	float Input = Value.Get<float>();

	int32 Input_int = FMath::RoundToInt32(Input);

	SelectedHardware = Input_int;
	if (Input_int == 1) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Use JoyCon Hardware")));
	}
	else if (Input_int == 2) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Use Gyro Hardware")));
	}
}