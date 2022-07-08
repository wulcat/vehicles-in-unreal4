// Fill out your copyright notice in the Description page of Project Settings.


#include "VehiclePawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4w.h"

static const FName NAME_SteerInputAxis("Steer");
static const FName NAME_ThrottleInputAxis("Throttle");
static const FName NAME_LookUpInputAxis("LookUp");
static const FName NAME_TurnInputAxis("Turn");
static const FName NAME_HandbrakeInputAction("Handbrake");

AVehiclePawn::AVehiclePawn() {
	UWheeledVehicleMovementComponent4W* vehicle4w = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	// Adjust Tire Loading
	vehicle4w->MinNormalizedTireLoad = 0.0f;
	vehicle4w->MinNormalizedTireLoadFiltered = 0.2f;
	vehicle4w->MaxNormalizedTireLoad = 2.0f;
	vehicle4w->MaxNormalizedTireLoadFiltered = 2.2f;

	// Torque Setup
	vehicle4w->MaxEngineRPM = 5700.0f;
	vehicle4w->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	vehicle4w->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	vehicle4w->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 500.0f);
	vehicle4w->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

	// Steering Curve
	vehicle4w->SteeringCurve.GetRichCurve()->Reset();
	vehicle4w->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	vehicle4w->SteeringCurve.GetRichCurve()->AddKey(40.0f, 0.7f);
	vehicle4w->SteeringCurve.GetRichCurve()->AddKey(120.0f, 0.6);

	// Basically give rear wheels more spin than forward wheels
	vehicle4w->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	vehicle4w->DifferentialSetup.FrontRearSplit = 0.65f;

	// Automatic Gearbox
	vehicle4w->TransmissionSetup.bUseGearAutoBox = true;
	vehicle4w->TransmissionSetup.GearSwitchTime = 0.15f;
	vehicle4w->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	// Create Spring Arm Component for our chase camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 250.0f;
	SpringArm->bUsePawnControlRotation = true;

	// Create Camera Chase component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->FieldOfView = 90.0f;
}

void AVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateInAirControl(DeltaTime);
	
}

void AVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(NAME_SteerInputAxis, this, &AVehiclePawn::ApplySteering);
	PlayerInputComponent->BindAxis(NAME_ThrottleInputAxis, this, &AVehiclePawn::ApplyThrottle);
	PlayerInputComponent->BindAxis(NAME_LookUpInputAxis, this, &AVehiclePawn::LookUp);
	PlayerInputComponent->BindAxis(NAME_TurnInputAxis, this, &AVehiclePawn::Turn);

	PlayerInputComponent->BindAction(NAME_HandbrakeInputAction, IE_Pressed, this, &AVehiclePawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction(NAME_HandbrakeInputAction, IE_Released, this, &AVehiclePawn::OnHandbrakeReleased);
}

void AVehiclePawn::ApplyThrottle(float val)
{
	GetVehicleMovement()->SetThrottleInput(val);
}

void AVehiclePawn::ApplySteering(float val)
{
	GetVehicleMovement()->SetSteeringInput(val);
}

void AVehiclePawn::LookUp(float val)
{
	if (val != 0.f) {
		AddControllerPitchInput(val);
	}
}

void AVehiclePawn::Turn(float val)
{
	if (val != 0.f) {
		AddControllerYawInput(val);
	}
}

void AVehiclePawn::OnHandbrakePressed()
{
	GetVehicleMovement()->SetHandbrakeInput(true);
}

void AVehiclePawn::OnHandbrakeReleased()
{
	GetVehicleMovement()->SetHandbrakeInput(false);
}

void AVehiclePawn::UpdateInAirControl(float DeltaTime)
{
	if (UWheeledVehicleMovementComponent4W* vehicle4w = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement())) {
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		const FVector TraceStart = GetActorLocation() + FVector(0, 0, 50);
		const FVector TraceEnd = GetActorLocation() + FVector(0, 0, 200);

		FHitResult hit;

		// check if car is flipped on it's side and check if it is in air
		const bool bInAir = GetWorld()->LineTraceSingleByChannel(hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
		const bool bNotGrounded = FVector::DotProduct(GetActorUpVector(), FVector::UpVector) < 0.1f; // dot product of world up and actorss upward direction is not aligned then not grounded properly (flipped)

		if (bInAir || bNotGrounded) {
			const float ForwardInput = InputComponent->GetAxisValue(NAME_ThrottleInputAxis);
			const float RightInput = InputComponent->GetAxisValue(NAME_SteerInputAxis);

			// If car is grounded allow player to roll the car over
			const float AirMovementForcePitch = 3.0f; // rolling forwards & backwords
			const float AirMovementForceRoll = !bInAir && bNotGrounded ? 20.0f : 3.0f; // rolling side to side

			// GetVehicle mesh as its' where the physic rigidbody is
			if (UPrimitiveComponent* VehicleMesh = vehicle4w->UpdatedPrimitive) {
				const FVector MovementVector = FVector(RightInput * -AirMovementForceRoll, ForwardInput * AirMovementForcePitch, 0.0f) * DeltaTime * 200;
				const FVector NewAngularMovement = GetActorRotation().RotateVector(MovementVector);

				VehicleMesh->SetPhysicsAngularVelocity(NewAngularMovement, true);
			}
		}

	}
}
