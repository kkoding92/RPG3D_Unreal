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
	void Attack();


public:
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY()
	class UCharacterAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums", Meta = (AllowPrivateAccess = true))
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere)
	bool IsAttacking = false;

	UPROPERTY(VisibleAnywhere)
	bool CameraToggle = false;

	UPROPERTY()
	int32 AttackIndex = 0;
};
