// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "CharacterAnimInstance.h"

// Sets default values
AActionRPGCharacter::AActionRPGCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = 500.f;
	SpringArm->SetRelativeRotation(FRotator(-35.f, 0.f, 0.f));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/ParagonGreystone/Characters/Heroes/Greystone/Skins/Novaborn/Meshes/Greystone_Novaborn.Greystone_Novaborn'"));
	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
	}


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

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AActionRPGCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AActionRPGCharacter::Attack);
}

void AActionRPGCharacter::Attack()
{
	if (IsAttacking)
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
	UpDownValue = Value;
	AddMovementInput(GetActorForwardVector(), Value);
}

void AActionRPGCharacter::LeftRight(float Value)
{
	LeftRightValue = Value;
	AddMovementInput(GetActorRightVector(), Value);
}

void AActionRPGCharacter::Yaw(float Value)
{
	AddControllerYawInput(Value);
}
