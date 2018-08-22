// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LineProjectile.generated.h"

UCLASS()
class SHOOTERPROJECT_API ALineProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALineProjectile(const FObjectInitializer &ObjectInitializer);


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UStaticMeshComponent *Mesh;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USceneComponent *MeshScene;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
		FVector TargetPoint;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ForwardScale;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int Increments;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Duration = 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float TargetScale = 0;
	float EndTime;
	float StartTime;
	FVector StartLocation;
};
