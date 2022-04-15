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

void UCharacterAnimInstance::AnimNotify_AttackHit()
{
	UE_LOG(LogTemp, Log, TEXT("Anim Notify : AttackHit"));
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn))
	{
		Speed = Pawn->GetVelocity().Size();

		auto Character = Cast<AActionRPGCharacter>(Pawn);
		if (IsValid(Character))
		{
			IsFalling = Character->GetMovementComponent()->IsFalling();

			Vertical = Character->GetUpDownValue();
			Horizontal = Character->GetLeftRightValue();
		}
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