// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealNetwork.h"
#include "Weapon.generated.h"

class AProjectile;

UCLASS()
class SHOOTERPROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
		//UClass *Projectile;
	TSubclassOf<AProjectile> Projectile;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FTransform BarrelSocket;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FTransform FrontGripSocket;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UStaticMeshComponent *WeaponMesh;

	UFUNCTION()
		void SetFiring(bool bNewFiring);

	UFUNCTION(Server, WithValidation, Unreliable)
		void ServerFireWeapon();
	UFUNCTION(NetMulticast, Unreliable)
		void MulticastFireWeapon();
	UFUNCTION(CLient, Unreliable)
		void ClientFireWeapon();

	void FireWeaponNetworkHandler();
private:
	float NextShotTime;
	bool Firing;
	void FireWeapon();
};
