// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "SHealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "Sound/SoundCue.h"





// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));


	RootComponent = MeshComp;
	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;
	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	bExploded = false;
	ExplosionDamage = 40;
	Explosionradius = 200;
	SelfDamageInterval = 0.25f;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetRelativeLocation(FVector(0, 0, 0));


	

}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeAnyDamage);
	bUseVelocityChange = true;
	if (Role == ROLE_Authority)		
	{
		NextPathPoint = GetNextPathPoint();
	}
	
	
}

FVector ASTrackerBot::GetNextPathPoint()
{
	// Hack to get player location
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerPawn)
	{
		UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
		if (NavPath->PathPoints.Num() > 1)
		{
			// Return wanted location
			return NavPath->PathPoints[1];
		}
	}
	// Failed to find path
	return GetActorLocation();
}

void ASTrackerBot::HandleTakeAnyDamage(USHealthComponent* OwningHealthCompHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//Exploded on hitpoints = 0;

	// TODO Pulse the material on hit
	UE_LOG(LogTemp, Warning, TEXT("Health: %s of %s"), *FString::SanitizeFloat(Health), *GetName());
	//Mat MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial);
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
	
	if (Health <= 0)
		SelfDestruct();
		
	
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
		return;
	bExploded = true;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Delete actor immediadely
	if (Role == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), Explosionradius, nullptr, IgnoredActors, this, GetInstigatorController(), true);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), GetActorLocation(), Explosionradius, 12, FColor::Red, 2.0f, 1.0f);
		SetLifeSpan(2.0f);
	}
	
}


// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Role == ROLE_Authority && !bExploded)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();
		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			// Keep moving towards next target
			NextPathPoint = GetNextPathPoint();
			UKismetSystemLibrary::DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached");
		}
		else
		{
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;
			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
			UKismetSystemLibrary::DrawDebugArrow(GetWorld(), GetActorLocation(), GetActorLocation() * ForceDirection, 32, FColor::Yellow, 0, 0);
		}

		//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, 5, 1);
		
	}

}

void ASTrackerBot::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	bUseVelocityChange = true;
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!bStartedSelfDestruction && !bExploded)
	{
		bStartedSelfDestruction = true;
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn)
		{
			if (Role == ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0);
			}
			// we overlapped actor			
			UGameplayStatics::SpawnSoundAttached(SelfdestructSound, RootComponent);

		}
	}
	
}

void ASTrackerBot::DamageSelf()
{
	// Start self destructor
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);

}


