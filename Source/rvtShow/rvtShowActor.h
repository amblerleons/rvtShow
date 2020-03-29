// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "rvtShowActor.generated.h"

//class UProceduralMeshComponent;
//class FProcMeshTangent;
//三维顶点
struct tpoint
{
	int X;
	int Y;
	int Z;
};
//三角形
struct Triangle
{
	TArray<FVector> TriangulatePts;
};
struct Triganlelst
{
	TArray<Triangle> Triangulate;
};
struct Face
{
	TArray<Triganlelst> Triangs;
};
struct ParamItem
{
	FString Name;
	FString Value;
};
struct JsonItem
{
	FString Category;
	FString	Name;
	FString Id;
	int32 BInteract;
	int32 BShowParames;	 
	FColor Meshcolor;
	FString MeshTexture;
	TArray<ParamItem> Parameters;
	TArray<FVector> Faces;
	TArray<float> UVs;
};

UCLASS()
class RVTSHOW_API ArvtShowActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ArvtShowActor();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//每一个jsonitem对应一个网格
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<UProceduralMeshComponent*> thisMeshs;
protected:
	void ReadRevitJsonData(FString& strjson);
	TArray<JsonItem> curJsonItems;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* ThisScene;
	//UPROPERTY(VisibleAnywhere)
		//UProceduralMeshComponent* ThisMesh;

	

	TArray<JsonItem> myJsonData;
	//virtual void PostActorCreated() override;
	//virtual void PostLoad() override;
	void GenerateMesh();

private:
	/*TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FVector2D> UVs;
	TArray<FLinearColor>Colors;*/
	void AddTriangleMesh(FVector TopLeft, FVector BottomLeft, FVector BottomRight, int32& TriIndex, FProcMeshTangent Tangent);
};
