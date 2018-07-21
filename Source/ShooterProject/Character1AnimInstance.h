// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character1AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERPROJECT_API UCharacter1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
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

	void SetSprinting(bool bSprinting);
};
