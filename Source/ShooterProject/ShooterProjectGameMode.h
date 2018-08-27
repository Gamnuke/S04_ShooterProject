// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterProjectGameMode.generated.h"

UCLASS(minimalapi)
class AShooterProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShooterProjectGameMode();

	enum MenuState {
		MainMenu,
		SinglePlayer,
		MultiPlayer,
		Shop,
		PauseMenu,
		InGame
	};
	MenuState CurrentState = MenuState::MainMenu;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Yeet;
};



