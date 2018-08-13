// Fill out your copyright notice in the Description page of Project Settings.

#include "Character1_CPP.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UnrealNetwork.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACharacter1_CPP::ACharacter1_CPP(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
		bReplicates = true;
	bReplicateMovement = true;
	SetReplicates(true);
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	CameraComp->SetupAttachment(CameraBoom);

}

// Replicates Variables
void ACharacter1_CPP::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacter1_CPP, AimYaw);
	DOREPLIFETIME(ACharacter1_CPP, AimPitch);
	DOREPLIFETIME(ACharacter1_CPP, Aiming);
	DOREPLIFETIME(ACharacter1_CPP, bSprinting);
	DOREPLIFETIME(ACharacter1_CPP, bFalling);
	DOREPLIFETIME(ACharacter1_CPP, CrntCntrlRot);

}

// Called when the game starts or when spawned
void ACharacter1_CPP::BeginPlay()
{
	Super::BeginPlay();
	//SetAiming(true);
}

// Called every frame
void ACharacter1_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrentDeltaTime = DeltaTime;
	// Set Movement input based on player input.
	InputDirection = (GetActorForwardVector() * ForwardInput) + (GetActorRightVector() * RightInput);
	AddMovementInput(InputDirection);

	// Set Pitch And Yaw of character and print to log.
	SetAimPitchAndYaw();
	///UE_LOG(LogTemp, Warning, TEXT("Pitch: %f, Yaw: %f"), AimPitch, AimYaw);
	SetMeshAndActorRotation();
	SetFalling();

	if (TextRenderer != nullptr) {
		TextRenderer->SetText(Aiming ? TEXT("Aiming: True") : TEXT("Aiming: False"));
		//TextRenderer->SetText(Aiming ? TEXT("Aiming: True") : TEXT("Aiming: False"));
	}
	auto V = FRotator(0, GetVelocity().Rotation().Yaw, 0);
	auto R = FRotator(0, CameraComp->GetComponentRotation().Yaw, 0);
	auto F = Aiming ? R : V;
	bUseControllerRotationYaw = (V == FRotator::ZeroRotator);

	if (F != FRotator::ZeroRotator) {
		//GetMesh()->SetWorldRotation(F);
	}

	
}

//Set Aiming to server and client
void ACharacter1_CPP::SetAiming(bool bNewAim) {
	Aiming = bNewAim;

	if (Role < ROLE_Authority) {
		ServerSetAiming(bNewAim);
	}
}
void ACharacter1_CPP::ServerSetAiming_Implementation(bool bNewAim) {
	SetAiming(bNewAim);
}
bool ACharacter1_CPP::ServerSetAiming_Validate(bool bNewAim) {
	return true;
}
/////////////////////////////////////

void ACharacter1_CPP::SetSprinting(bool bNewSprinting) {
	bSprinting = bNewSprinting;

	GetCharacterMovement()->MaxWalkSpeed = bSprinting ? SprintSpeed : WalkSpeed;

	if (Role < ROLE_Authority) {
		ServerSetSprinting(bNewSprinting);
	}
}

void ACharacter1_CPP::ServerSetSprinting_Implementation(bool bNewSprinting) {
	SetSprinting(bNewSprinting);
}
bool ACharacter1_CPP::ServerSetSprinting_Validate(bool bNewSprinting) { return true; }
/////////////////////////////////////

void ACharacter1_CPP::SetFalling() {
	bFalling = GetCharacterMovement()->IsFalling();
	if (Role < ROLE_Authority) {
		ServerSetFalling();
	}
}
void ACharacter1_CPP::ServerSetFalling_Implementation() {
	SetFalling();
}

bool ACharacter1_CPP::ServerSetFalling_Validate() { return true; }

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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);


}
void ACharacter1_CPP::MoveForward(float Value) { AddMovementInput(GetActorForwardVector()*Value); }
void ACharacter1_CPP::MoveRight(float Value){ AddMovementInput(GetActorRightVector()*Value); }

void ACharacter1_CPP::StartAiming() { SetAiming(true); }
void ACharacter1_CPP::StopAiming() { SetAiming(false); }

void ACharacter1_CPP::StartSprinting() { SetSprinting(true); }
void ACharacter1_CPP::StopSprinting() { SetSprinting(false); }
//////////////////////////////////////////////////////////

///--------Set Actor and Mesh Rotation--------///
// Core of mesh rotatiob functionality
void ACharacter1_CPP::SetMeshAndActorRotation() {
	
	if (Role < ROLE_Authority) {
		//Server_SetMeshActorRotation();
	}
}

// Gets smoothened rotation of mesh
FRotator ACharacter1_CPP::GetSmoothenedRotation() {
	// If moving, use Rotation of input, else use last Mesh's rotation.
	auto Rotation = (LastControlInputVector != FVector::ZeroVector) ? LastControlInputVector.Rotation() : GetMesh()->GetComponentRotation();
	
	// If aiming, rotate the actor towards the control rotation, else use last Mesh's rotation.
	auto R = GetControlRotation();
	Rotation = Aiming ? FRotator(0, R.Yaw, 0) : Rotation;

	// Return smoothened value from mesh's rotation to the target rotation.
	return FMath::RInterpTo(GetMesh()->GetComponentRotation(), Rotation, CurrentDeltaTime, float(10));
}

// Makes sure the rotation is synced to server and clients
void ACharacter1_CPP::Server_SetMeshActorRotation_Implementation()
{
	SetMeshAndActorRotation();
}

void ACharacter1_CPP::Multicast_SetMeshActorRotation_Implementation() {
	auto R = GetControlRotation();
	auto FR = FRotator(0, R.Yaw, 0);
	if (GetControlRotation() != FRotator::ZeroRotator && LastControlInputVector != FVector::ZeroVector) {
		SetActorRotation(FR);
		GetMesh()->SetWorldRotation(LastControlInputVector.Rotation());
	}
}

bool ACharacter1_CPP::Server_SetMeshActorRotation_Validate() { return true; }
void ACharacter1_CPP::SetFinalRotation(FRotator MeshRotation, FRotator ActorRotation) 
{
	//if (LastControlInputVector != FVector::ZeroVector) {
		SetActorRotation(ActorRotation);
		GetMesh()->SetWorldRotation(MeshRotation);
	//}
}
///------------------------------------------///

///--------Set Pitch And Yaw--------///
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

// Gets pitch relative to mesh rotation
float ACharacter1_CPP::GetPitch() {
	auto B = GetMesh()->GetComponentRotation();
	auto A = GetControlRotation();
	auto Delta = NormalizedDeltaRotator(A, B);
	return Delta.Pitch;
}

// Gets yaw relative to mesh rotation
float ACharacter1_CPP::GetYaw() {
	auto B = GetMesh()->GetComponentRotation();
	auto A = GetControlRotation();
	auto Delta = FMath::Clamp(NormalizedDeltaRotator(A, B).Yaw, float(-90), float(90));
	return Delta;
}

// Set value to server
void ACharacter1_CPP::Server_SetPitchYaw_Implementation(float Pitch, float Yaw) {
	Multicast_SetPitchYaw_Implementation(Pitch, Yaw);
}

bool ACharacter1_CPP::Server_SetPitchYaw_Validate(float Pitch, float Yaw) { return true; }

// Multicast values down to clients
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
///------------------------------------------///




