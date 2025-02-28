// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "WeaponComponent.generated.h"

class AProjectile;
class ACharacter1_CPP;
class ALineProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERPROJECT_API UWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	// Variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TimeBetweenShots = 0.2;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Damage = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Spread = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ReloadTime = 2;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float BulletsPerMag = 30;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MaxBullets = 600;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RecoilDistance = 0.5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		ACharacter1_CPP *CharacterRef;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Camera)
		float FOVIncrement = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<ALineProjectile> Projectile;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FTransform BarrelSocket;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FTransform FrontGripSocket;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UStaticMeshComponent *WeaponMesh;

	UFUNCTION(Server, WithValidation, Reliable)
		void ServerFireWeapon(FVector newAimLocation);

	UFUNCTION(Client, Unreliable)
		void ClientRecoilWeapon();

	UFUNCTION(Client, Unreliable)
		void ClientFireWeapon(FVector newAimLocation);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastFireWeapon(FVector newAimLocation);

	void FireWeapon(FVector newAimLocation);

private:
	float NextShotTime;

};
