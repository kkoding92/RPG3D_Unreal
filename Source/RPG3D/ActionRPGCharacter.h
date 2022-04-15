// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ActionRPGCharacter.generated.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UpDown(float Value);
	void LeftRight(float Value);
	void Yaw(float Value);
	void Attack();

	UFUNCTION()
	float GetUpDownValue() { return UpDownValue; }

	UFUNCTION()
	float GetLeftRightValue() { return LeftRightValue; }

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY()
	class UCharacterAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere)
	bool IsAttacking = false;

	UPROPERTY()
	int32 AttackIndex = 0;

	UPROPERTY()
	float UpDownValue = 0;

	UPROPERTY()
	float LeftRightValue = 0;
};
