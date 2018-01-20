// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PowerupInterval = 0;
	TotalNrOfTicks = 0;
	SetReplicates(true);
	bIsPowerupActive = false;

}


void ASPowerupActor::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASPowerupActor, bIsPowerupActive);
}

void ASPowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
}


void ASPowerupActor::OnTickPowerup()
{
	TicksProcessed++;
	OnPowerupTicked();
	if (TicksProcessed >= TotalNrOfTicks)
	{
		bIsPowerupActive = false;
		OnExpired();
		
		// We need to manually call it. Because onRep isn't called on server
		OnRep_PowerupActive();
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void ASPowerupActor::ActivatePowerup(AActor* ActivateFor)
{
	bIsPowerupActive = true;
	OnActivated(ActivateFor);
	
	// We need to manually call it. Because onRep isn't called on server
	OnRep_PowerupActive(); 
	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
}
