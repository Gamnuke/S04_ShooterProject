// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core.h"
#include "UnrealNetwork.h"
#include "Components/TextRenderComponent.h"
#include "Components/ChildActorComponent.h"
#include "Character1_CPP.generated.h"

class UCharacter1AnimInstance;
class AWeapon;
class UWeaponComponent;

UCLASS()
class SHOOTERPROJECT_API ACharacter1_CPP : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacter1_CPP(const FObjectInitializer &ObjectInitializer);

	UPROPERTY(EditAnywhere)
		float MouseSensitivity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USpringArmComponent *CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UCameraComponent *CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UTextRenderComponent *TextRenderer;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent *OuterMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UWeaponComponent *WeaponComponent;

	UPROPERTY(Replicated)
		FRotator CrntCntrlRot;
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

	UCharacter1AnimInstance *AnimInstance;
	FVector InputDirection = FVector::ZeroVector;
	float ForwardInput = 0;
	float RightInput = 0;
	float CurrentCameraFOV;

	bool AimButtonDown;
	bool SprintButtonDown;
	bool FireButtonDown;

	// Used by Animation Blueprint
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		float AimPitch = 0;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		float AimYaw = 0;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		bool Aiming;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		bool Firing;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		bool bSprinting;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		bool bJumping;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		bool bFalling;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		FTransform WeaponHandleLocation;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		FVector CurrentMovementInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraSettings)
		FVector2D MinMaxFOV = FVector2D(60,80);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
		float ZoomSpeed = 10;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WalkSpeed = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SprintSpeed = 500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D AimPointScreenLocation;

	////Aiming
	UFUNCTION(Server, unreliable, WithValidation)
	void ServerSetAiming(bool bNewAim);
	void SetAiming(bool bNewAim);
	void StartAiming();
	void StopAiming();

	UFUNCTION(Server, unreliable, WithValidation)
	void ServerSetFiring(bool bNewFire);
	void SetFiring(bool bNewFire);
	void StartFiring();
	void StopFiring();
	
	UFUNCTION(Server, unreliable, WithValidation)
	void ServerSetSprinting(bool bNewSprinting);
	void SetSprinting(bool bNewSprinting);
	void StartSprinting();
	void StopSprinting();

	UFUNCTION(Server, unreliable, WithValidation)
	void ServerSetFalling();
	void SetFalling();

	UFUNCTION(Server, unreliable, WithValidation)
	void Server_SetPitchYaw(float Pitch, float Yaw);
	void SetAimPitchAndYaw();
	float GetPitch();
	float GetYaw();
	UFUNCTION(NetMulticast, unreliable)
	void Multicast_SetPitchYaw(float Pitch, float Yaw);
	FRotator NormalizedDeltaRotator(FRotator A, FRotator B);
	FRotator MeshRotation;

	UFUNCTION(Server, unreliable, WithValidation)
	void ServerSetMeshRotation(FRotator NewRotation, bool ToServer);
	void SetMeshRotation(FRotator NewRotation, bool ToServer);
	FRotator GetSmoothRotation(FRotator NewRotation);

	UFUNCTION(Server, unreliable, WithValidation)
	void ServerSetMovementInput(FVector MovementInput);
	void SetMovementInput(FVector MovementInput);
	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Server, unreliable, WithValidation)
	void ServerSetWeaponRotation(FRotator newRotation, bool Relative);

	UFUNCTION(Client, unreliable)
		void ClientSetWeaponRotation(FRotator newRotation, bool Relative);

	void SetWeaponRotationFinal(FRotator newRotation, bool Relative);
	void SetWeaponRotation();
	/*UFUNCTION(NetMulticast, unreliable)
	void MulticastSetWeaponRotation(FRotator NewRotation, FVector NewAimLocation, bool Relative);
	void SetWeaponRotationRepetitive(FRotator NewRotation, FVector NewAimLocation, bool Relative);*/


	//-----------WEAPON VARIABLES------------//
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		FVector AimLocation;

	UFUNCTION(Server, unreliable, WithValidation)
	void ServerSetWeaponVariables(FTransform newBarrelSocket,
		FTransform newFrontGripSocket
	);
	void SetWeaponVariables(FTransform newBarrelSocket,FTransform newFrontGripSocket);

};

