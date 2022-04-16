// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPG3D_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	UCharacterAnimInstance();

	UFUNCTION()
	void AnimNotify_PlaySwingSound();

	UFUNCTION()
	void AnimNotify_ActivateCollision();

	UFUNCTION()
	void AnimNotify_DeactivateCollision();

	UFUNCTION()
	void AnimNotify_DeathEnd();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	FName GetAttackMontageName(int32 SectionIndex);

public:
	void PlayAttackMontage();
	void PlayDeathMontage();
	void JumpToSection(int32 SectionIndex);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float Horizontal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float Vertical;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
	class APawn* Pawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
	class AActionRPGCharacter* Main;
};
