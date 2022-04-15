// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	MovementStatus = EMovementStatus::EMS_Normal;
}

// Called when the game starts or when spawned
void AActionRPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->OnMontageEnded.AddDynamic(this, &AActionRPGCharacter::OnAttackMontageEnded);
}

// Called every frame
void AActionRPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AActionRPGCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("CameraToggle"), EInputEvent::IE_Pressed, this, &AActionRPGCharacter::CameraMoveOn);
	PlayerInputComponent->BindAction(TEXT("CameraToggle"), EInputEvent::IE_Released, this, &AActionRPGCharacter::CameraMoveOff);
}

void AActionRPGCharacter::Attack()
{
	if (IsAttacking || GetMovementComponent()->IsFalling() || MovementStatus != EMovementStatus::EMS_Dead)
		return;

	AnimInstance->PlayAttackMontage();
	AnimInstance->JumpToSection(AttackIndex);

	AttackIndex = (AttackIndex + 1) % 3;
	IsAttacking = true;
}

void AActionRPGCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
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