// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"
#include "ActionRPGCharacter.h"

UEnemyAnimInstance::UEnemyAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/Enemies/Spider/SpiderCombatMontage.SpiderCombatMontage'"));
	if (AM.Succeeded())
	{
		CombatMontage = AM.Object;
	}
}

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Enemy = Cast<AEnemy>(Pawn);
		}
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Enemy = Cast<AEnemy>(Pawn);
		}
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralsSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralsSpeed.Size();
	}
}

void UEnemyAnimInstance::AnimNotify_EndAttack()
{
	if (Pawn)
	{
		Enemy = Cast<AEnemy>(Pawn);

		if (IsValid(Enemy) && !Enemy->GetOverlappingCombatSphere())
		{
			AActionRPGCharacter* Target = Enemy->GetCombatTarget();
			if (IsValid(Target))
				Enemy->MoveToTarget(Target);
		}
	}
}

void UEnemyAnimInstance::AnimNotify_ActivateCollision()
{
	if (Pawn)
	{
		Enemy = Cast<AEnemy>(Pawn);
		if(IsValid(Enemy))
			Enemy->ActivateCollision();
	}
}

void UEnemyAnimInstance::AnimNotify_DeactivateCollision()
{
	if (Pawn)
	{
		Enemy = Cast<AEnemy>(Pawn);
		if (IsValid(Enemy))
			Enemy->DeactivateCollision();
	}
}

void UEnemyAnimInstance::AnimNotify_AttackEnd()
{
	if (Pawn)
	{
		Enemy = Cast<AEnemy>(Pawn);
		if (IsValid(Enemy))
			Enemy->AttackEnd();
	}
}

void UEnemyAnimInstance::AnimNotify_DeathEnd()
{
	if (Pawn)
	{
		Enemy = Cast<AEnemy>(Pawn);
		if (IsValid(Enemy))
			Enemy->DeathEnd();
	}
}

void UEnemyAnimInstance::PlayAttackMontage()
{
	Montage_Play(CombatMontage, 1.35f);
	Montage_JumpToSection(FName("Attack"), CombatMontage);
}
void UEnemyAnimInstance::PlayDeathMontage()
{
	Montage_Play(CombatMontage, 1.0f);
	Montage_JumpToSection(FName("Death"), CombatMontage);
}
