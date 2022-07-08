// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "VehiclePawn.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLE_API AVehiclePawn : public AWheeledVehicle
{
	GENERATED_BODY()
	
public:
	
	AVehiclePawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void ApplyThrottle(float val);
	void ApplySteering(float val);

	void LookUp(float val);
	void Turn(float val);

	void OnHandbrakePressed();
	void OnHandbrakeReleased();

	void UpdateInAirControl(float val);

protected:
	/** Sprint arm that will offset the camera */
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;

};
