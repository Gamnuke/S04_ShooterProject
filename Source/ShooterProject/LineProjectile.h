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
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<ALineProjectile> ProjectileBlueprint;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
		FVector TargetPoint;
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
		FVector ImpactNormal;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ForwardScale;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int Increments;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Duration = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool BounceOff = false;
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

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerSetScale(FVector newTargetPoint, FVector newImpactNormal);
	void SetScale(FVector newTargetPoint, FVector newImpactNormal);

	UFUNCTION(Server, Unreliable, WithValidation)
		void ServerSetVariables(FVector newTargetPoint);
		void SetVariables(FVector newTargetPoint);
};
