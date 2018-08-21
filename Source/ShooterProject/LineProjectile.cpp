// Fill out your copyright notice in the Description page of Project Settings.

#include "LineProjectile.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ALineProjectile::ALineProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshScene = CreateDefaultSubobject<USceneComponent>(FName("MeshScene"), false);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"), false);
	MeshScene->SetupAttachment(GetRootComponent());
	Mesh->SetupAttachment(MeshScene);
}

// Called when the game starts or when spawned
void ALineProjectile::BeginPlay()
{
	Super::BeginPlay();
	TargetScale = FVector::Distance(GetActorLocation(), TargetPoint) / 100;
	StartTime = GetWorld()->TimeSeconds;
	EndTime = StartTime + Duration;
}

// Called every frame
void ALineProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float Alpha = FMath::Clamp(UKismetMathLibrary::NormalizeToRange(GetWorld()->TimeSeconds, StartTime, EndTime),0.0f,1.0f);
	UE_LOG(LogTemp, Warning, TEXT("%f"),Alpha);
	SetActorScale3D(FVector(FMath::Lerp(float(1), TargetScale, Alpha), GetActorScale3D().Y, GetActorScale3D().Z));
}

