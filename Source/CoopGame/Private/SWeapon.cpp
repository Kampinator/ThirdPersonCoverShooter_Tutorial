// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MuzzleSocketName = "MuzzleSocket";

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASWeapon::Fire()
{
	//Trace the world, from pawn eyes to crosshair location
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		FVector ShotDirection = EyeRotation.Vector();


		// This is used with the line tracing
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		FHitResult Hit;
		FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * 10000);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			// Blocking hit! Process Damage
			AActor* HitActor = Hit.GetActor();
			// Pass all our data
			UGameplayStatics::ApplyPointDamage(HitActor, 1.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			if (ImpactEffect)
			{
				UE_LOG(LogTemp, Warning, TEXT("Direct hit"));
				DrawDebugSphere(GetWorld(), Hit.Location, 10, 10, FColor::Emerald, false, 1.0f, 1, 1.0f);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, Hit.ImpactNormal.Rotation());
			} 
	


			
			

		}
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		if(MuzzleEffect)
		{ 
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}
	}

}
