// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <array>

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Car.generated.h"

UCLASS()
class VEHICLES_API ACar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACar();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	UPROPERTY(EditAnywhere)
	bool bIsTurningAllowed = false;

	UPROPERTY(EditAnywhere)
	float EngineForce = 2500; // engine torque = 440 rpm
	UPROPERTY(EditAnywhere)
	float BrakeForce = 1000; // uses same torque as engine force 

	UPROPERTY(EditAnywhere)
	float Mass = 1500; // kg

	UPROPERTY(EditAnywhere)
	float CAirDrag = 0.4257f; 
	UPROPERTY(EditAnywhere)
	float CRollingResistance = 12.8f;
	UPROPERTY(EditAnywhere)
	float CCenterOfMassHeight = 5 ; // arbitrary constant added to total height of center of mass from ground while resting on ground

	UPROPERTY(EditAnywhere)
	TArray<float>Gears{2.66f, 1.78f, 1.3f, 1, 0.74f, 0.5f};

	UPROPERTY(EditAnywhere)
	float ReverseGear = 2.9f;
	UPROPERTY(EditAnywhere)
	float DifferentialRatio = 3.42f;
	UPROPERTY(EditAnywhere)
	float TransmissionRatio = 0.7f; // torque from engine is converted via gear and differential ratio where energy is lost in form of heat (around 30%) so we get 70% efficacy

	
	UPROPERTY(EditAnywhere)
	float CWheelFriction = 1;
	UPROPERTY(EditAnywhere)
	float SteerRotationLimit = 25; // in degrees
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> AWheelMesh;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> Wheels;
	
protected:
	UPROPERTY(VisibleAnywhere)
	FVector Acceleration; // m/s*s
	UPROPERTY(VisibleAnywhere)
	FVector Velocity; // m/s
	UPROPERTY(VisibleAnywhere)
	FVector DriveForce; // calculated when vehicle is resting on ground

	UPROPERTY(VisibleAnywhere)
	float Speed;
	UPROPERTY(VisibleAnywhere)
	float MaxCenterOfMassHeight;
	UPROPERTY(VisibleAnywhere)
	float MaxTractionForce;
	UPROPERTY(VisibleAnywhere)
	float WheelBase;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* CenterOfMass;

	UPROPERTY(VisibleAnywhere)
	TArray<UChildActorComponent*> WheelComponents;
};
