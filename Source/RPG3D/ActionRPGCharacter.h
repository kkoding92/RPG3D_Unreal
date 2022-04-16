// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ActionRPGCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Dead UMETA(DisplayName = "DEAD"),
	EMS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class RPG3D_API AActionRPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AActionRPGCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Jump() override;

	void UpDown(float Value);
	void LeftRight(float Value);
	void Yaw(float Value);
	void Pitch(float Value);
	void CameraMoveOn();
	void CameraMoveOff();
	void LMBDown();
	void LMBUp();
	void Attack();

private:
	void SetInterpToEnemy(bool Interp);

	FRotator GetLookAtRotationYaw(FVector Target);

	void Die();

public:
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	AEnemy* GetCombatTarget() { return CombatTarget; }

	UFUNCTION(BlueprintCallable)
	bool GetHasCombatTarget() { return bHasCombatTarget; }

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY()
	class UCharacterAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums", Meta = (AllowPrivateAccess = true))
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller", Meta = (AllowPrivateAccess = true))
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	class AEnemy* CombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	class USoundCue* SwingSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat", Meta = (AllowPrivateAccess = true))
	class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	AController* WeaponInstigator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat", Meta = (AllowPrivateAccess = true))
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	bool bHasCombatTarget;

	UPROPERTY(VisibleAnywhere)
	bool IsAttacking = false;

	UPROPERTY(VisibleAnywhere)
	bool CameraToggle = false;

	UPROPERTY()
	int32 AttackIndex = 0;

	float InterpSpeed;
	bool bInterpToEnemy;
	bool bLMBDown = false;

public:
	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }
	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }
	FORCEINLINE AMainPlayerController* GetPlayerController() { return MainPlayerController; }
	FORCEINLINE UParticleSystem* GetHitParticles() { return HitParticles; };
	FORCEINLINE USoundCue* GetHitSound() { return HitSound; }
	FORCEINLINE void SetInstigator(AController* Inst) { WeaponInstigator = Inst; }
};
