// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Core.h"
#include "UnrealNetwork.h"
#include "Character1_CPP.generated.h"

UCLASS()
class SHOOTERPROJECT_API ACharacter1_CPP : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacter1_CPP(const FObjectInitializer &ObjectInitializer);

	UPROPERTY(EditAnywhere)
		float MouseSensitivity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USpringArmComponent *CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent *CameraComp;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	float CurrentDeltaTime;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void StartAiming();
	void StopAiming();
	void SetAiming(bool Aim);

	UFUNCTION(Server, unreliable, WithValidation)
		void Server_SetAiming(bool Aim);

	UFUNCTION(NetMulticast, unreliable)
		void Multicast_SetAiming(bool Aim);

	FVector InputDirection = FVector::ZeroVector;
	float ForwardInput = 0;
	float RightInput = 0;

	// Used by Animation Blueprint
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		float AimPitch = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		float AimYaw = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		bool Aiming;

	void SetAimPitchAndYaw();
	float GetPitch();
	float GetYaw();

	UFUNCTION(Server, unreliable, WithValidation)
		void Server_SetPitchYaw(float Pitch, float Yaw);

	UFUNCTION(NetMulticast, unreliable)
		void Multicast_SetPitchYaw(float Pitch, float Yaw);

	FRotator NormalizedDeltaRotator(FRotator A, FRotator B);

public:
	void SetMeshAndActorRotation();

	UFUNCTION(Server, unreliable, WithValidation)
		void Server_SetMeshActorRotation(FRotator MeshRotation, FRotator ActorRotation);

	UFUNCTION(NetMulticast, unreliable)
		void Multicast_SetMeshActorRotation(FRotator MeshRotation, FRotator ActorRotation);

	void SetFinalRotation(FRotator MeshRotation, FRotator ActorRotation);

	FRotator GetSmoothenedRotation();

};

