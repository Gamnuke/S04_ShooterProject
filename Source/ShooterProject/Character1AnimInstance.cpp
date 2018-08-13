// Fill out your copyright notice in the Description page of Project Settings.

#include "Character1AnimInstance.h"
#include "Character1_CPP.h"

void UCharacter1AnimInstance::ParentTick() {
	if (CharacterRef != nullptr) {
		Sprinting = CharacterRef->bSprinting;
		Aiming = CharacterRef->Aiming;
		WeaponHandleLocation = CharacterRef->WeaponHandleLocation;
		Yaw = CharacterRef->AimYaw;
		Pitch = CharacterRef->AimPitch;
		Falling = CharacterRef->bFalling;
		Jumping = CharacterRef->bJumping;
	}
}