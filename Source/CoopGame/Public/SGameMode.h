// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

class ASGameState;
enum class EWaveState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor);

/**
 * 
 */

UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

	void StartWave();
	void EndWave();
	// Set timer for next start wave
	void PrepareForNextWave();

protected:
	int32 NroBotsToSpawn;
	int32 WaveCount;
	void SpawnBotTimerElapsed();

	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_NextWaveStart;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	void CheckWaveState();
	void SetWaveState(EWaveState NewState);

	void RestartDeadPlayers();

public:
	ASGameMode();
	virtual void StartPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnBot();

	virtual void Tick(float DeltaSeconds) override;

	void CheckAnyPlayerAlive();

	void GameOver();

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

	
	
	
	
};
