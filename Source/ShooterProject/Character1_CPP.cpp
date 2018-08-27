// Fill out your copyright notice in the Description page of Project Settings.

#include "Character1_CPP.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UnrealNetwork.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character1AnimInstance.h"
#include "Weapon.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponComponent.h"
#include "DrawDebugHelpers.h"
#include "LineProjectile.h"

// Sets default values
ACharacter1_CPP::ACharacter1_CPP(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	OuterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("OuterMesh"));
	OuterMesh->SetupAttachment(GetRootComponent());

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	CameraComp->SetupAttachment(CameraBoom);

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(FName("WeaponComponent"));
	WeaponComponent->SetupAttachment(OuterMesh, FName("WeaponSocket"));
	WeaponComponent->CharacterRef = this;
}

// Replicates Variables
void ACharacter1_CPP::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacter1_CPP, AimYaw);
	DOREPLIFETIME(ACharacter1_CPP, AimPitch);
	DOREPLIFETIME(ACharacter1_CPP, Aiming);
	DOREPLIFETIME(ACharacter1_CPP, bSprinting);
	DOREPLIFETIME(ACharacter1_CPP, CurrentMovementInput);
	DOREPLIFETIME(ACharacter1_CPP, AimLocation);
}

// Called when the game starts or when spawned
void ACharacter1_CPP::BeginPlay()
{
	Super::BeginPlay();

	if (OuterMesh->GetAnimInstance() != nullptr) {
		AnimInstance = Cast<UCharacter1AnimInstance>(OuterMesh->GetAnimInstance());

		if (AnimInstance != nullptr) {
			AnimInstance->CharacterRef = this;
			if (WeaponComponent != nullptr) {
				AnimInstance->WeaponRef = WeaponComponent;
			}
		}
	}
	SetSprinting(false);
	CurrentCameraFOV = MinMaxFOV.X;
	WeaponComponent->CharacterRef = this;
	WeaponHandleLocation.SetLocation(WeaponComponent->FrontGripSocket.GetLocation() - WeaponComponent->GetComponentLocation());
}

// Called every frame
void ACharacter1_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrentDeltaTime = DeltaTime;

	SetAimPitchAndYaw();
	SetFalling();
	
	if (AnimInstance != nullptr) {
		AnimInstance->ParentTick();
	}
	if (CurrentMovementInput != FVector::ZeroVector) {
		SetMeshRotation(!Aiming ? ((GetVelocity()*FVector(1, 1, 0)).Rotation()) : GetActorRotation(), true);
	}
	else {
		SetMeshRotation(GetActorRotation(), true);
	}

	//Set Weapon Front Grip Location
	if (WeaponComponent != nullptr) {
		if (WeaponComponent->WeaponMesh == nullptr) { return; }

		SetWeaponRotation();
		WeaponHandleLocation.SetRotation(WeaponComponent->FrontGripSocket.GetRotation());
		SetWeaponVariables(
			WeaponComponent->WeaponMesh->GetSocketTransform(FName("FrontGrip")),
			WeaponComponent->WeaponMesh->GetSocketTransform(FName("Barrel"))
		);

		if (Firing && Role == ROLE_AutonomousProxy)
		{
			WeaponComponent->ServerFireWeapon(AimLocation);

		}

	}

	/*if (InputComponent != nullptr) {
		SetMovementInput(InputComponent->GetAxisValue(FName("MoveForward")) * GetActorForwardVector() + InputComponent->GetAxisValue(FName("MoveRight")) * GetActorRightVector());
	}*/

	/*if (CameraBoom != nullptr) {
		CameraBoom->SetWorldRotation(GetControlRotation());
		FRotator RelativeCamRot = CameraBoom->RelativeRotation;
		RelativeCamRot.Pitch = FMath::ClampAngle(RelativeCamRot.Pitch, -45, 45);
		CameraBoom->SetRelativeRotation(RelativeCamRot);
	}*/
	

	if (CameraComp != nullptr) {
		CameraComp->SetFieldOfView(FMath::FInterpTo(CameraComp->FieldOfView, CurrentCameraFOV, DeltaTime, ZoomSpeed));
	}
}
void ACharacter1_CPP::SetWeaponRotation() {
	APlayerController *PlayerCont = Cast<APlayerController>(GetController());
	FHitResult OutHit;
	FVector WorldLoc;
	FVector Direction;
	PlayerCont->DeprojectScreenPositionToWorld(AimPointScreenLocation.X, AimPointScreenLocation.Y, OUT WorldLoc, OUT Direction);

	//WorldLoc = WorldLoc + Direction;
	GetWorld()->LineTraceSingleByChannel(
		OUT OutHit,
		WorldLoc, WorldLoc + Direction * 10000,
		WeaponFireCC
	);
	if (Aiming) {
		if (PlayerCont != nullptr) {
			if(OutHit.bBlockingHit && OutHit.GetActor()!=nullptr){
				if (!OutHit.GetActor()->IsA(ALineProjectile::StaticClass())) {
					AimLocation = OutHit.ImpactPoint;
				}

			}
			else {
				AimLocation = WorldLoc + Direction * 10000;
			}
			ClientSetWeaponRotation((AimLocation - WeaponComponent->GetComponentLocation()).Rotation(), false);
			if (!HasAuthority()) {
				ServerSetWeaponRotation((AimLocation - WeaponComponent->GetComponentLocation()).Rotation(), false);
			}
		}
	}
	else {
		//SetWeaponRotationRepetitive(FRotator::ZeroRotator, OutHit.TraceEnd, true);
		WeaponComponent->SetRelativeRotation(FRotator(0,0,0));
		AimLocation = WeaponComponent->BarrelSocket.GetLocation() + WeaponComponent->BarrelSocket.GetRotation().GetForwardVector() * 10000000;
		ClientSetWeaponRotation(FRotator(0, 0, 0), true);

		if (!HasAuthority()) {
			ServerSetWeaponRotation(FRotator(0,0,0), true);
		}
	}
}

void ACharacter1_CPP::ServerSetWeaponRotation_Implementation(FRotator newRotation, bool Relative) {
	SetWeaponRotationFinal(newRotation, Relative);
}
bool ACharacter1_CPP::ServerSetWeaponRotation_Validate(FRotator newRotation, bool Relative) {
	return true;
}
void ACharacter1_CPP::ClientSetWeaponRotation_Implementation(FRotator newRotation, bool Relative) {
	SetWeaponRotationFinal(newRotation, Relative);
}
void ACharacter1_CPP::SetWeaponRotationFinal(FRotator newRotation, bool Relative) {
	if (Relative) {
		WeaponComponent->SetRelativeRotation(newRotation);
	}
	else {
		WeaponComponent->SetWorldRotation(newRotation);
	}
	/*FRotator ClampedRelative = WeaponComponent->GetComponentRotation();
	ClampedRelative.Pitch = FMath::ClampAngle(ClampedRelative.Pitch,-90, 90);
	ClampedRelative.Roll = 0;
	WeaponComponent->SetWorldRotation(ClampedRelative);*/
}

// Called to bind functionality to input
void ACharacter1_CPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Check if Input has been set up
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MousePitch", this, &ACharacter1_CPP::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MouseYaw", this, &ACharacter1_CPP::AddControllerYawInput);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacter1_CPP::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacter1_CPP::MoveRight);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACharacter1_CPP::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ACharacter1_CPP::StopAiming);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACharacter1_CPP::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACharacter1_CPP::StopSprinting);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacter1_CPP::StartFiring);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacter1_CPP::StopFiring);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
}

//--------Set MovementInput--------//
void ACharacter1_CPP::MoveForward(float Value) { SetMovementInput(GetActorForwardVector()*Value); }
void ACharacter1_CPP::MoveRight(float Value) { SetMovementInput(GetActorRightVector()*Value); }
void ACharacter1_CPP::SetMovementInput(FVector MovementInput) {
	//CurrentMovementInput = MovementInput.GetSafeNormal();
	AddMovementInput(MovementInput);
	CurrentMovementInput = GetControlRotation().Vector();
	if (Role == ROLE_AutonomousProxy) {
		ServerSetMovementInput(MovementInput);
	}
}
void ACharacter1_CPP::ServerSetMovementInput_Implementation(FVector MovementInput) {
	SetMovementInput(MovementInput);
}
bool ACharacter1_CPP::ServerSetMovementInput_Validate(FVector MovementInput) { return true; }

//--------Set bAiming--------//
void ACharacter1_CPP::StartAiming() {
	AimButtonDown = true;
	SetAiming(true);
	SetSprinting(false);
}
void ACharacter1_CPP::StopAiming() {
	AimButtonDown = false;
	SetAiming(false);
	if (SprintButtonDown) {
		SetSprinting(true);
	}
}
void ACharacter1_CPP::SetAiming(bool bNewAim) {
	Aiming = bNewAim;
	CurrentCameraFOV = Aiming ? MinMaxFOV.Y : MinMaxFOV.X;
	if (Role == ROLE_AutonomousProxy) {
		ServerSetAiming(bNewAim);
	}
}
void ACharacter1_CPP::ServerSetAiming_Implementation(bool bNewAim) {
	SetAiming(bNewAim);
}
bool ACharacter1_CPP::ServerSetAiming_Validate(bool bNewAim) { return true; }

//--------Set bFiring--------//
void ACharacter1_CPP::StartFiring() {
	FireButtonDown = true;
	SetFiring(true);
}
void ACharacter1_CPP::StopFiring() {
	FireButtonDown = false;
	SetFiring(false);
}
void ACharacter1_CPP::SetFiring(bool bNewFire) {
	Firing = bNewFire;
	if (Role < ROLE_Authority) {
		ServerSetFiring(bNewFire);
	}
}
void ACharacter1_CPP::ServerSetFiring_Implementation(bool bNewFire) {
	SetFiring(bNewFire);
}
bool ACharacter1_CPP::ServerSetFiring_Validate(bool bNewFire) { return true; }

//--------Set bSprinting--------//
void ACharacter1_CPP::StartSprinting() {
	SprintButtonDown = true;
	SetSprinting(true);
	SetAiming(false);
}
void ACharacter1_CPP::StopSprinting() {
	SprintButtonDown = false;
	SetSprinting(false);
	if (AimButtonDown) {
		SetAiming(true);
	}
}
void ACharacter1_CPP::SetSprinting(bool bNewSprinting) {
	bSprinting = bNewSprinting;
	GetCharacterMovement()->MaxWalkSpeed = bSprinting ? SprintSpeed : WalkSpeed;
	if (Role == ROLE_AutonomousProxy) {
		ServerSetSprinting(bNewSprinting);
	}
}
void ACharacter1_CPP::ServerSetSprinting_Implementation(bool bNewSprinting) {
	SetSprinting(bNewSprinting);
}
bool ACharacter1_CPP::ServerSetSprinting_Validate(bool bNewSprinting) { return true; }

//--------Set bFalling--------//
void ACharacter1_CPP::SetFalling() {
	bFalling = GetCharacterMovement()->IsFalling();
	if (Role == ROLE_AutonomousProxy) {
		ServerSetFalling();
	}
}
void ACharacter1_CPP::ServerSetFalling_Implementation() {
	SetFalling();
}
bool ACharacter1_CPP::ServerSetFalling_Validate() { return true; }

//--------Set Pitch And Yaw--------//
void ACharacter1_CPP::SetAimPitchAndYaw() {
	if (HasAuthority() && IsLocallyControlled()) { // Check if the server is a controlled pawn
		// We're on the server so cast values down to clients
		Multicast_SetPitchYaw_Implementation(GetPitch(), GetYaw());

		// Set values directly so the player won't get input lag
		AimPitch = GetPitch();
		AimYaw = GetYaw();
	}
	else if (GetController() != nullptr) { // Check if the pawn is being posessed by a controller
		// We're on a client so cast values up to server
		Server_SetPitchYaw_Implementation(GetPitch(), GetYaw());

		// Set values directly so the player won't get input lag
		AimPitch = GetPitch();
		AimYaw = GetYaw();
	}
}
float ACharacter1_CPP::GetPitch() {
	auto B = GetMesh()->GetComponentRotation();
	auto A = GetControlRotation();
	auto Delta = NormalizedDeltaRotator(A, B);
	return Delta.Pitch;
}
float ACharacter1_CPP::GetYaw() {
	auto B = GetMesh()->GetComponentRotation();
	auto A = GetControlRotation();
	auto Delta = FMath::Clamp(NormalizedDeltaRotator(A, B).Yaw, float(-90), float(90));
	return Delta;
}
void ACharacter1_CPP::Server_SetPitchYaw_Implementation(float Pitch, float Yaw) {
	Multicast_SetPitchYaw_Implementation(Pitch, Yaw);
}
bool ACharacter1_CPP::Server_SetPitchYaw_Validate(float Pitch, float Yaw) { return true; }
void ACharacter1_CPP::Multicast_SetPitchYaw_Implementation(float Pitch, float Yaw) {
	AimPitch = Pitch;
	AimYaw = Yaw;
}
FRotator ACharacter1_CPP::NormalizedDeltaRotator(FRotator A, FRotator B)
{
	FRotator Delta = A - B;
	Delta.Normalize();
	return Delta;
}

//-----------Set Outer Mesh Rotation-------------//
void ACharacter1_CPP::SetMeshRotation(FRotator NewRotation, bool ToServer) {
	MeshRotation = NewRotation;
	auto SmoothedRotation = GetSmoothRotation(NewRotation);
	OuterMesh->SetWorldRotation(SmoothedRotation);

	if (GetController() != nullptr) {
		FRotator ClampedControlRotation = GetControlRotation();
		ClampedControlRotation.Pitch = FMath::ClampAngle(ClampedControlRotation.Pitch, -80, 70);
		GetController()->SetControlRotation(ClampedControlRotation);
	}

	if (Role == ROLE_AutonomousProxy && ToServer) {
		ServerSetMeshRotation(NewRotation, ToServer);
	}
}
void ACharacter1_CPP::ServerSetMeshRotation_Implementation(FRotator NewRotation, bool ToServer) {
	SetMeshRotation(NewRotation, ToServer);
	//MulticastSetMeshRotation(NewRotation);
}
bool ACharacter1_CPP::ServerSetMeshRotation_Validate(FRotator NewRotation, bool ToServer){ return true; };
FRotator ACharacter1_CPP::GetSmoothRotation(FRotator NewRotation) {
	return FMath::RInterpTo(OuterMesh->GetComponentRotation(), NewRotation, GetWorld()->DeltaTimeSeconds, 10);
}

void ACharacter1_CPP::SetWeaponVariables(FTransform newBarrelSocket, FTransform newFrontGripSocket)
{
	if (WeaponComponent != nullptr)
	{
		if (WeaponComponent->WeaponMesh == nullptr) { return; }
		WeaponComponent->FrontGripSocket = newBarrelSocket;
			WeaponComponent->BarrelSocket = newFrontGripSocket;
	}

	if (Role < ROLE_Authority)
	{
		ServerSetWeaponVariables(newBarrelSocket, newFrontGripSocket);
	}
}
void ACharacter1_CPP::ServerSetWeaponVariables_Implementation(FTransform newBarrelSocket,FTransform newFrontGripSocket)
{
	SetWeaponVariables(newBarrelSocket, newFrontGripSocket);
}

bool ACharacter1_CPP::ServerSetWeaponVariables_Validate(FTransform newBarrelSocket,FTransform newFrontGripSocket)
{
	return true;
}


