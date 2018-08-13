// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Character1AnimInstance.generated.h"

class ACharacter1_CPP;
/**
 * 
 */
UCLASS()
class SHOOTERPROJECT_API UCharacter1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ACharacter1_CPP *CharacterRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent *WeaponRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Sprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Aiming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Speed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Yaw = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Pitch = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Jumping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Falling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector SupportArmEffectorLocation = FVector(0.840000, 0.561680, -0.168000);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform WeaponHandleLocation;

	UFUNCTION()
		void ParentTick();
};
