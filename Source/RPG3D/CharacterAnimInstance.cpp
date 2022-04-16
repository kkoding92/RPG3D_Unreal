// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ActionRPGCharacter.h"

UCharacterAnimInstance::UCharacterAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/Animation/CharacterAnimMontage.CharacterAnimMontage'"));
	if (AM.Succeeded())
	{
		AttackMontage = AM.Object;
	}
}

void UCharacterAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UCharacterAnimInstance::PlayDeathMontage()
{
	Montage_Play(AttackMontage, 1.0f);
	Montage_JumpToSection(FName("Death"));
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Main = Cast<AActionRPGCharacter>(Pawn);
		}
	}
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (IsValid(Pawn))
	{
		Speed = Pawn->GetVelocity().Size();

		Main = Cast<AActionRPGCharacter>(Pawn);
		if (IsValid(Main))
		{
			IsFalling = Main->GetMovementComponent()->IsFalling();
		}
	}
}

void UCharacterAnimInstance::AnimNotify_PlaySwingSound()
{
	if (IsValid(Pawn))
	{

		Main = Cast<AActionRPGCharacter>(Pawn);
		if (IsValid(Main))
			Main->PlaySwingSound();
	}
}

void UCharacterAnimInstance::AnimNotify_ActivateCollision()
{
	if (IsValid(Pawn))
	{

		Main = Cast<AActionRPGCharacter>(Pawn);
		if (IsValid(Main))
			Main->ActivateCollision();
	}
}

void UCharacterAnimInstance::AnimNotify_DeactivateCollision()
{
	if (IsValid(Pawn))
	{

		Main = Cast<AActionRPGCharacter>(Pawn);
		if (IsValid(Main))
			Main->DeactivateCollision();
	}
}

void UCharacterAnimInstance::JumpToSection(int32 SectionIndex)
{
	FName Name = GetAttackMontageName(SectionIndex);
	Montage_JumpToSection(Name, AttackMontage);
}

FName UCharacterAnimInstance::GetAttackMontageName(int32 SectionIndex)
{
	return FName(*FString::Printf(TEXT("Attack%d"), SectionIndex));
}