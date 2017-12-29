// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
	BaseDamage = 20.0f;

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
		QueryParams.bReturnPhysicalMaterial = true;
		// Particle "Target" parameter
		FVector TracerEndPoint = TraceEnd;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			// Blocking hit! Process Damage
			AActor* HitActor = Hit.GetActor();
			// Pass all our data
			
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			float ActualDamage = 10;
			if (SurfaceType == SURFACE_FLESHVUNERABLE)
			{
				ActualDamage = BaseDamage * 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			UParticleSystem* SelectedEffect = nullptr;

			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
				SelectedEffect = FleshImpactEffect;
				break;
			case SURFACE_FLESHVUNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			if (SelectedEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.Location, Hit.ImpactNormal.Rotation());
			}




			TracerEndPoint = Hit.ImpactPoint;
			if (DebugWeaponDrawing > 0)
			{
				DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
			}		
			PlayFireEffects(TracerEndPoint);
		}
	}
}

void ASWeapon::PlayFireEffects(const FVector& TraceEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
	if (TracerComp)
	{
		TracerComp->SetVectorParameter("Target", TraceEnd);
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			SIMPLELOG("Playing CameraShake");
			PC->ClientPlayCameraShake(FireCamShake);
			
		}
	}

	


}
