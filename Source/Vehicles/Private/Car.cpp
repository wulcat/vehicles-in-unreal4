// Fill out your copyright notice in the Description page of Project Settings.


#include "Car.h"

// Sets default values
ACar::ACar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if(!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	}

	if(!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));

		RootComponent = CollisionComponent;
	}

	if(!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	}
	
	if(!CenterOfMass)
	{
		CenterOfMass = CreateDefaultSubobject<UArrowComponent>(TEXT("Center of Mass"));
	}

	if(WheelComponents.IsEmpty())
	{
		auto WheelActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Wheel_RL"));
		WheelComponents.Add(WheelActor);

		WheelActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Wheel_RR"));
		WheelComponents.Add(WheelActor);

		WheelActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Wheel_TL"));
		WheelComponents.Add(WheelActor);

		WheelActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Wheel_TR"));
		WheelActor->SetRelativeLocation(FVector(100,-100,0));
		WheelComponents.Add(WheelActor);
	}
	else
	{
		for(const auto WheelActor : WheelComponents)
		{
			WheelActor->SetChildActorClass(AWheelMesh);
		}
	}

	
}

// Called when the game starts or when spawned
void ACar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

