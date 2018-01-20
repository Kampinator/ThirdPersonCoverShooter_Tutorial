// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"


class USHealthComponent;
class UMaterialInstanceDynamic;
class UParticleSystem;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "Tracker bot")
	USHealthComponent* HealthComponent;
	FVector GetNextPathPoint();
	FVector NextPathPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Tracker bot")
	float MovementForce;
	UPROPERTY(EditDefaultsOnly, Category = "Tracker bot")
	bool bUseVelocityChange;
	UPROPERTY(EditDefaultsOnly, Category = "Tracker bot")
	float RequiredDistanceToTarget;
	UFUNCTION()
	void HandleTakeAnyDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	// Dynamic Material to pulse on damage
	UMaterialInstanceDynamic* MatInst;
	void SelfDestruct();
	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	UParticleSystem* ExplosionEffect;

	bool bExploded;
	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	float Explosionradius;
	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	float ExplosionDamage;
	
	FTimerHandle TimerHandle_SelfDamage;
	void DamageSelf();

	bool bStartedSelfDestruction;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot Sound")
	USoundCue* SelfdestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot Sound")
	USoundCue* ExplodeSound;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	float SelfDamageInterval;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	


	
	
};
