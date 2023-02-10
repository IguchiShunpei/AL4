#include "GameScene.h"
#include <cassert>
#include "Collision.h"
#include <sstream>
#include <iomanip>

using namespace DirectX;

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete spriteBG;
	delete object3d;
	delete objSphere;
	delete objGround;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);

	this->dxCommon = dxCommon;
	this->input = input;

	// デバッグテキスト用テクスチャ読み込み
	Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	Sprite::LoadTexture(1, L"Resources/background.png");

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });

	//OBJからモデルデータを読み込む
	modelSphere = Model::LoadFromOBJ("sphere");
	modelGround = Model::LoadFromOBJ("ground");

	object3d = Object3d::Create();
	object3d->Update();

	// 3Dオブジェクト生成
	objSphere = Object3d::Create();
	objGround = Object3d::Create();

	//オブジェクトにモデルを紐付ける
	objSphere->SetModel(modelSphere);
	objGround->SetModel(modelGround);

	objSphere->SetPosition(XMFLOAT3(0, 0, 100));
	objGround->SetPosition(XMFLOAT3(0, -20, 0));
	
	//球の初期値を設定
	sphere.center = XMVectorSet(0, 2, 0, 1); //中心座標
	sphere.radius = 1.0f; //半径

	//平面の初期値を設定
	plane.normal = XMVectorSet(0, 0.5f, 0, 0); //法線ベクトル
	plane.distance = 0.0f; //原点(0,0,0)からの距離

}

void GameScene::Update() {
	// オブジェクト移動
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT)) {
		// 現在の座標を取得
		XMFLOAT3 position = object3d->GetPosition();
		XMVECTOR moveY = XMVectorSet(0, 0.01f, 0, 0);

		// 移動後の座標を計算
		if (input->PushKey(DIK_UP)) 
		{
			position.y += 1.0f; 
			sphere.center += moveY;
		}
		else if (input->PushKey(DIK_DOWN)) 
		{ 
			position.y -= 1.0f; 
			sphere.center = moveY;
		}
		if (input->PushKey(DIK_RIGHT)) { position.x += 1.0f; }
		else if (input->PushKey(DIK_LEFT)) { position.x -= 1.0f; }

		// 座標の変更を反映
		object3d->SetPosition(position);
	}

	// カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A)) {
		if (input->PushKey(DIK_W)) { Object3d::CameraMoveVector({ 0.0f,+1.0f,0.0f }); }
		else if (input->PushKey(DIK_S)) { Object3d::CameraMoveVector({ 0.0f,-1.0f,0.0f }); }
		if (input->PushKey(DIK_D)) { Object3d::CameraMoveVector({ +1.0f,0.0f,0.0f }); }
		else if (input->PushKey(DIK_A)) { Object3d::CameraMoveVector({ -1.0f,0.0f,0.0f }); }
	}

	//球の移動
	{
		XMFLOAT3 position = objSphere->GetPosition();

		XMVECTOR moveY = XMVectorSet(0, 0.01f, 0, 0);

		if (isMove == false) {
			sphere.center += moveY;
			position.y += 1.0f;
			if (position.y >= 30) {
				isMove = true;
			}
		}
		if (isMove == true) {
			sphere.center -= moveY;
			position.y -= 1.0f;
			if (position.y <= -10) {
				isMove = false;
			}
		}

		/*XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
		if (input->PushKey(DIK_RIGHT)) {
			sphere.center += moveX;
			position.x += 1.0f;
		}
		else if (input->PushKey(DIK_LEFT)) {
			sphere.center -= moveY;
			position.x -= 1.0f;
		}*/

		objSphere->SetPosition(position);
	}
	//stringstreamで変数の値を埋め込んで整形する
	std::ostringstream spherestr;
	spherestr << "Sphere:("
		<< std::fixed << std::setprecision(2)	//小数点以下2桁まで
		<< sphere.center.m128_f32[0] << ","		//X
		<< sphere.center.m128_f32[1] << ","		//Y
		<< sphere.center.m128_f32[2] << ")";	//Z
	debugText.Print(spherestr.str(), 50, 180, 1.0f);

	//球と平面の当たり判定
	bool hit = Collision::CheckSphere2Plane(sphere, plane);
	if ((hit)) {
		isHit = true;
		debugText.Print("HIT", 50, 200, 1.0f);
		spherestr.str("");
		spherestr.clear();
		spherestr << "("
			<< std::fixed << std::setprecision(2)	//小数点以下2桁まで
			<< sphere.center.m128_f32[0] << ","		//X
			<< sphere.center.m128_f32[1] << ","		//Y
			<< sphere.center.m128_f32[2] << ")";	//Z
		debugText.Print(spherestr.str(), 50, 220, 1.0f);
	}

	if (isHit == true) {
		modelSphere->SetColor(DirectX::XMFLOAT3(1, 0, 0));
		isHit = false;
	}
	else {
		modelSphere->SetColor(DirectX::XMFLOAT3(1, 1, 1));
	}
	object3d->Update();
	objSphere->Update();
	objGround->Update();
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	//spriteBG->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(cmdList);

	// 3Dオブクジェクトの描画
	object3d->Draw();
	objSphere->Draw();
	objGround->Draw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText.DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}