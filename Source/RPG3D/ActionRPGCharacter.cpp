// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "CharacterAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainPlayerController.h"
#include "Sound/SoundCue.h"
#include "Kismet/GamePlayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"

// Sets default values
AActionRPGCharacter::AActionRPGCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/ParagonGreystone/Characters/Heroes/Greystone/Skins/Novaborn/Meshes/Greystone_Novaborn.Greystone_Novaborn'"));
	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->TargetArmLength = 600.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; //Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // ...at this rotation rate

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("SwordTopSocket"));

	MovementStatus = EMovementStatus::EMS_Normal;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;

	MaxHealth = 100.f;
	Health = 100.f;
	Damage = 25.f;
}

// Called when the game starts or when spawned
void AActionRPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());

	AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->OnMontageEnded.AddDynamic(this, &AActionRPGCharacter::OnAttackMontageEnded);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AActionRPGCharacter::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AActionRPGCharacter::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	SetInstigator(GetController());
}

// Called every frame
void AActionRPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetTargetLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

// Called to bind functionality to input
void AActionRPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AActionRPGCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AActionRPGCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &AActionRPGCharacter::Yaw);
	PlayerInputComponent->BindAxis(TEXT("Pitch"), this, &AActionRPGCharacter::Pitch);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AActionRPGCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &AActionRPGCharacter::StopJumping);
	PlayerInputComponent->BindAction(TEXT("LMB"), EInputEvent::IE_Pressed, this, &AActionRPGCharacter::LMBDown);
	PlayerInputComponent->BindAction(TEXT("LMB"), EInputEvent::IE_Pressed, this, &AActionRPGCharacter::LMBUp);
	PlayerInputComponent->BindAction(TEXT("CameraToggle"), EInputEvent::IE_Pressed, this, &AActionRPGCharacter::CameraMoveOn);
	PlayerInputComponent->BindAction(TEXT("CameraToggle"), EInputEvent::IE_Released, this, &AActionRPGCharacter::CameraMoveOff);
}

void AActionRPGCharacter::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			UParticleSystem* PlayerHitParticles = Enemy->GetHitParticles();
			USoundCue* PlayerHitSound = Enemy->GetHitSound();

			if (PlayerHitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName("FX_Sword_Top");
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlayerHitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (PlayerHitSound)
			{
				UGameplayStatics::PlaySound2D(this, PlayerHitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}
}

void AActionRPGCharacter::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AActionRPGCharacter::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (!GetMovementComponent()->IsFalling())
		Attack();
}

void AActionRPGCharacter::LMBUp()
{
	bLMBDown = false;
}

void AActionRPGCharacter::Attack()
{
	if (IsAttacking)
		return;

	SetInterpToEnemy(true);

	AnimInstance->PlayAttackMontage();
	AnimInstance->JumpToSection(AttackIndex);

	AttackIndex = (AttackIndex + 1) % 3;
	IsAttacking = true;
}

void AActionRPGCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		IsAttacking = false;
		SetInterpToEnemy(false);
		if (bLMBDown)
		{
			Attack();
		}
	}
	else
	{
		GetMesh()->bPauseAnims = true;
		GetMesh()->bNoSkeletonUpdate = true;
	}
}

void AActionRPGCharacter::UpDown(float Value)
{
	if (IsAttacking)
		return;

	if (Controller != nullptr && Value != 0.0f && MovementStatus != EMovementStatus::EMS_Dead)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AActionRPGCharacter::LeftRight(float Value)
{
	if (IsAttacking)
		return;

	if (Controller != nullptr && Value != 0.0f && MovementStatus != EMovementStatus::EMS_Dead)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AActionRPGCharacter::Yaw(float Value)
{
	AddControllerYawInput(Value);
}

void AActionRPGCharacter::Pitch(float Value)
{
	AddControllerPitchInput(Value);
}

void AActionRPGCharacter::CameraMoveOn()
{
	CameraToggle = true;
}
void AActionRPGCharacter::CameraMoveOff()
{
	CameraToggle = false;
}

void AActionRPGCharacter::Jump()
{
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();
	}
}

void AActionRPGCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

void AActionRPGCharacter::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0)
	{
		Health -= Amount;
		Die();
	}
	else
	{
		Health -= Amount;
	}
}

float AActionRPGCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0)
	{
		Health -= DamageAmount;
		Die();

		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->SetHasValidTarget(false);
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AActionRPGCharacter::PlaySwingSound()
{
	if (SwingSound)
		UGameplayStatics::PlaySound2D(this, SwingSound);
}

FRotator AActionRPGCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

	return LookAtRotationYaw;
}

void AActionRPGCharacter::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	AnimInstance->PlayDeathMontage();
	MovementStatus = EMovementStatus::EMS_Dead;
}

void AActionRPGCharacter::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AActionRPGCharacter::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}