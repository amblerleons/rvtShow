// Fill out your copyright notice in the Description page of Project Settings.


#include "rvtShowActor.h"
#include "Json//Public/Json.h"
#include "JsonUtilities/public/JsonUtilities.h"
#include "stdlib.h"

void ArvtShowActor::ReadRevitJsonData(FString& strjson)
{
	TSharedRef< TJsonReader<TCHAR> > rvtJsonReader = TJsonReaderFactory<TCHAR>::Create(strjson);
	TSharedPtr<FJsonObject> rRoot;
	bool breadRes = FJsonSerializer::Deserialize(rvtJsonReader, rRoot);
	if (!breadRes)
	{
		return;
	}
	//TArray<JsonItem> myJsonData;
	//成功得到json对象
	const TArray< TSharedPtr<FJsonValue> >& myElements = rRoot->GetArrayField("Elems");
	for (int ki = 0; ki < myElements.Num(); ++ki)
	{
		TSharedPtr<FJsonObject> objki = myElements[ki]->AsObject();
		JsonItem tmpItem;
		tmpItem.Category = objki->GetStringField("Category");
		tmpItem.Name = objki->GetStringField("Name");
		tmpItem.Id = objki->GetStringField("Id");
		tmpItem.BInteract = objki->GetIntegerField("BInteract");
		tmpItem.BShowParames = objki->GetIntegerField("BShowParames");
		//const TArray< TSharedPtr<FJsonValue> >& myColor= objki->GetArrayField("Meshcolor");
		//if(myColor.)
		//tmpItem.Meshcolor += FColor(myColor[0]->AsNumber(), myColor[1]->AsNumber(), myColor[2]->AsNumber(), myColor[3]->AsNumber());
		const TArray< TSharedPtr<FJsonValue> >& myParameters = objki->GetArrayField("Parameters");
		for (int kj = 0; kj < myParameters.Num(); ++kj)
		{
			TSharedPtr<FJsonObject> objkj = myParameters[kj]->AsObject();
			ParamItem myParaItem;
			myParaItem.Name = objkj->GetStringField("Name");
			myParaItem.Value = objkj->GetStringField("Value");
			tmpItem.Parameters.Add(myParaItem);
		}
		//解析网格面
		//TArray<Face> tmpFaces;
		const TArray< TSharedPtr<FJsonValue> >& myFaces = objki->GetArrayField("Faces");
		for (int c1 = 0; c1 < myFaces.Num(); c1=c1+3)
		{
			FVector tmppoint(myFaces[c1]->AsNumber(), myFaces[c1+1]->AsNumber(), myFaces[c1+2]->AsNumber());
			tmpItem.Faces.Add(tmppoint);
		}
		myJsonData.Add(tmpItem);
	}

}
// Sets default values

ArvtShowActor::ArvtShowActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	ThisScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = ThisScene;
	FString jsonstr;
	FFileHelper::LoadFileToString(jsonstr, TEXT("D:/work/unreal/studyData/RevitCloudPlatformData.json"));
	ReadRevitJsonData(jsonstr);
	GenerateMesh();
}

// Called when the game starts or when spawned
void ArvtShowActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ArvtShowActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ArvtShowActor::GenerateMesh()
{
	int32 triangleIndex = 0;
	//由于在创建网格的时候必须要顶点，索引，法线，切线，uv坐标，颜色
	for (int i = 0; i < myJsonData.Num(); ++i)
	{
		int32 vertexIndex = 0;
		TArray<FVector> Vertices;
		TArray<int32> vertexIndexs;
		TArray<FVector> Normals;
		TArray<FProcMeshTangent> Tangents;
		TArray<FVector2D> UVs;
		TArray<FLinearColor>Colors;
		Vertices.Reset();
		vertexIndexs.Reset();
		Normals.Reset();
		Tangents.Reset();
		UVs.Reset();
		Colors.Reset();
		JsonItem curItem = myJsonData[i];
		for (int j = 0; j < curItem.Faces.Num(); j=j+3)
		{			
			//顶点数据
			FVector p1(curItem.Faces[j].Y / 10, curItem.Faces[j].X / 10, curItem.Faces[j].Z / 10);
			FVector p2(curItem.Faces[j+1].Y / 10, curItem.Faces[j + 1].X / 10, curItem.Faces[j + 1].Z / 10);
			FVector p3(curItem.Faces[j + 2].Y / 10, curItem.Faces[j + 2].X / 10, curItem.Faces[j + 2].Z / 10);
			Vertices.Add(p1);
			Vertices.Add(p2);
			Vertices.Add(p3);
			//索引数据
			vertexIndexs.Add(vertexIndex++);
			vertexIndexs.Add(vertexIndex++);
			vertexIndexs.Add(vertexIndex++);
			//法线数据
			FVector ThisNoraml = FVector::CrossProduct(p3 - p2, p2 - p1).GetSafeNormal();
			Normals.Add(ThisNoraml);
			Normals.Add(ThisNoraml);
			Normals.Add(ThisNoraml);
			//切线数据
			FVector tangentV = (p3 - p2).GetSafeNormal();
			FProcMeshTangent TangentSetup(tangentV.X, tangentV.Y, tangentV.Z);
			Tangents.Add(TangentSetup);
			Tangents.Add(TangentSetup);
			Tangents.Add(TangentSetup);
			//纹理坐标数据
			//UVs.Add(FVector2D(0.0f, 0.0f));
			//UVs.Add(FVector2D(0.0f, 0.0f));
			//UVs.Add(FVector2D(0.0f, 0.0f));
			//颜色坐标数据信息
			Colors.Add(FLinearColor::Transparent);
			Colors.Add(FLinearColor::Transparent);
			Colors.Add(FLinearColor::Transparent);			
		}
		FString strname = "GenerateMesh" + FString::FromInt(triangleIndex);
		UProceduralMeshComponent* ThisMesh = CreateDefaultSubobject<UProceduralMeshComponent>(FName(*strname));
		ThisMesh->SetupAttachment(RootComponent);
		ThisMesh->CreateMeshSection_LinearColor(triangleIndex, Vertices, vertexIndexs, Normals, UVs, Colors, Tangents, true);
		thisMeshs.Add(ThisMesh);
		triangleIndex++;
	}

	
}
void ArvtShowActor::AddTriangleMesh(FVector TopLeft, FVector BottomLeft, FVector BottomRight, int32& TriIndex, FProcMeshTangent Tangent)
{
	/*int32 point1 = TriIndex++;
	int32 point2 = TriIndex++;
	int32 point3 = TriIndex++;

	Vertices.Add(TopLeft);
	Vertices.Add(BottomLeft);
	Vertices.Add(BottomRight);

	Triangles.Add(point1);
	Triangles.Add(point2);
	Triangles.Add(point3);


	FVector ThisNoraml = FVector::CrossProduct(BottomLeft - TopLeft, BottomRight - BottomLeft).GetSafeNormal();
	for (int i = 0; i < 3; ++i)
	{
		Normals.Add(ThisNoraml);
		Tangents.Add(Tangent);
		Colors.Add(FLinearColor::Green);
	}
	UVs.Add(FVector2D(0.0f, 1.0f));
	UVs.Add(FVector2D(0.0f, 0.0f));
	UVs.Add(FVector2D(1.0f, 0.0f));*/
}