#include "WindowApp.h"
#include "GameSystem.h"
#include"Camera.h"
#include"Easing.h"

const float radian = XM_PI / 180.0f;//ラジアン

//カメラの情報（消すな）
static struct CAMERA
{
	Camera* info = nullptr;
	XMFLOAT3 Position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 Target = { 0.0f,0.0f,0.0f };

	//カメラのイージング用
	XMFLOAT3 beforePosition = { 0.0f,0.0f,0.0f };
	XMFLOAT3 beforeTarget = { 0.0f,0.0f,0.0f };
	XMFLOAT3 afterPosition = { 0.0f,0.0f,0.0f };
	XMFLOAT3 afterTarget = { 0.0f,0.0f,0.0f };
}camera;

//プレイヤーの情報
static struct PLAYER
{
	bool isAlive = true;
	XMFLOAT3 position = { 0.0f, 50.0f, 0.0f };
	float size = 10.0f;
	float radius = 20.0f;
}player;

//球の情報
const int bulletNum = 80;
static struct BULLET
{
	Obj* info = nullptr;
	bool isAlive = false;
	XMFLOAT3 position = { 0.0f, -50.0f, 0.0f };
	XMFLOAT3 movement = { 0.0f,0.0f, 0.0f };
	XMFLOAT3 rota = { 0.0f,0.0f, 0.0f };
	float size = 1.0f;
	float radius = 2.6f;
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

}bullet[bulletNum];

//チュートリアルの敵の情報
const int tutorialEnemyNum = 10;
static struct TUTORIAL_ENEMY
{
	Object3d* info = nullptr;
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 size = { 3.0f, 3.0f, 3.0f };
	XMFLOAT4 color = { 1.0f, 0.0f, 0.0f, 1.0f };//色(赤)
	float radius = 8.0f;//ボスパーツの半径
	bool isAlive = false;
	bool isDamageColor = false;
	int damageTime = 0;
}tutorialEnemy[tutorialEnemyNum];

//ボスの情報
//顔パーツの個数
const int bossFacePartNum = 343;
//体パーツの個数
const int bossBodyPartNum = 231;
//右足パーツの個数
const int bossRightFootPartNum = 48;
//左足パーツの個数
const int bossLeftFootPartNum = 48;
//右腕パーツの個数
const int bossRightArmPartNum = 90;
//左腕パーツの個数
const int bossLeftArmPartNum = 90;
//合計
const int bossPartNum = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum +
bossLeftFootPartNum + bossRightArmPartNum + bossLeftArmPartNum;
static struct BOSS_PART
{
	Object3d* info = nullptr;
	bool isAlive = false;
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };//位置
	XMFLOAT3 rota = { 0.0f, 270.0f, 0.0f };//回転
	XMFLOAT3 size = { 3.0f, 3.0f, 3.0f };//サイズ	
	float radius = 15.0f;//ボスパーツの半径
	XMFLOAT4 color = { 1.0f, 0.0f, 0.0f, 1.0f };//色(赤)
	bool isAttack = false;

	//イージング用の引数
	float angle = 0;//円形に回す為の角度
	float distance = 0;//中心からの距離
	float beforeDistance = 0;//イージングの終点用
	XMFLOAT3 beforePosition = { 0.0f,0.0f,0.0f };//イージングの始点用
	XMFLOAT3 afterPosition = { 0.0f,0.0f,0.0f };//イージングの終点用

	//部位破壊時使用
	int isBreak = 0;
}boss[bossPartNum];

const int attackRangeNum = 8;
static struct ATTACK_RANGE
{
	Obj* info = nullptr;
	bool isAlive = false;
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };//位置
	XMFLOAT3 rota = { 0.0f, 0.0f, 0.0f };//回転
	float size = 1.0f;//サイズ
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };//色(赤)

}attackRange[attackRangeNum];

//ボスの初期化（ブロックの個数分ある変数）
void InitBoss(BOSS_PART* boss, float* timeRateAttack, float* nowCountAttack, bool* bossIsShot, bool* fallPlace)
{
	//ボスパーツの情報
	for (int i = 0; i < bossPartNum; i++)
	{
		boss[i].isAlive = false;
		boss[i].position = {};//位置
		boss[i].rota = { 0.0f, 270.0f, 0.0f };//回転
		boss[i].size = { 3.0f, 3.0f, 3.0f };//サイズ	
		boss[i].radius = 15.0f;//ボスパーツの半径
		boss[i].color = { 1.0f, 0.0f, 0.0f, 1.0f };//色(赤)
		boss[i].isAttack = false;

		//イージング用の引数
		boss[i].angle = 0;//円形に回す為の角度
		boss[i].distance = 0;//中心からの距離
		boss[i].beforeDistance = 0;//イージングの終点用
		boss[i].beforePosition = { 0.0f,0.0f,0.0f };//イージングの始点用
		boss[i].afterPosition = { 0.0f,0.0f,0.0f };//イージングの終点用

		//部位破壊時使用
		boss[i].isBreak = 0;

		timeRateAttack[i] = 0;
		nowCountAttack[i] = 0;
		bossIsShot[i] = false;

		fallPlace[i] = false;
	}
};

//移動操作
bool PlayerOperationMove(PLAYER& player)
{
	Input* input = Input::GetInstance();

	bool isMove = false;

	//player移動
	float Pspeed = 5.0f;
	//右入力
	if (input->LEFT_PadStickX() == +1 || input->PushKey(DIK_D) == true) {
		player.position.x += Pspeed * cos((camera.Target.x + 90) * radian);
		player.position.z += Pspeed * sin((camera.Target.x + 90) * radian);
		isMove = true;
	}
	//左入力
	if (input->LEFT_PadStickX() == -1 || input->PushKey(DIK_A) == true) {
		player.position.x -= Pspeed * cos((camera.Target.x + 90) * radian);
		player.position.z -= Pspeed * sin((camera.Target.x + 90) * radian);
		isMove = true;
	}
	//下入力
	if (input->LEFT_PadStickY() == +1 || input->PushKey(DIK_W) == true) {
		player.position.x -= Pspeed * cos(camera.Target.x * radian);
		player.position.z -= Pspeed * sin(camera.Target.x * radian);
		isMove = true;
	}
	//上入力
	if (input->LEFT_PadStickY() == -1 || input->PushKey(DIK_S) == true) {
		player.position.x += Pspeed * cos(camera.Target.x * radian);
		player.position.z += Pspeed * sin(camera.Target.x * radian);
		isMove = true;
	}

	//プレイヤーの行動できる範囲を設定。それ以上進めないようにする
	const float playerPosXMax = 900.0f;
	const float playerPosXMin = -900.0f;
	const float playerPosZMax = 900.0f;
	const float playerPosZMin = -900.0f;

	if (player.position.x > playerPosXMax) {
		player.position.x = playerPosXMax;
	}
	if (player.position.x < playerPosXMin) {
		player.position.x = playerPosXMin;
	}
	if (player.position.z > playerPosZMax) {
		player.position.z = playerPosZMax;
	}
	if (player.position.z < playerPosZMin) {
		player.position.z = playerPosZMin;
	}

	return isMove;
};

//視界操作
void PlayerOperationEye(CAMERA& camera, const float Tgspeed)
{
	Input* input = Input::GetInstance();

	//視界移動
	if (input->RIGHT_PadStickX() == +1 || input->PushKey(DIK_RIGHT) == true) { camera.Target.x -= Tgspeed; }//右入力
	if (input->RIGHT_PadStickX() == -1 || input->PushKey(DIK_LEFT) == true) { camera.Target.x += Tgspeed; }//左入力
	if (input->RIGHT_PadStickY() == +1 || input->PushKey(DIK_UP) == true) { camera.Target.y += Tgspeed; }//上入力
	if (input->RIGHT_PadStickY() == -1 || input->PushKey(DIK_DOWN) == true) { camera.Target.y -= Tgspeed; }//下入力

	//上下方向の角度制限
	if (camera.Target.y < 85) { camera.Target.y = 86; }//下制限
	if (camera.Target.y > 251) { camera.Target.y = 250; }//上制限

	//横の制限
	if (camera.Target.x > 360) { camera.Target.x = 0; }
	if (camera.Target.x < -360) { camera.Target.x = 0; }
};

//プレイヤーとボスパーツの当たり判定
bool CollisionPlayerBossPartCheck(PLAYER player, BOSS_PART bossPart)
{
	//ボスパーツが生きているなら
	if (bossPart.isAlive == true)
	{
		//当たり判定
		float lenght;
		//自機と敵の距離を計算
		lenght = (player.position.x - bossPart.position.x) * (player.position.x - bossPart.position.x)
			+ (player.position.y - bossPart.position.y) * (player.position.y - bossPart.position.y)
			+ (player.position.z - bossPart.position.z) * (player.position.z - bossPart.position.z);

		//自機と敵が衝突していた場合
		if (lenght <= (player.radius + bossPart.radius) * (player.radius + bossPart.radius))
		{
			return true;
		}
	}

	return false;
};

//弾とボスパーツの当たり判定
bool CollisionBulletBossPartCheck(BULLET bullet, BOSS_PART bossPart)
{
	//弾が発射状態なら＆ボスパーツが生きているなら
	if (bullet.isAlive == true && bossPart.isAlive == true)
	{
		//当たり判定
		float lenght;
		//自機と敵の距離を計算
		lenght = (bullet.position.x - bossPart.position.x) * (bullet.position.x - bossPart.position.x)
			+ (bullet.position.y - bossPart.position.y) * (bullet.position.y - bossPart.position.y)
			+ (bullet.position.z - bossPart.position.z) * (bullet.position.z - bossPart.position.z);

		//自機と敵が衝突していた場合
		if (lenght <= (bullet.radius + bossPart.radius) * (bullet.radius + bossPart.radius))
		{
			return true;
		}
	}

	return false;
};

//弾とチュートリアル用エネミーの当たり判定
bool CollisionPlayerTutorialEnemyCheck(BULLET bullet, TUTORIAL_ENEMY tutorialEnemy)
{
	//弾が発射状態なら＆チュートリアル用の敵が生きているなら
	if (bullet.isAlive == true && tutorialEnemy.isAlive == true)
	{
		//当たり判定
		float lenght;
		//自機と敵の距離を計算
		lenght = (bullet.position.x - tutorialEnemy.position.x) * (bullet.position.x - tutorialEnemy.position.x)
			+ (bullet.position.y - tutorialEnemy.position.y) * (bullet.position.y - tutorialEnemy.position.y)
			+ (bullet.position.z - tutorialEnemy.position.z) * (bullet.position.z - tutorialEnemy.position.z);

		//自機と敵が衝突していた場合
		if (lenght <= (player.radius + tutorialEnemy.radius) * (player.radius + tutorialEnemy.radius))
		{
			return true;
		}
	}

	return false;
};

//プレイヤーの攻撃準備処理
int BulletPreparation(const int bulletNum, BULLET& bullet, const PLAYER& player, XMFLOAT3 target, int bulletCount)
{
	if (bullet.isAlive == false)
	{
		//プレイヤーの位置に球をセット
		bullet.position = player.position;

		const float bulletSpeed = 30.0f;

		//移動方向をセット
		bullet.movement = { cos(target.x * radian) * -bulletSpeed ,
			sin(target.y * radian) * -bulletSpeed ,sin(target.x * radian) * -bulletSpeed };

		//向きをセット
		bullet.rota = { cos(target.x * radian) , sin(target.y * radian),0 };

		//表示をtrueに
		bullet.isAlive = true;

		//次番にする
		bulletCount = bulletCount + 1;

		//使用番号が合計個数より上なら0に直す
		if (bulletCount >= bulletNum)
		{
			bulletCount = 0;
		}
	}

	return bulletCount;
};

//プレイヤーの攻撃更新処理
void UpDateBullet(BULLET& bullet)
{
	//発射されているなら移動
	if (bullet.isAlive == true)
	{
		bullet.position.x += bullet.movement.x;
		bullet.position.y += bullet.movement.y;
		bullet.position.z += bullet.movement.z;
	}

	else if (bullet.isAlive == false)
	{
		bullet.position = { -1000, -1000, -1000 };
	}

	//一定範囲外なら消す
	int max = 1500;
	if (bullet.position.x > max || bullet.position.x < -max ||
		bullet.position.y > max || bullet.position.y < -max ||
		bullet.position.z > max || bullet.position.z < -max)
	{
		bullet.isAlive = false;
		bullet.position = { -100, -100, -100 };
	}
};


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウサイズ
	const int window_width = 1280;// 横幅
	const int window_height = 720;// 縦幅

	//ゲーム名
	const wchar_t* gameName = L"ブロック崩しの逆襲";


	//ウィンドウ初期化
	WindowApp* winApp = nullptr;
	winApp = new WindowApp();
	winApp->winStart(window_width, window_height, gameName);

	//全体の初期化
	GameSystem* gameSystem = nullptr;
	gameSystem = new GameSystem();
	gameSystem->init(winApp, window_width, window_height, gameName);

	//key
	Input* input = Input::GetInstance();
	input->init(winApp);

	//カメラ
	camera.info = new Camera();
	camera.info->init(winApp);

	//-------------------------------------------------------------------------------------------------//

	//引数宣言
	//3Dオブジェクト生成

	//画像読み込み
	Object3d::LoadTexture(winApp, 0, L"Resources/enemy.png");

	//ボス
	for (int i = 0; i < bossPartNum; i++)
	{
		boss[i].info = new Object3d();
		boss[i].info->Create(0);
	}

	//チュートリアルの的
	for (int i = 0; i < tutorialEnemyNum; i++)
	{
		tutorialEnemy[i].info = new Object3d();
		tutorialEnemy[i].info->Create(0);
	}

	//-------------------------------------------------------------------------------------------------//

	//obj生成
	enum OBJ
	{
		Bullet,
		Wall,
		attack
	};

	Obj::LoadObj(Bullet, "Bullet");//プレイヤーの攻撃
	Obj::LoadObj(Wall, "wall");//地面
	Obj::LoadObj(attack, "attack");//攻撃範囲

	//地面
	Obj* wall = nullptr;
	wall = new Obj();
	wall->CreateModel(Wall);

	//プレイヤー球
	for (int i = 0; i < bulletNum; i++)
	{
		bullet[i].info = new Obj();
		bullet[i].info->CreateModel(Bullet);
	}

	//ボスの攻撃範囲
	for (int i = 0; i < attackRangeNum; i++)
	{
		attackRange[i].info = new Obj();
		attackRange[i].info->CreateModel(attack);
	}
	//-------------------------------------------------------------------------------------------------//

	//スプライト生成
	const int spriteNum = 35;

	//スプライトの初期化
	Sprite* sprite[spriteNum] = { nullptr };

	for (int i = 0; i < spriteNum; i++)
	{
		sprite[i] = new Sprite();
	}

	//スプライトの生成
	{
		sprite[0]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //タイトルロゴ
		sprite[1]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.0f }); //タイトル画面のAorB
		sprite[2]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //操作説明1(パッドの絵1)
		sprite[3]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //操作説明2(パッドの絵2)
		sprite[4]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //操作説明3(パッドの絵3)
		sprite[5]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.0f }); //操作誘導1
		sprite[6]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.0f }); //操作誘導2
		sprite[7]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.0f }); //操作誘導3
		sprite[8]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.0f }); //操作誘導4
		sprite[9]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //「タイトルへ戻る」
		sprite[10]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //「チュートリアル・歩く」用ゲージ枠
		sprite[11]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //「チュートリアル・歩く」用ゲージ
		sprite[12]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //「チュートリアル・倒す」用カウント0
		sprite[13]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //「チュートリアル・倒す」用カウント1
		sprite[14]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //「チュートリアル・倒す」用カウント2
		sprite[15]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //「チュートリアル・倒す」用カウント3
		sprite[16]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //「チュートリアル・倒す」用カウント4
		sprite[17]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //「チュートリアル・倒す」用カウント5
		sprite[18]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //ボスのHP用ゲージ枠
		sprite[19]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //ボスのHP用ゲージ
		sprite[20]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //HPハート
		sprite[21]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //HPハート
		sprite[22]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //HPハート
		sprite[23]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //HPハート
		sprite[24]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //HPハート
		sprite[25]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //ハート枠
		sprite[26]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //ハート枠
		sprite[27]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //ハート枠
		sprite[28]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //ハート枠
		sprite[29]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //ハート枠
		sprite[30]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //レティクル
		sprite[31]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //ダメージ受けた時に赤くなる用
		sprite[32]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //YOUDIED
		sprite[33]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //背景を暗く
		sprite[34]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //YOUWIN
	}

	//スプライト番号に名前を付ける
	enum SpriteName
	{
		Title = 1,
		TitleBunki,
		Pad1,
		Pad2,
		Pad3,
		Step1,
		Step2,
		Step3,
		Step4,
		TitleBack,
		MoveGaugeOut,
		MoveGaugeIn,
		Count0,
		Count1,
		Count2,
		Count3,
		Count4,
		Count5,
		BossGaugeOut,
		BossGaugeIn,
		Heart,
		HeartEnpty,
		Reticle,
		DamageRed,
		YOUDIED,
		BrackBackGround,
		YOUWIN
	};

	//スプライト用のテクスチャ読み込み/texNumは1から
	{
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 1, L"Resources/title.png");	//タイトルロゴ
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 2, L"Resources/titleBunki.png");//タイトル画面のAorB
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 3, L"Resources/pad1.png");		//操作説明1
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 4, L"Resources/pad2.png");		//操作説明2
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 5, L"Resources/pad3.png");		//操作説明3
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 6, L"Resources/step1.png");	//操作誘導1
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 7, L"Resources/step2.png");	//操作誘導2
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 8, L"Resources/step3.png");	//操作誘導3
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 9, L"Resources/step4.png");	//操作誘導4
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 10, L"Resources/titleBack.png");//「タイトルへ戻る」
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 11, L"Resources/moveGaugeOut.png");//「チュートリアル・歩く」用ゲージ枠
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 12, L"Resources/moveGaugeIn.png");//「チュートリアル・歩く」用ゲージ
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 13, L"Resources/count0.png");//「チュートリアル・倒す」用カウント0
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 14, L"Resources/count1.png");//「チュートリアル・倒す」用カウント1
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 15, L"Resources/count2.png");//「チュートリアル・倒す」用カウント2
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 16, L"Resources/count3.png");//「チュートリアル・倒す」用カウント3
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 17, L"Resources/count4.png");//「チュートリアル・倒す」用カウント4
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 18, L"Resources/count5.png");//「チュートリアル・倒す」用カウント5
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 19, L"Resources/bossHPGaugeOut.png");//ボスのHP用ゲージ枠
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 20, L"Resources/bossHPGaugeIn.png");//ボスのHP用ゲージ
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 21, L"Resources/heart.png");//HPハート
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 22, L"Resources/heartEnpty.png");//ハート枠
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 23, L"Resources/reticle.png");//レティクル
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 24, L"Resources/playerDamageRed.png");//ダメージ受けた時に赤くなる用
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 25, L"Resources/youdied.png");//YOU DIED
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 26, L"Resources/blackBackGround.png");//背景を暗く
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 27, L"Resources/youwin.png");//YOUWIN
	}

	//デバッグテキスト
	DebugText text;
	text.init(winApp, gameSystem->getdXCommon());

	//-------------------------------------------------------------------------------------------------//
	//サウンド用
	ComPtr<IXAudio2>xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);//XAudioエンジンのインスタンス生成
	xAudio2->CreateMasteringVoice(&masterVoice);//マスターボイス生成
	masterVoice->SetVolume(0.1f);//全体の音量
	Audio* audio = nullptr;
	audio = new Audio();
	//-------------------------------------------------------------------------------------------------//

	//-------------wav読み込み----------------//
	SoundData BuckSound = audio->SoundLoadWave(xAudio2.Get(), "Resources/Sound/BuckSound.wav");//BGM
	SoundData BossDamage = audio->SoundLoadWave(xAudio2.Get(), "Resources/Sound/BossDamage.wav");//ボスのdamage音
	SoundData PlayerDamage = audio->SoundLoadWave(xAudio2.Get(), "Resources/Sound/PlayerDamage.wav");//プレイヤーのdamage音
	SoundData Shot = audio->SoundLoadWave(xAudio2.Get(), "Resources/Sound/Shot.wav");//撃つ
	SoundData Button = audio->SoundLoadWave(xAudio2.Get(), "Resources/Sound/Button.wav");//button効果音

	//---------------変数宣言--------------//
	//システム用変数
	float length = 0.0f;

	//イージングの進行度用
	float nowCount = 0.0f;
	float timeRate = 0.0f;

	//カメラの引数
	float cameraSpeed = 1;//カメラの速度

	//ゲームシーン
	enum SCENE {
		Init,
		Titel,
		Tutorial,
		StartMovie,
		Battle,
		Lose,
		Win
	};
	int scene = Titel;

	//各シーンの中で実行する処理の順番
	//使ったシーンの最後に必ず0にする処理を書く
	int step = 0;

	//チュートリアル用
	enum TutorialScene
	{
		TUT_Init,//イージング用のポジションセット
		TUT_PlayerEyeMove,//viewをプレイヤー視点にイージングで変化させる
		TUT_TryMove,//移動させる
		TUT_TryRedEnemy,//元々赤の敵を攻撃させる
		TUT_TryYellowEnemy,//黄色の敵をを赤にして倒せるようにする
		TUT_Undo//好きに行動させ、満足したらタイトルへ
	};
	int tutorialScene = TutorialScene::TUT_Init;
	int moveChack = 0;//どのくらい動いたか
	const int moveCheckMax = 100;//何歩でゲージMAXになるのか
	float moveGaugeSizeX = 0.0f;//「チュートリアル・歩く」用ゲージの大きさ
	const float moveGaugeSizeXMax = 526.0f;//「チュートリアル・歩く」用ゲージの大きさMAX
	int checkTutorialEnemy = 0;//チュートリアル用の敵倒した数


	//ムービー中はボスへのダメージ判定無し
	bool nowMovie = true;

	//player
	const float Tgspeed = 1.0f;

	const int playerHPMax = 5;//プレイヤーの最大HP
	int playerHP = playerHPMax;//プレイヤーのHP
	bool isPlayerDamage = false;//プレイヤーがダメージを食らっているか
	int playerDamageTime = 0;	//プレイヤーがダメージを食らってからの時間
	const int playerDamageTimeMax = 180;//無敵時間
	bool isPlayerDamageRedShow = false;//画面を赤くするか
	const int playerDamageRedShowTime = 60;//画面を赤くする時間


	//playerBullet
	const float bulletSize = 1;
	int bulletCount = 0;

	//ボス初期ムービー用
	enum BossMovieScene
	{
		BM_Init,//準備
		BM_CenterMove,//中心へイージング
		BM_CreateHumanModel,//人型になる
		BM_EyeMove//視界移動
	};
	int bossMovieScene = BossMovieScene::BM_Init;
	const float  maxTime = 300.0f;//全体時間
	const int afterY = 300;

	//ボスムービー時のカメラ
	const XMFLOAT3 MovieCameraPos = { 1000,300,0 };
	const XMFLOAT3 MovieCameratarget = { 0,180,0 };
	const float easeAfterTargetY = 180;

	//ボス
	const int maxArmAttackCount = 100;
	int armAttackCount = 0;
	int howBossBody = 5;
	int oldHowbossbody = 5;
	int bossAiRand = 0;
	int bossBulletCount = 0;
	int bossAttackCoolTime = 0;
	float timeRateAttack[850] = { 0 };
	float nowCountAttack[850] = { 0 };
	bool bossIsShot[850] = { false };
	bool moveAi = false;
	bool isBossAttack = false;

	//ボスの攻撃パターン
	enum BossAttack
	{
		NONE,
		SlidePart,//近距離攻撃1
		machineGun,//近距離攻撃2
		LongDisAttack1,//長距離攻撃1
		LongDisAttack2,//長距離攻撃2
		RightArmBreak,//右腕破壊
		LeftArmBreak,//左腕破壊
		RightLegBreak,//右足破壊
		LeftLegBreak//左足破壊
	};
	int howAttack = NONE;

	//右腕攻撃のシーン(近距離攻撃1)
	int machineGunScene = 0;

	//両足で薙ぎ払う攻撃のシーン(近距離攻撃2)
	int slidePartScene = 0;

	//長距離攻撃1のシーン
	int longDisAttack1Scene = 0;

	//長距離攻撃2のシーン
	int longDisAttack2Scene = 0;
	float partSpeedX = 0.0f;
	float partSpeedZ = 0.0f;

	//右腕破壊時のシーン
	enum Boss_RA_Broken
	{
		RA_BreakSet,
		RA_BreakUpdate,
		RA_Init,
		RA_Move,
		RA_Stay1,
		RA_Fry,
		RA_Stay2,
		RA_Fall,
		RA_Stay3,
		RA_Back,
		RA_End
	};

	//左腕破壊時シーン
	enum Boss_LA_Broken
	{
		LA_BreakSet,//部位が落ちるセット
		LA_BreakUpdate,//部位が落ちる更新
		LA_Init,//イージングの準備
		LA_UpMove,//上へ移動する
		LA_AttackBigPartFall,//上から一定個数落とす
		LA_Undo,//イージングで元の位置に戻す
	};

	//右足破壊時シーン
	enum Boss_RF_Broken
	{
		RF_BreakSet,//部位が落ちるセット
		RF_BreakUpdate,//部位が落ちる更新
		RF_Init,//イージングの準備
		RF_InFrontPart,//x座標のマイナス方向へイージング
		RF_ReturnEye,//視界を戻す
		RF_AttackBuckPart,//x座標のマイナス方向からプラス方向へ飛ばす攻撃
		RF_Undo,//イージングで元の位置に戻す
	};

	//左足破壊時シーン
	enum Boss_LF_Broken
	{
		LF_BreakSet,//部位が落ちるセット
		LF_BreakUpdate,//部位が落ちる更新
		LF_Init,//イージングの準備
		LF_CenterMove,//中心へイージング
		LF_ReturnEye,//視界を戻す
		LF_AttackCircleMove,//円周状に跳ばす
		LF_Undo,//イージングで元の位置に戻す
	};

	/// <summary>
	/// 部位破壊時のシーン代入引数/
	/// 0->右腕/1->右腕/2->右足/3->左足
	/// </summary>
	int partBreakScene[4] = {
		Boss_RA_Broken::RA_BreakSet ,Boss_LA_Broken::LA_BreakSet,Boss_RF_Broken::RF_BreakSet,Boss_LF_Broken::LF_BreakSet };


	//落ちる場所をプレイヤーの座標にするため距離を出す
	XMFLOAT3 fallPosDistance = { 0.0f, 0.0f, 0.0f };

	//-左腕破壊時に使用する-//
	//降らせるパーツの個数
	const int fallNum = 8;
	//場所セット
	bool fallPlace[bossPartNum] = { false };

	//-左足破壊時に使用する-//
	const int rotationNum = 20;//回転させる個数
	int pushCount = rotationNum;//発射する球の配列の位置

	//ボスパーツの直径
	const float bossPartDiameter = 30.0f;

	//ボスのパーツ初期位置
	XMFLOAT3 bossPartFirstPosition[bossPartNum];
	for (int i = 0; i < bossPartNum; i++)
	{
		//頭
		if (i < bossFacePartNum)
		{
			bossPartFirstPosition[i].x = -220.0f - (i % 49 / 7) * bossPartDiameter;
			bossPartFirstPosition[i].y = 760.0f - (i / 49) * bossPartDiameter;
			bossPartFirstPosition[i].z = -90.0f + (i % 7) * bossPartDiameter;
		}

		//胴体
		else if (i < bossFacePartNum + bossBodyPartNum)
		{
			//前のパーツ番号を引く用変数
			int partNumDiff = bossFacePartNum;

			bossPartFirstPosition[i].x = -280.0f - ((i - partNumDiff) % 21 / 7) * bossPartDiameter;
			bossPartFirstPosition[i].y = 550.0f - ((i - partNumDiff) / 21) * bossPartDiameter;
			bossPartFirstPosition[i].z = -90.0f + ((i - partNumDiff) % 7) * bossPartDiameter;
		}

		//右足
		else if (i < bossFacePartNum + bossBodyPartNum + bossRightFootPartNum)
		{
			//前のパーツ番号を引く用変数
			int partNumDiff = bossFacePartNum + bossBodyPartNum;

			bossPartFirstPosition[i].x = -280.0f - ((i - partNumDiff) % 6 / 2) * bossPartDiameter;
			bossPartFirstPosition[i].y = 220.0f - ((i - partNumDiff) / 6) * bossPartDiameter;
			bossPartFirstPosition[i].z = -90.0f + ((i - partNumDiff) % 2) * bossPartDiameter;
		}

		//左足
		else if (i < bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum)
		{
			//前のパーツ番号を引く用変数
			int partNumDiff = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum;

			bossPartFirstPosition[i].x = -280.0f - ((i - partNumDiff) % 6 / 2) * bossPartDiameter;
			bossPartFirstPosition[i].y = 220.0f - ((i - partNumDiff) / 6) * bossPartDiameter;
			bossPartFirstPosition[i].z = 60.0f + ((i - partNumDiff) % 2) * bossPartDiameter;
		}

		//右腕
		else if (i < bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum)
		{
			//前のパーツ番号を引く用変数
			int partNumDiff = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum;

			bossPartFirstPosition[i].x = -280.0f - ((i - partNumDiff) % 30 / 10) * bossPartDiameter;
			bossPartFirstPosition[i].y = 550.0f - ((i - partNumDiff) / 30) * bossPartDiameter;
			bossPartFirstPosition[i].z = -120.0f - ((i - partNumDiff) % 10) * bossPartDiameter;
		}

		//左腕
		else if (i < bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum + bossLeftArmPartNum)
		{
			//前のパーツ番号を引く用変数
			int partNumDiff = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum;

			bossPartFirstPosition[i].x = -280.0f - ((i - partNumDiff) % 30 / 10) * bossPartDiameter;
			bossPartFirstPosition[i].y = 550.0f - ((i - partNumDiff) / 30) * bossPartDiameter;
			bossPartFirstPosition[i].z = 120.0f + ((i - partNumDiff) % 10) * bossPartDiameter;
		}
	}

	//ボスパーツの色
	const XMFLOAT4 baseColor = { 1.0f, 0.0f, 0.0f, 1.0f };//通常時の色(赤)
	const XMFLOAT4 attackColor = { 1.0f, 1.0f, 0.0f, 1.0f };//攻撃時の色(黄色)
	const XMFLOAT4 damageColor = { 1.0f, 0.5f, 0.5f, 1.0f };//ダメージを食らったときの色(薄い赤)


	//ボスが生きているか
	bool isBossAlive = false;

	const int bossHPMax = 400;//ボスの最大HP
	int bossHP = bossHPMax;//ボスのHP
	const float bossHPGaugeSizeXMax = 694.0f;//ボスのHP用ゲージの大きさMAX
	float bossHPGaugeSizeX = bossHPGaugeSizeXMax;//ボスのHP用ゲージの大きさ 
	bool isBossDamageColor = false;//ボスをダメージを受けた時の色にするか
	int bossDamageTime = 0;//ボスがダメージを受けてから何秒経ったか
	const int bossDamageColorTime = 5;//ボスがダメージを受けた時の色にする時間

	const int tutorialDamageColorTime = 20;//ボスがダメージを受けた時の色にする時間

	//敗北シーン
	int loseScene = 0;

	//勝利シーン
	int winScene = 0;

	//スプライト関係
	XMFLOAT4 youDiedColor = { 1.0f, 1.0f, 1.0f, 0.0f };//YOUDIEDをじわじわ出すための変数
	XMFLOAT4 youWinColor = { 1.0f, 1.0f, 1.0f, 0.0f };//YOUWINをじわじわ出すための変数

	//-------------------------------------------------------------------------------------------------//

	gameSystem->afterInit(winApp);

	while (true)
	{
		input->upDate();
		//エスケープか×が押されたときゲーム終了
		if (input->PushKey(DIK_ESCAPE)) { break; }
		if (gameSystem->gameFin(winApp) == true) { break; }

		//システムアップデート
		gameSystem->upDate(winApp);

		//-------------------更新処理------------------//
		// 初期化
		if (scene == SCENE::Init)
		{
			//なにが無いのか分からなくなってきたから上から全部初期化しとくわ
			//constついてないもの以外全部

			//カメラの情報
			camera.Position = { 0.0f,0.0f,0.0f };
			camera.Target = { 0.0f,0.0f,0.0f };

			//カメラのイージング用
			camera.beforePosition = { 0.0f,0.0f,0.0f };
			camera.beforeTarget = { 0.0f,0.0f,0.0f };
			camera.afterPosition = { 0.0f,0.0f,0.0f };
			camera.afterTarget = { 0.0f,0.0f,0.0f };

			//プレイヤーの情報
			player.isAlive = true;
			player.position = { 0.0f, 50.0f, 0.0f };
			player.size = 10.0f;
			player.radius = 20.0f;

			//弾の情報
			for (int i = 0; i < bulletNum; i++)
			{
				bullet[i].isAlive = false;
				bullet[i].position = { 0.0f, -50.0f, 0.0f };
				bullet[i].movement = { 0.0f,0.0f, 0.0f };
				bullet[i].rota = { 0.0f,0.0f, 0.0f };
				bullet[i].size = 1.0f;
				bullet[i].radius = 2.6f;
			}

			//チュートリアル用の敵の情報
			for (int i = 0; i < tutorialEnemyNum; i++)
			{
				tutorialEnemy[i].position = { 0.0f,0.0f,0.0f };
				tutorialEnemy[i].size = { 3.0f, 3.0f, 3.0f };
				tutorialEnemy[i].color = { 1.0f, 0.0f, 0.0f, 1.0f };//色(赤)
				tutorialEnemy[i].radius = 8.0f;//ボスパーツの半径
				tutorialEnemy[i].isAlive = false;
				tutorialEnemy[i].isDamageColor = false;
				tutorialEnemy[i].damageTime = 0;
			}

			//システム用変数
			length = 0.0f;

			//イージングの進行度用
			nowCount = 0.0f;
			timeRate = 0.0f;

			//カメラの引数
			cameraSpeed = 1;//カメラの速度

			//チュートリアル用
			tutorialScene = TutorialScene::TUT_Init;
			moveChack = 0;//どのくらい動いたか
			moveGaugeSizeX = 0.0f;//「チュートリアル・歩く」用ゲージの大きさ
			checkTutorialEnemy = 0;//チュートリアル用の敵倒した数

			//ムービー中はボスへのダメージ判定無し
			nowMovie = true;

			//player
			playerHP = playerHPMax;//プレイヤーのHP
			isPlayerDamage = false;//プレイヤーがダメージを食らっているか
			playerDamageTime = 0;	//プレイヤーがダメージを食らってからの時間
			isPlayerDamageRedShow = false;//画面を赤くするか

			//playerBullet
			bulletCount = 0;

			//ボス初期ムービー用
			bossMovieScene = BossMovieScene::BM_Init;

			//ボス
			InitBoss(boss, timeRateAttack, nowCountAttack, bossIsShot, fallPlace);

			armAttackCount = 0;
			howBossBody = 5;
			oldHowbossbody = 5;
			bossAiRand = 0;
			bossBulletCount = 0;
			bossAttackCoolTime = 0;
			moveAi = false;
			isBossAttack = false;

			//ボスの攻撃パターン
			howAttack = NONE;

			//右腕攻撃のシーン(近距離攻撃1)
			machineGunScene = 0;

			//両足で薙ぎ払う攻撃のシーン(近距離攻撃2)
			slidePartScene = 0;

			//長距離攻撃1のシーン
			longDisAttack1Scene = 0;

			//長距離攻撃2のシーン
			longDisAttack2Scene = 0;
			partSpeedX = 0.0f;
			partSpeedZ = 0.0f;

			partBreakScene[0] = Boss_RA_Broken::RA_BreakSet;
			partBreakScene[1] = Boss_LA_Broken::LA_BreakSet;
			partBreakScene[2] = Boss_RF_Broken::RF_BreakSet;
			partBreakScene[3] = Boss_LF_Broken::LF_BreakSet;


			//落ちる場所をプレイヤーの座標にするため距離を出す
			fallPosDistance = { 0.0f, 0.0f, 0.0f };

			//-左足破壊時に使用する-//
			pushCount = rotationNum;//発射する球の配列の位置

			//ボスが生きているか
			isBossAlive = false;

			bossHP = bossHPMax;//ボスのHP
			bossHPGaugeSizeX = bossHPGaugeSizeXMax;//ボスのHP用ゲージの大きさ 
			isBossDamageColor = false;//ボスをダメージを受けた時の色にするか
			bossDamageTime = 0;//ボスがダメージを受けてから何秒経ったか

			for (int i = 0; i < attackRangeNum; i++)
			{
				attackRange[i].isAlive = false;
				attackRange[i].position = { 0.0f,0.0f,0.0f };
				attackRange[i].size = 1.0f;
			}

			//敗北シーン
			loseScene = 0;

			//勝利シーン
			winScene = 0;

			//スプライト関係
			youDiedColor = { 1.0f, 1.0f, 1.0f, 0.0f };//YOUDIEDをじわじわ出すための変数
			youWinColor = { 1.0f, 1.0f, 1.0f, 0.0f };//YOUWINをじわじわ出すための変数

			//タイトルにシーン変更
			scene = Title;
		}

		//タイトル
		else if (scene == SCENE::Titel)
		{
			//ボスムービー時のカメラにする
			camera.Position = MovieCameraPos;
			camera.Target = MovieCameratarget;
			cameraSpeed = 1;
			//チュートリアルで使った変数の初期化
			player.position = { 400.0f, 50.0f, 0.0f };

			//チュートリアル
			if (input->TriggerPadButtons(Input::PAD_A) || input->TriggerKey(DIK_RETURN) == true)
			{
				//ボタン効果音
				audio->SoundPlayWava(xAudio2.Get(), Button, false);
				scene = SCENE::Tutorial;
				nowCount = 0;
			}

			//ボス戦
			if (input->TriggerPadButtons(Input::PAD_B) || input->TriggerKey(DIK_SPACE) == true)
			{
				//ボタン効果音
				audio->SoundPlayWava(xAudio2.Get(), Button, false);
				//BGMループ
				audio->SoundPlayWava(xAudio2.Get(), BuckSound, true);
				scene = SCENE::StartMovie;
				nowCount = 0;
				isBossAlive = true;
				for (int i = 0; i < bossPartNum; i++)
				{
					boss[i].isAlive = true;
				}
			}
		}

		//チュートリアル
		else if (scene == SCENE::Tutorial)
		{
			//イージング用のポジションセット
			if (tutorialScene == TutorialScene::TUT_Init)
			{
				camera.beforePosition = camera.Position;
				camera.beforeTarget = camera.Target;

				tutorialScene++;
			}

			//viewをプレイヤー視点にイージングで変化させる
			else if (tutorialScene == TutorialScene::TUT_PlayerEyeMove)
			{
				timeRate = nowCount / maxTime;

				camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, player.position.x, timeRate));
				camera.Position.y = static_cast<float>(EASE::easeIn(camera.beforePosition.y, player.position.y, timeRate));
				camera.Position.z = static_cast<float>(EASE::easeIn(camera.beforePosition.z, player.position.z, timeRate));

				camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y / 2, 180, timeRate));

				//イージングのカウント
				nowCount++;

				//視界移動が終わったので次のstepへ
				if (nowCount > maxTime) {
					tutorialScene++;
					nowCount = 0;
					cameraSpeed = 5;
				}
			}

			//移動させる
			else if (tutorialScene == TutorialScene::TUT_TryMove)
			{
				//player移動
				moveChack += PlayerOperationMove(player);
				//視界移動
				PlayerOperationEye(camera, Tgspeed);

				///まずは動いてみましょう
				///パッドを表示させスティックでplayer移動、視界移動、と表示
				//「チュートリアル・歩く」用ゲージの大きさ
				moveGaugeSizeX = moveChack * moveGaugeSizeXMax / moveCheckMax;

				//ビューの処理
				{
					camera.Position = player.position;
				}


				//一定以上動いたら次に行く
				if (moveChack > 100) {
					float dist = 500;//プレイヤーとエネミーの距離

					//tutorialの的設置
					for (int i = 0; i < tutorialEnemyNum; i++)
					{
						tutorialEnemy[i].position.x = player.position.x + cos(i * 36 * radian) * dist;
						tutorialEnemy[i].position.y = 20;
						tutorialEnemy[i].position.z = player.position.z + sin(i * 36 * radian) * dist;
						tutorialEnemy[i].isAlive = true;
						//色変え（黄色追加）Iが奇数なら黄色で酔いのでは
						if (i % 2 == 1)
						{
							//黄色にする処理
							tutorialEnemy[i].color = attackColor;
						}
					}

					moveChack = 0;
					moveGaugeSizeX = 0.0f;
					tutorialScene++;
				}
			}

			//元々赤の敵を攻撃させる
			else if (tutorialScene == TutorialScene::TUT_TryRedEnemy)
			{
				//視界移動
				PlayerOperationEye(camera, Tgspeed);

				//ビューの処理
				{
					camera.Position = player.position;
				}

				//プレイヤーの攻撃入力
				if (input->TriggerPadButtons(Input::PAD_RB) || input->TriggerKey(DIK_SPACE) == true)
				{
					//攻撃時SE
					audio->SoundPlayWava(xAudio2.Get(), Shot, false);
					bulletCount = BulletPreparation(bulletNum, bullet[bulletCount], player, camera.Target, bulletCount);
				}

				//プレイヤーの攻撃更新
				for (int i = 0; i < bulletNum; i++)
				{
					UpDateBullet(bullet[i]);
				}

				//的と弾の当たり判定
				//赤い敵なら当たり判定を通す
				for (int j = 0; j < tutorialEnemyNum / 2; j++)
				{
					for (int i = 0; i < bulletNum; i++)
					{
						//当たっているか？
						bool isCollision = CollisionPlayerTutorialEnemyCheck(bullet[i], tutorialEnemy[j * 2]);

						//当たっていたら
						if (isCollision == true)
						{
							//敵に攻撃を与えたときのSE
							audio->SoundPlayWava(xAudio2.Get(), BossDamage, false);

							//弾は消す
							bullet[i].isAlive = false;
							tutorialEnemy[j * 2].isAlive = false;

							//チュートリアル用の敵倒した数を加算
							checkTutorialEnemy++;
						}
					}
				}

				//赤が全員倒せたら次に行く
				if (checkTutorialEnemy == 5)
				{
					//黄色を赤にする処理
					for (int j = 0; j < tutorialEnemyNum / 2; j++)
					{
						tutorialEnemy[(j * 2) + 1].color = baseColor;
					}

					tutorialScene++;
				}
			}

			//黄色の敵をを赤にして倒せるようにする
			else if (tutorialScene == TutorialScene::TUT_TryYellowEnemy)
			{
				//視界移動
				PlayerOperationEye(camera, Tgspeed);

				//ビューの処理
				{
					camera.Position = player.position;
				}

				//プレイヤーの攻撃入力
				if (input->TriggerPadButtons(Input::PAD_RB) || input->TriggerKey(DIK_SPACE) == true)
				{
					//攻撃時SE
					audio->SoundPlayWava(xAudio2.Get(), Shot, false);
					bulletCount = BulletPreparation(bulletNum, bullet[bulletCount], player, camera.Target, bulletCount);
				}

				//プレイヤーの攻撃更新
				for (int i = 0; i < bulletNum; i++)
				{
					UpDateBullet(bullet[i]);
				}

				//的と弾の当たり判定
				//赤い敵なら当たり判定を通す
				for (int j = 0; j < tutorialEnemyNum / 2; j++)
				{
					for (int i = 0; i < bulletNum; i++)
					{
						//当たっているか？
						bool isCollision = CollisionPlayerTutorialEnemyCheck(bullet[i], tutorialEnemy[(j * 2) + 1]);

						//当たっていたら
						if (isCollision == true)
						{
							//敵に攻撃を与えたときのSE
							audio->SoundPlayWava(xAudio2.Get(), BossDamage, false);

							//弾は消す
							bullet[i].isAlive = false;
							tutorialEnemy[(j * 2) + 1].isAlive = false;

							//チュートリアル用の敵倒した数を加算
							checkTutorialEnemy++;
						}
					}
				}

				//赤が全員倒せたら次に行く
				if (checkTutorialEnemy == 10)
				{
					tutorialScene++;
					float dist = 1000;//プレイヤーとエネミーの距離

					//tutorialの的設置
					for (int i = 0; i < tutorialEnemyNum; i++)
					{
						tutorialEnemy[i].position.x = cos(i * 36 * radian) * dist;
						tutorialEnemy[i].position.y = 150;
						tutorialEnemy[i].position.z = sin(i * 36 * radian) * dist;
						tutorialEnemy[i].isAlive = true;
					}
				}
			}

			//好きに行動させ、満足したらタイトルへ
			else if (tutorialScene == TutorialScene::TUT_Undo)
			{
				//player移動
				PlayerOperationMove(player);
				//視界移動
				PlayerOperationEye(camera, Tgspeed);

				//ビューの処理
				{
					camera.Position = player.position;
				}

				//プレイヤーの攻撃入力
				if (input->TriggerPadButtons(Input::PAD_RB) || input->TriggerKey(DIK_SPACE)  == true)
				{
					//攻撃時SE
					audio->SoundPlayWava(xAudio2.Get(), Shot, false);
					bulletCount = BulletPreparation(bulletNum, bullet[bulletCount], player, camera.Target, bulletCount);
				}

				//プレイヤーの攻撃更新
				for (int i = 0; i < bulletNum; i++)
				{
					UpDateBullet(bullet[i]);
				}

				//当たり判定
				for (int j = 0; j < tutorialEnemyNum; j++)
				{
					for (int i = 0; i < bulletNum; i++)
					{
						//当たっているか？
						bool isCollision = CollisionPlayerTutorialEnemyCheck(bullet[i], tutorialEnemy[j]);

						//当たっていたら
						if (isCollision == true)
						{
							//敵に攻撃を与えたときのSE
							audio->SoundPlayWava(xAudio2.Get(), BossDamage, false);

							//色変える
							if (tutorialEnemy[j].isDamageColor == false)
							{
								tutorialEnemy[j].isDamageColor = true;
							}

							//弾は消す
							bullet[i].isAlive = false;

							break;
						}
					}
				}

				for (int i = 0; i < tutorialEnemyNum; i++)
				{
					if (tutorialEnemy[i].isDamageColor == true)
					{
						//ボスがダメージを食らってからの時間を計算
						tutorialEnemy[i].damageTime++;

						//チュートリアル用の敵の色を変える
						tutorialEnemy[i].color = damageColor;



						//指定の時間まで進んだら元の色に戻す
						if (tutorialEnemy[i].damageTime > tutorialDamageColorTime)
						{
							tutorialEnemy[i].color = baseColor;

							tutorialEnemy[i].damageTime = 0;

							//色変え終了
							tutorialEnemy[i].isDamageColor = false;
						}
					}
				}

				//パッドBボタンを押したらタイトルへ
				if (input->TriggerPadButtons(Input::PAD_B) == true || input->TriggerKey(DIK_RETURN)  == true)
				{
					//ボタン効果音
					audio->SoundPlayWava(xAudio2.Get(), Button, false);

					for (int i = 0; i < bulletNum; i++)
					{
						bullet[i].isAlive = false;
					}

					for (int j = 0; j < tutorialEnemyNum; j++)
					{
						tutorialEnemy[j].isAlive = false;
					}
					//チュートリアル用の敵倒した数を0に戻す
					checkTutorialEnemy = 0;

					nowCount = 0;
					tutorialScene = TutorialScene::TUT_Init;
					scene = SCENE::Init;
				}
			}
		}

		//ボスムービー
		else if (scene == SCENE::StartMovie)
		{
			//準備
			if (bossMovieScene == BossMovieScene::BM_Init)
			{
				for (int i = 0; i < bossPartNum; i++)
				{
					boss[i].angle = static_cast<float>(rand() % 360);
					boss[i].distance = static_cast<float>((rand() % 3000) - 1500);
					boss[i].beforeDistance = boss[i].distance;
					boss[i].position.x = cos(boss[i].angle * radian) * boss[i].distance;
					boss[i].position.y = static_cast<float>((rand() % 1000) + 10);
					boss[i].position.z = sin(boss[i].angle * radian) * boss[i].distance;
					boss[i].beforePosition = boss[i].position;
				}

				bossMovieScene++;
			}

			//中央に集まる
			else if (bossMovieScene == BossMovieScene::BM_CenterMove)
			{
				timeRate = nowCount / maxTime;

				//中心に集まるようにイージング
				for (int i = 0; i < bossPartNum; i++)
				{
					boss[i].angle++;
					boss[i].distance = static_cast<float>(EASE::easeIn(boss[i].beforeDistance, 0, timeRate));

					//position反映
					boss[i].position.x = cos(boss[i].angle * radian) * boss[i].distance;
					boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, afterY, timeRate));
					boss[i].position.z = sin(boss[i].angle * radian) * boss[i].distance;
				}

				//イージングのカウント
				nowCount++;

				//中心に集まったので,次のイージングのために位置をセットして次に行く
				if (nowCount > maxTime) {
					for (int i = 0; i < bossPartNum; i++)
					{
						boss[i].beforePosition = boss[i].position;
					}

					bossMovieScene++;
					nowCount = 0;
				}
			}

			//人型になる
			else if (bossMovieScene == BossMovieScene::BM_CreateHumanModel)
			{
				timeRate = nowCount / maxTime;

				for (int i = 0; i < bossPartNum; i++)
				{
					//position反映
					boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
					boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
					boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
				}

				//イージングのカウント
				nowCount++;

				//カウントダウンがマックスならbattleに移る
				if (nowCount > maxTime) {
					bossMovieScene++;
					nowCount = 0;
					camera.beforePosition = { player.position.x ,camera.Position.y,player.position.z };
					camera.beforeTarget = camera.Target;
				}
			}

			//視界移動
			else if (bossMovieScene == BossMovieScene::BM_EyeMove)
			{
				timeRate = nowCount / maxTime;

				camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, player.position.x, timeRate));
				camera.Position.y = static_cast<float>(EASE::easeIn(camera.beforePosition.y / 2, player.position.y, timeRate));
				camera.Position.z = static_cast<float>(EASE::easeIn(camera.beforePosition.z, player.position.z, timeRate));

				camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y * 1.5, 180, timeRate));

				//イージングのカウント
				nowCount++;

				//カウントダウンがマックスならbattleに移る
				if (nowCount > maxTime) {
					scene = SCENE::Battle;
					bossMovieScene = BossMovieScene::BM_Init;
					cameraSpeed = 5;
					nowMovie = false;
				}
			}
		}

		//バトル
		else if (scene == SCENE::Battle)
		{
			//ムービーでなければプレイヤーは行動できる
			if (nowMovie == false)
			{
				//プレイヤー移動
				PlayerOperationMove(player);
				//プレイヤー視点移動
				PlayerOperationEye(camera, Tgspeed);

				//プレイヤーの攻撃入力
				if (input->TriggerPadButtons(Input::PAD_RB) || input->TriggerKey(DIK_SPACE) == true)
				{
					//攻撃時SE
					audio->SoundPlayWava(xAudio2.Get(), Shot, false);

					bulletCount = BulletPreparation(bulletNum, bullet[bulletCount], player, camera.Target, bulletCount);
				}

				//プレイヤーがダメージ後の無敵ではなければ当たり判定を作動
				if (isPlayerDamage == false)
				{
					//プレイヤーとボスの当たり判定
					for (int i = 0; i < bossPartNum; i++)
					{
						//当たっているか？
						bool isCollision = CollisionPlayerBossPartCheck(player, boss[i]);

						//当たっていたら
						if (isCollision == true)
						{
							//プレイヤーがdamageを受けたときのSE
							audio->SoundPlayWava(xAudio2.Get(), PlayerDamage, false);

							//プレイヤーにダメージ
							playerHP--;

							//プレイヤーがダメージを食らったら無敵時間へ
							isPlayerDamage = true;

							//プレイヤーがダメージを食らったら画面を赤くする
							isPlayerDamageRedShow = true;

							//多段ヒットしないようにループを抜ける
							break;
						}
					}
				}

				//プレイヤーのダメージ後の無敵時間計算
				else if (isPlayerDamage == true)
				{
					playerDamageTime++;

					//指定した時間が来たら
					if (playerDamageTime >= playerDamageRedShowTime)
					{
						//画面赤いの終了
						isPlayerDamageRedShow = false;
					}

					//無敵時間が最大までいったら
					if (playerDamageTime >= playerDamageTimeMax)
					{
						//無敵終了
						isPlayerDamage = false;

						//次の時のために初期化
						playerDamageTime = 0;
					}
				}

				//HPが0以下になるとプレイヤー死亡
				if (playerHP <= 0)
				{
					//0以下にならないように
					playerHP = 0;

					player.isAlive = false;
				}
			}
			//プレイヤーの攻撃更新
			for (int i = 0; i < bulletNum; i++)
			{
				UpDateBullet(bullet[i]);
			}

			//ボスが生きていたら
			if (isBossAlive == true)
			{
				//ボスと弾の当たり判定
				for (int j = 0; j < bossPartNum; j++)
				{
					//当たっているか？
					bool isCollision = false;
					for (int i = 0; i < bulletNum; i++)
					{
						isCollision = CollisionBulletBossPartCheck(bullet[i], boss[j]);
						//当たっていたら
						if (isCollision == true)
						{
							//弾は消す
							bullet[i].isAlive = false;

							//ボスパーツが攻撃中ではなければ
							if (boss[j].isAttack == false)
							{
								//敵に攻撃を与えたときのSE
								audio->SoundPlayWava(xAudio2.Get(), BossDamage, false);

								//敵にダメージ
								bossHP--;

								//ボスの色をダメージ受けた時の色に変更
								isBossDamageColor = true;

								//ダメージを食らってからの時間を0に戻す
								bossDamageTime = 0;
							}

							//多段ヒットしないようにループを抜ける
							break;
						}
					}

					//当たっていたら
					if (isCollision == true)
					{
						//多段ヒットしないようにループを抜ける
						break;
					}
				}

				if (isBossDamageColor == true)
				{
					//ボスがダメージを食らってからの時間を計算
					bossDamageTime++;

					for (int i = 0; i < bossPartNum; i++)
					{
						//生きている＆攻撃中ではないパーツの色を変える
						if (boss[i].isAlive == true && boss[i].isAttack == false)
						{
							boss[i].color = damageColor;
						}
					}

					//指定の時間まで進んだら元の色に戻す
					if (bossDamageTime > bossDamageColorTime)
					{
						for (int i = 0; i < bossPartNum; i++)
						{
							//生きている＆攻撃中ではないパーツの色を戻す
							if (boss[i].isAlive == true && boss[i].isAttack == false)
							{
								boss[i].color = baseColor;
							}
						}

						//色変え終了
						isBossDamageColor = false;
					}
				}

				//HPが0以下になるとボス死亡
				if (bossHP <= 0)
				{
					//0以下にならないように
					bossHP = 0;

					isBossAlive = false;
				}
			}

			//ボスのHP用ゲージの大きさ
			bossHPGaugeSizeX = bossHP * bossHPGaugeSizeXMax / bossHPMax;

			//部位破壊されるHPか調べるために現フレームのHP残量を記録する
			if (bossHP > bossHPMax / 5 * 4)
			{
				howBossBody = 5;
			} else if (bossHP > bossHPMax / 5 * 3)
			{
				howBossBody = 4;
			} else if (bossHP > bossHPMax / 5 * 2)
			{
				howBossBody = 3;
			} else if (bossHP > bossHPMax / 5 * 1)
			{
				howBossBody = 2;
			} else if (bossHP > 0)
			{
				howBossBody = 1;
			} else if (bossHP == 0)
			{
				howBossBody = 0;
			}

			//右腕が破壊されている
			if (oldHowbossbody == 5 && howBossBody == 4)
			{
				for (int i = 0; i < 850; i++)
				{
					boss[i].position = bossPartFirstPosition[i];
					boss[i].beforePosition = { 0,0,0 };
					boss[i].afterPosition = { 0,0,0 };

					//攻撃に使用したボスパーツの攻撃中フラグをおろす
					boss[i].isAttack = false;

					//ボスパーツの色を赤に戻す
					boss[i].color = baseColor;
				}
				howAttack = BossAttack::RightArmBreak;
				slidePartScene = 0;
				machineGunScene = 0;
				longDisAttack1Scene = 0;
				longDisAttack2Scene = 0;
			}
			//左腕が破壊されている
			else if (oldHowbossbody == 4 && howBossBody == 3)
			{
				for (int i = 0; i < 850; i++)
				{
					boss[i].position = bossPartFirstPosition[i];
					boss[i].beforePosition = { 0,0,0 };
					boss[i].afterPosition = { 0,0,0 };

					//攻撃に使用したボスパーツの攻撃中フラグをおろす
					boss[i].isAttack = false;

					//ボスパーツの色を赤に戻す
					boss[i].color = baseColor;
				}
				howAttack = BossAttack::LeftArmBreak;
				slidePartScene = 0;
				machineGunScene = 0;
				longDisAttack1Scene = 0;
				longDisAttack2Scene = 0;
			}
			//右足が破壊されている
			else if (oldHowbossbody == 3 && howBossBody == 2)
			{
				for (int i = 0; i < 850; i++)
				{
					boss[i].position = bossPartFirstPosition[i];
					boss[i].beforePosition = { 0,0,0 };
					boss[i].afterPosition = { 0,0,0 };

					//攻撃に使用したボスパーツの攻撃中フラグをおろす
					boss[i].isAttack = false;

					//ボスパーツの色を赤に戻す
					boss[i].color = baseColor;
				}
				howAttack = BossAttack::RightLegBreak;
				slidePartScene = 0;
				machineGunScene = 0;
				longDisAttack1Scene = 0;
				longDisAttack2Scene = 0;
			}
			//左足が破壊されている
			else if (oldHowbossbody == 2 && howBossBody == 1)
			{
				for (int i = 0; i < 850; i++)
				{
					boss[i].position = bossPartFirstPosition[i];
					boss[i].beforePosition = { 0,0,0 };
					boss[i].afterPosition = { 0,0,0 };

					//攻撃に使用したボスパーツの攻撃中フラグをおろす
					boss[i].isAttack = false;

					//ボスパーツの色を赤に戻す
					boss[i].color = baseColor;
				}
				howAttack = BossAttack::LeftLegBreak;
				slidePartScene = 0;
				machineGunScene = 0;
				longDisAttack1Scene = 0;
				longDisAttack2Scene = 0;
			}
			//全て破壊されている
			else if (oldHowbossbody == 1 && howBossBody == 0)
			{
				for (int i = 0; i < 850; i++)
				{
					boss[i].position = bossPartFirstPosition[i];
					boss[i].beforePosition = { 0,0,0 };
					boss[i].afterPosition = { 0,0,0 };

					//攻撃に使用したボスパーツの攻撃中フラグをおろす
					boss[i].isAttack = false;

					//ボスパーツの色を赤に戻す
					boss[i].color = baseColor;
				}

				slidePartScene = 0;
				machineGunScene = 0;
				longDisAttack1Scene = 0;
				longDisAttack2Scene = 0;
			}

			//oldにhowBossBodyを代入
			oldHowbossbody = howBossBody;

			//ボスとプレイヤーとの距離
			if (isBossAlive == true)
			{
				length = sqrtf((player.position.x - boss[25].position.x) * (player.position.x - boss[25].position.x) + (player.position.z - boss[25].position.z) * (player.position.z - boss[25].position.z));
			}


			//ボスの攻撃処理
			if (isBossAttack == true)
			{
				//行動パターン用乱数の生成
				moveAi = true;
				if (moveAi == true)
				{
					bossAiRand = rand() % 100;
				}
				//プレイヤーとボスとの距離が近いとき
				if (length < 500)
				{
					if (howBossBody >= 3)
					{
						if (bossAiRand < 30)
						{
							//長距離攻撃1
							howAttack = LongDisAttack1;
						} else if (bossAiRand < 60)
						{
							//長距離攻撃2
							howAttack = LongDisAttack2;
						} else if (bossAiRand < 80)
						{
							//近距離攻撃2
							howAttack = machineGun;
						} else
						{
							//近距離攻撃1
							howAttack = SlidePart;
						}
					} else if (howBossBody <= 2)
					{
						if (bossAiRand < 45)
						{
							//長距離攻撃2
							howAttack = LongDisAttack2;
						} else if (bossAiRand < 75)
						{
							//遠距離攻撃1
							howAttack = LongDisAttack1;
						} else
						{
							//近距離攻撃2
							howAttack = machineGun;
						}
					}
					moveAi = false;
					isBossAttack = false;
				}
				//プレイヤーとボスとの距離が遠いとき
				else
				{
					if (howBossBody >= 3)
					{
						if (bossAiRand < 35)
						{
							//近距離攻撃1
							howAttack = SlidePart;
						} else if (bossAiRand < 70)
						{
							//近距離攻撃2
							howAttack = machineGun;
						} else if (bossAiRand < 85)
						{
							//長距離攻撃1
							howAttack = LongDisAttack1;
						} else
						{
							//遠距離攻撃2
							howAttack = LongDisAttack2;
						}
					} else if (howBossBody <= 2)
					{
						if (bossAiRand < 25)
						{
							//遠距離攻撃1
							howAttack = LongDisAttack1;
						} else if (bossAiRand < 50)
						{
							//遠距離攻撃2
							howAttack = LongDisAttack2;
						} else
						{
							//近距離攻撃2
							howAttack = machineGun;
						}
					}
					moveAi = false;
					isBossAttack = false;
				}
			}

			if (howAttack == BossAttack::NONE)
			{
				bossAttackCoolTime++;
			}
			if (bossAttackCoolTime > 120)
			{
				isBossAttack = true;
				bossAttackCoolTime = 0;
			}

			//近距離攻撃1
			if (howAttack == BossAttack::SlidePart)
			{
				if (slidePartScene == 0)
				{
					nowCount = 0;
					slidePartScene = 1;
					for (int i = 574; i < 670; i++)
					{
						//イージング処理用の代入
						boss[i].beforePosition = boss[i].position;
						boss[i].afterPosition.x = 800.0f - ((i - 574) % 20) * bossPartDiameter;
						boss[i].afterPosition.y = 74.0f - ((i - 574) / 30) * bossPartDiameter;
						boss[i].afterPosition.z = -800.0f + ((i - 574) % 30 / 10) * bossPartDiameter;

						//攻撃に使用するボスパーツは攻撃中にしておく
						boss[i].isAttack = true;

						//ボスパーツの色を変更
						boss[i].color = attackColor;
					}
				} else if (slidePartScene == 1)
				{
					int countNum = 100;
					timeRate = nowCount / countNum;
					//両足からプレイヤーの右側に飛ばす
					for (int i = 574; i < 670; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//中心に集まったので次に行く
					if (nowCount > countNum)
					{
						//イージング処理用の代入
						for (int i = 574; i < 670; i++)
						{
							boss[i].beforePosition = boss[i].position;
							boss[i].afterPosition.x = boss[i].position.x;
							boss[i].afterPosition.y = boss[i].position.y;
							boss[i].afterPosition.z = boss[i].position.z + 1600;
						}
						nowCount = 0;
						slidePartScene = 2;
					}
				} else if (slidePartScene == 2)
				{
					int countNum = 100;
					timeRate = nowCount / countNum;
					//薙ぎ払う処理
					for (int i = 574; i < 670; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//中心に集まったので次に行く
					if (nowCount > countNum)
					{
						//イージング処理用の代入
						for (int i = 574; i < 670; i++)
						{
							boss[i].beforePosition = boss[i].position;
							boss[i].afterPosition = bossPartFirstPosition[i];
						}
						nowCount = 0;
						slidePartScene = 3;
					}
				} else if (slidePartScene == 3)
				{
					int countNum = 100;
					timeRate = nowCount / countNum;
					//最初の位置に帰ってくる
					for (int i = 574; i < 670; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//中心に集まったので次に行く
					if (nowCount > countNum)
					{
						for (int i = 574; i < 670; i++)
						{
							//攻撃に使用したボスパーツの攻撃中フラグをおろす
							boss[i].isAttack = false;

							//ボスパーツの色を赤に戻す
							boss[i].color = baseColor;
						}

						nowCount = 0;
						//攻撃をなしにする
						howAttack = NONE;
						//攻撃シーンを0に
						slidePartScene = 0;
					}
				}
			}

			//頭を乱射する攻撃
			if (howAttack == BossAttack::machineGun)
			{
				if (machineGunScene == 0)
				{
					nowCount = 0;
					machineGunScene = 1;
					//イージング処理用の代入
					for (int i = 0; i < 343; i++)
					{
						boss[i].beforePosition = boss[i].position;
						boss[i].afterPosition = boss[171].position;

						//攻撃に使用するボスパーツは攻撃中にしておく
						boss[i].isAttack = true;

						//ボスパーツの色を変更
						boss[i].color = attackColor;
					}
				} else if (machineGunScene == 1)
				{
					int countNum = 100;
					timeRate = nowCount / countNum;
					//右腕を一か所に集める処理
					for (int i = 0; i < 343; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//中心に集まったので次に行く
					if (nowCount > countNum)
					{
						//イージング処理用の代入
						for (int i = 0; i < 343; i++)
						{
							boss[i].beforePosition = boss[i].position;
							nowCountAttack[i] = 0;
							bossIsShot[i] = false;
						}
						machineGunScene = 2;
						nowCount = 0;
						bossBulletCount = 0;
					}
				} else if (machineGunScene == 2)
				{
					float attackCount = 100;
					//マシンガンのように飛ばす処理(若干のホーミングあり)
					if (bossBulletCount < 343)
					{
						bossBulletCount++;
						bossIsShot[bossBulletCount] = true;
						boss[bossBulletCount].afterPosition.x = player.position.x + (rand() % 600) - 300;
						boss[bossBulletCount].afterPosition.y = player.position.y + (rand() % 600) - 300;
						boss[bossBulletCount].afterPosition.z = player.position.z + (rand() % 600) - 300;
						for (int i = 0; i < 343; i++)
						{
							timeRateAttack[i] = 0;
						}
					}
					for (int i = 0; i < 343; i++)
					{
						if (bossIsShot[i] == true)
						{
							nowCountAttack[i]++;
							timeRateAttack[i] = nowCountAttack[i] / attackCount;
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRateAttack[i]));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRateAttack[i]));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRateAttack[i]));
							nowCount++;
						}
					}
					if (nowCountAttack[342] > 200)
					{
						//イージング処理用の代入
						for (int i = 0; i < 343; i++)
						{
							boss[i].beforePosition = boss[i].position;
							boss[i].afterPosition.x = boss[i].position.x;
							boss[i].afterPosition.y = 2000;
							boss[i].afterPosition.z = boss[i].position.z;
						}
						machineGunScene = 3;
						nowCount = 0;
					}
				} else if (machineGunScene == 3)
				{
					int returnCount = 15;
					timeRate = nowCount / returnCount;
					//プレイヤーに当たらないように上に飛ばす
					for (int i = 0; i < 343; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//中心に集まったので次に行く
					if (nowCount > returnCount)
					{
						nowCount = 0;
						machineGunScene = 4;
						//イージング処理用の代入
						for (int i = 0; i < 343; i++)
						{
							boss[i].beforePosition = boss[i].position;
							boss[i].afterPosition = bossPartFirstPosition[i];
						}
					}
				} else if (machineGunScene == 4)
				{
					int returnArmCount = 150;
					timeRate = nowCount / returnArmCount;
					//元の位置に戻る
					for (int i = 0; i < 343; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//中心に集まったので次に行く
					if (nowCount > returnArmCount)
					{
						nowCount = 0;
						howAttack = NONE;
						machineGunScene = 0;
						for (int i = 0; i < 343; i++)
						{
							//攻撃に使用したボスパーツの攻撃中フラグをおろす
							boss[i].isAttack = false;

							//ボスパーツの色を赤に戻す
							boss[i].color = baseColor;
						}
					}
				}
			}

			//長距離攻撃1
			else if (howAttack == BossAttack::LongDisAttack1)
			{
				//初期化
				if (longDisAttack1Scene == 0)
				{
					nowCount = 0;
					longDisAttack1Scene = 1;

					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//胴体の真ん中の部分だけを指定
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							//イージングのスタート座標は動かないように別の変数に代入
							boss[i].beforePosition = boss[i].position;

							//綺麗に円状に並べる
							boss[i].angle = ((i - (bossFacePartNum + 63)) - (((i - (bossFacePartNum + 63)) / 7) * 4)) / 36.0f * 360.0f;

							//イージングエンドは毎フレーム計算するとバグるのでここで別の変数に代入
							boss[i].afterPosition.x = cos(boss[i].angle * radian) * 200 + bossPartFirstPosition[25].x;
							boss[i].afterPosition.y = 45;
							boss[i].afterPosition.z = sin(boss[i].angle * radian) * 200;

							//攻撃に使用するボスパーツは攻撃中にしておく
							boss[i].isAttack = true;

							//ボスパーツの色を変更
							boss[i].color = attackColor;
						}
					}
				}

				//ボスパーツを円状に移動させる
				else if (longDisAttack1Scene == 1)
				{
					int maxCount = 120;
					timeRate = nowCount / maxCount;
					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//胴体の真ん中の部分だけを指定
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeOut(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
						}
					}

					//イージングのカウント
					nowCount++;

					//カウントダウンがマックスなら発射に移る
					if (nowCount > maxCount) {
						longDisAttack1Scene = 2;
						nowCount = 0;

						for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
						{
							//イージングのスタート座標は動かないように別の変数に代入
							boss[i].beforePosition = boss[i].position;

							//胴体の真ん中の部分だけを指定
							if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
							{
								//イージングのスタート座標は動かないように別の変数に代入
								boss[i].beforePosition = boss[i].position;

								//綺麗に円状に並べる
								boss[i].angle = ((i - (bossFacePartNum + 63)) - (((i - (bossFacePartNum + 63)) / 7) * 4)) / 36.0f * 360.0f;

								//イージングエンドは毎フレーム計算するとバグるのでここで別の変数に代入
								boss[i].afterPosition.x = cos(boss[i].angle * radian) * 1500 + bossPartFirstPosition[25].x;
								boss[i].afterPosition.y = 45;
								boss[i].afterPosition.z = sin(boss[i].angle * radian) * 1500;
							}
						}
					}
				}

				//ボスパーツを円状に移動させる
				else if (longDisAttack1Scene == 2)
				{
					int maxCount = 90;
					timeRate = nowCount / maxCount;
					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//胴体の真ん中の部分だけを指定
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							boss[i].position.x = static_cast<float>(EASE::lerp(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::lerp(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::lerp(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
						}
					}

					//イージングのカウント
					nowCount++;

					//カウントダウンがマックスなら上昇に移る
					if (nowCount > maxCount) {
						longDisAttack1Scene = 3;
						nowCount = 0;

						for (int i = 0; i < 36; i++)
						{
							//胴体の真ん中の部分だけを指定
							if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
							{
								//イージングのスタート座標は動かないように別の変数に代入
								boss[i].beforePosition = boss[i].position;
							}
						}
					}
				}

				//帰るときにプレイヤーに当たらないように上昇
				else if (longDisAttack1Scene == 3)
				{
					int maxCount = 100;
					timeRate = nowCount / maxCount;
					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//胴体の真ん中の部分だけを指定
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, 300, timeRate));
						}
					}

					//イージングのカウント
					nowCount++;

					//カウントダウンがマックスならBackに移る
					if (nowCount > maxCount) {
						longDisAttack1Scene = 4;
						nowCount = 0;

						for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
						{
							//胴体の真ん中の部分だけを指定
							if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
							{
								//イージングのスタート座標は動かないように別の変数に代入
								boss[i].beforePosition = boss[i].position;
							}
						}
					}
				}

				//散らばった胴体を元の場所に戻す
				else if (longDisAttack1Scene == 4)
				{
					int maxCount = 200;
					timeRate = nowCount / maxCount;
					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//胴体の真ん中の部分だけを指定
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeOut(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
						}
					}

					//イージングのカウント
					nowCount++;

					//カウントダウンがマックスならEndに移る
					if (nowCount > maxCount) {
						longDisAttack1Scene = 5;
						nowCount = 0;
					}
				}

				//長距離攻撃1終了
				else if (longDisAttack1Scene == 5)
				{
					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//胴体の真ん中の部分だけを指定
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							//攻撃に使用したボスパーツの攻撃中フラグをおろす
							boss[i].isAttack = false;

							//ボスパーツの色を赤に戻す
							boss[i].color = baseColor;
						}
					}

					//次のときのために戻しておく
					longDisAttack1Scene = 0;

					howAttack = BossAttack::NONE;
				}
			}

			//長距離攻撃2
			else if (howAttack == BossAttack::LongDisAttack2)
			{
				//初期化
				if (longDisAttack2Scene == 0)
				{
					nowCount = 0;
					longDisAttack2Scene = 1;

					for (int i = 0; i < bossFacePartNum; i++)
					{
						//イージングのスタート座標は動かないように別の変数に代入
						boss[i].beforePosition = boss[i].position;

						//イージングエンドは毎フレーム計算するとバグるのでここで別の変数に代入
						boss[i].afterPosition.x = boss[i].position.x + 100;
						boss[i].afterPosition.y = boss[i].position.y - 570;

						//攻撃に使用するボスパーツは攻撃中にしておく
						boss[i].isAttack = true;

						//ボスパーツの色を変更
						boss[i].color = attackColor;
					}
				}

				//ボスの頭を外させる
				else if (longDisAttack2Scene == 1)
				{
					int maxCount = 60;
					timeRate = nowCount / maxCount;
					for (int i = 0; i < bossFacePartNum; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
					}

					//イージングのカウント
					nowCount++;

					//カウントダウンがマックスなら移動に移る
					if (nowCount > maxCount) {
						longDisAttack2Scene = 2;
						nowCount = 0;

						//部位破壊度合いによって速度が異なる
						//健康体
						if (howBossBody == 5)
						{
							partSpeedX = 10.0f;
							partSpeedZ = 6.0f;
						}

						//右腕破壊時
						else if (howBossBody == 4)
						{
							partSpeedX = 11.0f;
							partSpeedZ = -7.0f;
						}

						//両腕破壊時
						else if (howBossBody == 3)
						{
							partSpeedX = 12.0f;
							partSpeedZ = 8.0f;
						}

						//右足破壊時
						else if (howBossBody == 2)
						{
							partSpeedX = 13.0f;
							partSpeedZ = -9.0f;
						}

						//左足破壊時
						else if (howBossBody == 1)
						{
							partSpeedX = 20.0f;
							partSpeedZ = 15.0f;
						}
					}
				}

				//ボスの頭を移動させる
				else if (longDisAttack2Scene == 2)
				{
					//ボスの頭を動かす
					for (int i = 0; i < bossFacePartNum; i++)
					{
						boss[i].position.x += partSpeedX;
						boss[i].position.z += partSpeedZ;
					}

					//一度壁に当たったらシーンを変更
					const float playerPosXMax = 900.0f;
					const float playerPosXMin = -900.0f;
					const float playerPosZMax = 900.0f;
					const float playerPosZMin = -900.0f;

					for (int i = 0; i < bossFacePartNum; i++)
					{
						if (boss[i].position.x > playerPosXMax)
						{
							partSpeedX *= -1;
							longDisAttack2Scene = 3;
							nowCount = 0;
						}

						if (boss[i].position.z > playerPosZMax || boss[i].position.z < playerPosZMin)
						{
							partSpeedZ *= -1;
							longDisAttack2Scene = 3;
							nowCount = 0;
						}
					}
				}

				//ボスの頭を平行に移動させる
				else if (longDisAttack2Scene == 3)
				{
					//普通にカウント
					nowCount++;

					//カウント終了フレーム数
					int maxCount = 600;

					//ボスを動かす
					for (int i = 0; i < bossFacePartNum; i++)
					{
						boss[i].position.x += partSpeedX;
						boss[i].position.z += partSpeedZ;
					}

					//壁に当たったら反射させる
					const float playerPosXMax = 900.0f;
					const float playerPosXMin = -900.0f;
					const float playerPosZMax = 900.0f;
					const float playerPosZMin = -900.0f;
					for (int i = 0; i < bossFacePartNum; i++)
					{
						if (boss[i].position.x > playerPosXMax || boss[i].position.x < playerPosXMin)
						{
							partSpeedX *= -1;
							break;
						}

						if (boss[i].position.z > playerPosZMax || boss[i].position.z < playerPosZMin)
						{
							partSpeedZ *= -1;
							break;
						}
					}

					//カウントダウンが一定時間経ったら一時停止に移る
					if (nowCount > maxCount) {
						longDisAttack2Scene = 4;
						nowCount = 0;
					}
				}

				//ボスを一時停止させる
				else if (longDisAttack2Scene == 4)
				{
					//普通にカウント
					nowCount++;

					//カウント終了フレーム数
					int maxCount = 60;

					//カウントダウンが一定時間経ったらBackに移る
					if (nowCount > maxCount) {
						longDisAttack2Scene = 5;
						nowCount = 0;

						for (int i = 0; i < bossPartNum; i++)
						{
							//生きているパーツだけ
							if (boss[i].isAlive == true)
							{
								//イージングのスタート座標は動かないように別の変数に代入
								boss[i].beforePosition = boss[i].position;
							}
						}
					}
				}

				//ボスを元の状態に戻す
				else if (longDisAttack2Scene == 5)
				{
					int maxCount = 200;
					timeRate = nowCount / maxCount;

					//イージングさせて移動
					for (int i = 0; i < bossPartNum; i++)
					{
						//生きているパーツだけ
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeOut(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
						}
					}

					//イージングのカウント
					nowCount++;

					//カウントダウンがマックスならEndに移る
					if (nowCount > maxCount) {
						longDisAttack2Scene = 6;
						nowCount = 0;
					}
				}

				//長距離攻撃2終了
				else if (longDisAttack2Scene == 6)
				{
					for (int i = 0; i < bossFacePartNum; i++)
					{
						//攻撃に使用したボスパーツの攻撃中フラグをおろす
						boss[i].isAttack = false;

						//ボスパーツの色を赤に戻す
						boss[i].color = baseColor;
					}

					//次のときのために戻しておく
					longDisAttack2Scene = 0;

					howAttack = BossAttack::NONE;
				}
			}

			//-HP一定以下になった時の特別行動-//
			//右腕破壊
			else if (howAttack == BossAttack::RightArmBreak)
			{
				//落ちるパーツの番号
				int partNumberStart = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum;
				int partNumberLast = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum;

				//部位が落ちるセット
				if (partBreakScene[0] == Boss_RA_Broken::RA_BreakSet)
				{
					int partNum = partNumberLast - partNumberStart;

					//まだ落ちる処理のセットが完了して無ければセットする
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						if (boss[i].isBreak == 0)
						{
							boss[i].isBreak = rand() % partNum;
							boss[i].beforePosition = boss[i].position;
						}
					}

					//ムービー系
					nowMovie = true;
					camera.Position = MovieCameraPos;
					camera.afterTarget = camera.Target;//元に戻すように位置を記録
					camera.Target = MovieCameratarget;
					camera.beforeTarget = camera.Target;//イージングように元の位置を記録

					//セットしたので次に行く
					partBreakScene[0] = Boss_RA_Broken::RA_BreakUpdate;
					nowCount = 0;

					//見えるようにする
					for (int i = 0; i < bossPartNum; i++)
					{
						fallPlace[i] = true;
					}
				}

				//部位が落ちる更新
				else if (partBreakScene[0] == Boss_RA_Broken::RA_BreakUpdate)
				{
					const int maxCount = 100;
					//落ちる処理
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						float timeRate = (nowCount - boss[i].isBreak) / maxCount;

						if (boss[i].isAlive == true && nowCount - boss[i].isBreak > 0)
						{
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, 10, timeRate));

							if (timeRate > 1.0f)
							{
								boss[i].isAlive = false;
							}
						}
					}

					//カウントを進める
					nowCount++;

					//落ち終わったら次に行く
					if (nowCount > (partNumberLast - partNumberStart) * 3 + maxCount)
					{
						partBreakScene[0] = Boss_RA_Broken::RA_Init;
					}
				}

				//初期化
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Init)
				{
					for (int i = 574; i < 670; i++)
					{
						//イージングのスタート座標は動かないように別の変数に代入
						boss[i].beforePosition = boss[i].position;
					}

					partBreakScene[0] = Boss_RA_Broken::RA_Move;
					nowCount = 0;
				}

				//足のパーツを右腕に持ってくる
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Move)
				{
					int maxCount = 60;
					timeRate = nowCount / maxCount;
					for (int i = 574; i < 670; i++)
					{
						if (i < 664)
						{
							boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, bossPartFirstPosition[i + 96].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, bossPartFirstPosition[i + 96].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeOut(boss[i].beforePosition.z, bossPartFirstPosition[i + 96].z, timeRate));
						}

						else
						{
							boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, bossPartFirstPosition[i + 80].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, bossPartFirstPosition[i + 80].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeOut(boss[i].beforePosition.z, bossPartFirstPosition[i + 80].z, timeRate));
						}
					}

					//イージングのカウント
					nowCount++;

					//カウントダウンがマックスならStay1に移る
					if (nowCount > maxCount) {
						for (int i = 0; i < bossPartNum; i++)
						{
							//攻撃に使用するボスパーツは攻撃中にしておく
							boss[i].isAttack = true;

							//ボスパーツの色を変更
							boss[i].color = attackColor;
						}

						partBreakScene[0] = Boss_RA_Broken::RA_Stay1;
						nowCount = 0;
					}
				}

				//一定時間止まる
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Stay1)
				{
					//普通にカウント
					nowCount++;

					//カウント終了フレーム数
					int maxCount = 100;

					//カウントダウンが一定時間経ったらFallに移る
					if (nowCount > maxCount) {
						for (int i = 0; i < bossPartNum; i++)
						{
							//生きているパーツだけ
							if (boss[i].isAlive == true)
							{
								//イージングのスタート座標は動かないように別の変数に代入
								boss[i].beforePosition = boss[i].position;

								//イージングエンドは毎フレーム計算するとバグるのでここで別の変数に代入
								boss[i].afterPosition.y = boss[i].position.y + 2000;
							}
						}

						partBreakScene[0] = Boss_RA_Broken::RA_Fry;
						nowCount = 0;
					}
				}

				//真上に飛ぶ
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Fry)
				{
					int maxCount = 60;
					timeRate = nowCount / maxCount;

					//イージングさせて移動
					for (int i = 0; i < bossPartNum; i++)
					{
						//生きているパーツだけ
						if (boss[i].isAlive == true)
						{
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						}
					}

					//カメラのターゲットのイージング
					float afterCameraPos = 200.0f;
					camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, afterCameraPos, timeRate));

					//イージングのカウント
					nowCount++;

					//カウントダウンがマックスならStay2に移る
					if (nowCount > maxCount) {
						partBreakScene[0] = Boss_RA_Broken::RA_Stay2;
						nowCount = 0;
						camera.beforeTarget = camera.Target;

						//ボスの大きさを3倍にする
						/*for (int i = 0; i < bossPartNum; i++)
						{
							if (boss[i].isAlive == true)
							{
								boss[i].size = { 15.0f, 15.0f, 15.0f };
								boss[i].radius = 75.0f;
							}
						}*/
					}
				}

				//一定時間止まる
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Stay2)
				{
					//普通にカウント
					nowCount++;

					//カウント終了フレーム数
					int maxCount = 400;

					timeRate = nowCount / (maxCount / 2.0f);

					//カメライージング
					if (nowMovie == true)
					{
						camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, player.position.x, timeRate));
						camera.Position.y = static_cast<float>(EASE::easeIn(camera.beforePosition.y, player.position.y, timeRate));
						camera.Position.z = static_cast<float>(EASE::easeIn(camera.beforePosition.z, player.position.z, timeRate));

						camera.Target.x = static_cast<float>(EASE::easeIn(camera.beforeTarget.x, camera.afterTarget.x, timeRate));
						camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, easeAfterTargetY, timeRate));
						camera.Target.z = static_cast<float>(EASE::easeIn(camera.beforeTarget.z, camera.afterTarget.z, timeRate));

						//ムービーが終わるので操作可能にする
						if (nowCount > maxCount / 2) { nowMovie = false; }
					}

					//落ちてくる目安をプレイヤーの座標に移動させる
					attackRange[0].position.x = player.position.x;
					attackRange[0].position.y = 1.0f;
					attackRange[0].position.z = player.position.z;
					attackRange[0].size = 150.0f;
					attackRange[0].color = { 0.9, 0.9, 0.1, 1 };
					attackRange[0].isAlive = true;

					//カウントダウンが一定時間経ったらFallに移る
					if (nowCount > maxCount) {
						//落ちる場所をプレイヤーの座標にするため距離を出す
						//ボスパーツ25がちょうどボスのx,zの中心
						fallPosDistance.x = boss[24].position.x - player.position.x;
						fallPosDistance.z = boss[24].position.z - player.position.z;

						//落ちてくる目安の色を変える
						attackRange[0].color = { 0.9, 0.1, 0.1, 1 };

						for (int i = 0; i < bossPartNum; i++)
						{
							//生きているパーツだけ
							if (boss[i].isAlive == true)
							{
								//ボスのパーツをプレイヤーの座標に
								boss[i].position.x -= fallPosDistance.x;
								boss[i].position.z -= fallPosDistance.z;

								//イージングのスタート座標は動かないように別の変数に代入
								boss[i].beforePosition = boss[i].position;

								//イージングエンドは毎フレーム計算するとバグるのでここで別の変数に代入
								boss[i].afterPosition.y = boss[i].position.y - 2250;
							}
						}

						partBreakScene[0] = Boss_RA_Broken::RA_Fall;
						nowCount = 0;
					}
				}

				//プレイヤーの座標に落ちてくる
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Fall)
				{
					int maxCount = 60;
					timeRate = nowCount / maxCount;

					//イージングさせて移動
					for (int i = 0; i < bossPartNum; i++)
					{
						//生きているパーツだけ
						if (boss[i].isAlive == true)
						{
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						}
					}

					//イージングのカウント
					nowCount++;

					//カウントダウンがマックスならStay3に移る
					if (nowCount > maxCount) {
						partBreakScene[0] = Boss_RA_Broken::RA_Stay3;
						nowCount = 0;
					}
				}

				//一定時間止まる
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Stay3)
				{
					//普通にカウント
					nowCount++;

					//カウント終了フレーム数
					int maxCount = 200;

					//カウントダウンが一定時間経ったらBackに移る
					if (nowCount > maxCount) {
						for (int i = 0; i < bossPartNum; i++)
						{
							//生きているパーツだけ
							if (boss[i].isAlive == true)
							{
								//イージングのスタート座標は動かないように別の変数に代入
								boss[i].beforePosition = boss[i].position;
							}
						}
						//落ちてくる目安のものを消す
						attackRange[0].isAlive = false;
						partBreakScene[0] = Boss_RA_Broken::RA_Back;
						nowCount = 0;
					}
				}

				//右腕となっていた足を元の場所に戻す
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Back)
				{
					int maxCount = 200;
					timeRate = nowCount / maxCount;

					//イージングさせて移動
					for (int i = 0; i < bossPartNum; i++)
					{
						//生きているパーツだけ
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeOut(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
							/*boss[i].size.x = static_cast<float>(EASE::easeOut(15.0f, 3.0f, timeRate));
							boss[i].size.y = static_cast<float>(EASE::easeOut(15.0f, 3.0f, timeRate));
							boss[i].size.z = static_cast<float>(EASE::easeOut(15.0f, 3.0f, timeRate));
							boss[i].radius = static_cast<float>(EASE::easeOut(75.0f, 15.0f, timeRate));*/
						}
					}

					//イージングのカウント
					nowCount++;

					//カウントダウンがマックスならEndに移る
					if (nowCount > maxCount) {
						partBreakScene[0] = Boss_RA_Broken::RA_End;
						nowCount = 0;
					}
				}

				//右腕破壊時の特別攻撃終了
				else if (partBreakScene[0] == Boss_RA_Broken::RA_End)
				{
					for (int i = 0; i < bossPartNum; i++)
					{
						fallPlace[i] = false;

						//攻撃に使用したボスパーツの攻撃中フラグをおろす
						boss[i].isAttack = false;

						//ボスパーツの色を赤に戻す
						boss[i].color = baseColor;
					}

					howAttack = BossAttack::NONE;
					partBreakScene[0] = Boss_RA_Broken::RA_BreakSet;
				}
			}

			//左腕破壊
			else if (howAttack == BossAttack::LeftArmBreak)
			{
				//落ちるパーツの番号
				int partNumberStart = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum;
				int partNumberLast = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum + bossLeftArmPartNum;

				//部位が落ちるセット
				if (partBreakScene[1] == Boss_LA_Broken::LA_BreakSet)
				{
					int partNum = partNumberLast - partNumberStart;

					//まだ落ちる処理のセットが完了して無ければセットする
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						if (boss[i].isBreak == 0)
						{
							boss[i].isBreak = rand() % partNum;
							boss[i].beforePosition = boss[i].position;
						}
					}

					//ムービー系
					nowMovie = true;
					camera.Position = MovieCameraPos;
					camera.afterTarget = camera.Target;//元に戻すように位置を記録
					camera.Target = MovieCameratarget;
					camera.beforeTarget = camera.Target;//イージングように元の位置を記録

					for (int i = 0; i < bossPartNum; i++)
					{
						fallPlace[i] = true;
					}

					//セットしたので次に行く
					partBreakScene[1]++;
					nowCount = 0;
				}

				//部位が落ちる更新
				else if (partBreakScene[1] == Boss_LA_Broken::LA_BreakUpdate)
				{
					const int maxCount = 100;
					//落ちる処理
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						float timeRate = (nowCount - boss[i].isBreak) / maxCount;

						if (boss[i].isAlive == true && nowCount - boss[i].isBreak > 0)
						{
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, 10, timeRate));

							if (timeRate > 1.0f)
							{
								boss[i].isAlive = false;
							}
						}
					}

					//カウントを進める
					nowCount++;

					//落ち終わったら次に行く
					if (nowCount > (partNumberLast - partNumberStart) * 3 + maxCount)
					{
						partBreakScene[1]++;
						nowCount = 0;
					}
				}

				//イージングの準備
				else if (partBreakScene[1] == Boss_LA_Broken::LA_Init)
				{
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].beforePosition = boss[i].position;
							boss[i].afterPosition.x = static_cast<float>((rand() % 4000) - 2000);
							boss[i].afterPosition.z = static_cast<float>((rand() % 4000) - 2000);

							//攻撃に使用するボスパーツは攻撃中にしておく
							boss[i].isAttack = true;

							//ボスパーツの色を変更
							boss[i].color = attackColor;
						}
					}

					partBreakScene[1]++;
					nowCount = 0;
				}

				//上へ移動する
				else if (partBreakScene[1] == Boss_LA_Broken::LA_UpMove)
				{
					timeRate = nowCount / maxTime;

					//\上にばらけながらイージング
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, 2000, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
						}
					}

					//カメラのターゲットのイージング
					float afterCameraTarget = 200.0f;
					camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, afterCameraTarget, timeRate));

					//イージングのカウント
					nowCount++;

					//イージングが終わったので場所をセットして次に行く
					if (nowCount > maxTime)
					{
						for (int i = 0; i < fallNum; i++)
						{
							boss[i].size = { 30.0f,30.0f,30.0f };
							boss[i].radius = 150.0f;
							fallPlace[i] = false;
						}

						for (int i = fallNum; i < bossPartNum; i++)
						{
							fallPlace[i] = false;
						}

						nowCount = 0;
						partBreakScene[1]++;
						camera.beforeTarget = camera.Target;
					}
				}

				//上から一定個数落とす
				else if (partBreakScene[1] == Boss_LA_Broken::LA_AttackBigPartFall)
				{
					nowCount++;
					int maxCount = 80;
					timeRate = nowCount / maxCount;

					//カメライージング
					if (nowMovie == true)
					{
						camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, player.position.x, timeRate));
						camera.Position.y = static_cast<float>(EASE::easeIn(camera.beforePosition.y, player.position.y, timeRate));
						camera.Position.z = static_cast<float>(EASE::easeIn(camera.beforePosition.z, player.position.z, timeRate));

						camera.Target.x = static_cast<float>(EASE::easeIn(camera.beforeTarget.x, camera.afterTarget.x, timeRate));
						camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, easeAfterTargetY, timeRate));
						camera.Target.z = static_cast<float>(EASE::easeIn(camera.beforeTarget.z, camera.afterTarget.z, timeRate));

						//ムービーが終わるので操作可能にする
						if (nowCount > maxCount) { nowMovie = false; }
					}


					const float fallSpeed = 10;
					int nowFall = static_cast<int>(nowCount) / 100;//今落ちているブロックの数
					//今のカウント以下なら落とす
					for (int i = 0; i < fallNum; i++)
					{
						if (i < nowFall && fallPlace[i] == false)
						{
							//プレイヤー基準で場所をセットX
							int random = (rand() % 100) - 50;
							boss[i].position.x = player.position.x + random;
							//落ちてくる目安をプレイヤーの座標に移動させる
							attackRange[i].position.x = player.position.x + random;

							//プレイヤー基準で場所をセットZ
							random = (rand() % 100) - 50;
							boss[i].position.z = player.position.z + random;
							attackRange[i].position.y = 1.0f;
							attackRange[i].position.z = player.position.z + random;
							attackRange[i].size = 150.0f;
							attackRange[i].color = { 0.9, 0.1, 0.1, 1 };
							attackRange[i].isAlive = true;

							//セットしたのでtrueにする
							fallPlace[i] = true;
						}

						//落とす
						else if (fallPlace[i] == true)
						{
							boss[i].position.y = boss[i].position.y - fallSpeed;
							if (boss[i].position.y < 10&& attackRange[i].isAlive == true)
							{
								attackRange[i].isAlive = false;
							}
						}
					}

					//全て落とし終えたので次に行く
					if (boss[fallNum - 1].position.y < -500)
					{
						//落とす前の高さと大きさに戻す
						for (int i = 0; i < fallNum; i++)
						{
							boss[i].position.y = 900;
							boss[i].size = { 3.0f,3.0f,3.0f };
							boss[i].radius = 15.0f;
							fallPlace[i] = false;
						}

						//次のイージング準備
						for (int i = 0; i < bossPartNum; i++)
						{
							if (boss[i].isAlive == true)
							{
								boss[i].beforePosition = boss[i].position;
							}

							for (int i = 0; i < bossPartNum; i++)
							{
								fallPlace[i] = true;
							}
						}

						partBreakScene[1]++;
						nowCount = 0;
					}
				}

				//イージングで元の位置に戻す
				else if (partBreakScene[1] == Boss_LA_Broken::LA_Undo)
				{
					timeRate = nowCount / maxTime;

					//元の位置にイージング
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
						}
					}

					//イージングのカウント
					nowCount++;

					//イージングが終わったので場所をセットして次に行く
					if (nowCount > maxTime)
					{
						for (int i = 0; i < bossPartNum; i++)
						{
							fallPlace[i] = false;

							//攻撃に使用したボスパーツの攻撃中フラグをおろす
							boss[i].isAttack = false;

							//ボスパーツの色を赤に戻す
							boss[i].color = baseColor;
						}

						nowCount = 0;
						partBreakScene[1] = Boss_LA_Broken::LA_BreakSet;
						howAttack = BossAttack::NONE;
					}
				}
			}

			//右足破壊
			else if (howAttack == BossAttack::RightLegBreak)
			{
				//落ちるパーツの番号
				int partNumberStart = bossFacePartNum + bossBodyPartNum;
				int partNumberLast = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum;

				//部位が落ちるセット
				if (partBreakScene[2] == Boss_RF_Broken::RF_BreakSet)
				{
					int partNum = partNumberLast - partNumberStart;

					//まだ落ちる処理のセットが完了して無ければセットする
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						if (boss[i].isBreak == 0)
						{
							boss[i].isBreak = rand() % partNum;
							boss[i].beforePosition = boss[i].position;
						}
					}

					//ムービー系
					nowMovie = true;
					camera.afterPosition = camera.Position;//元に戻すように位置を記録
					camera.afterTarget = camera.Target;//元に戻すように位置を記録
					camera.Position = MovieCameraPos;//ムービー時の位置に移動
					camera.Target = MovieCameratarget;//ムービー時の位置に移動
					camera.beforePosition = camera.Position;//イージングように元の位置を記録
					camera.beforeTarget = camera.Target;//イージングように元の位置を記録

					for (int i = 0; i < bossPartNum; i++)
					{
						fallPlace[i] = true;
					}

					//セットしたので次に行く
					partBreakScene[2]++;
					nowCount = 0;
				}

				//部位が落ちる更新
				else if (partBreakScene[2] == Boss_RF_Broken::RF_BreakUpdate)
				{
					const int maxCount = 100;
					//落ちる処理
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						float timeRate = (nowCount - boss[i].isBreak) / maxCount;

						if (boss[i].isAlive == true && nowCount - boss[i].isBreak > 0)
						{
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, 10, timeRate));

							if (timeRate > 1.0f)
							{
								boss[i].isAlive = false;
							}
						}
					}

					//カウントを進める
					nowCount++;

					//落ち終わったら次に行く
					if (nowCount > (partNumberLast - partNumberStart) * 3 + maxCount)
					{
						partBreakScene[2]++;
						nowCount = 0;
					}
				}

				//イージング準備
				else if (partBreakScene[2] == Boss_RF_Broken::RF_Init)
				{
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].beforePosition = boss[i].position;
							boss[i].afterPosition.y = static_cast<float>((rand() % 2000) + 10);
							boss[i].afterPosition.z = static_cast<float>((rand() % 4000) - 2000);

							//攻撃に使用するボスパーツは攻撃中にしておく
							boss[i].isAttack = true;

							//ボスパーツの色を変更
							boss[i].color = attackColor;
						}
						fallPlace[i] = true;
					}

					partBreakScene[2]++;
					nowCount = 0;
				}

				//x座標のマイナス方向へイージング
				else if (partBreakScene[2] == Boss_RF_Broken::RF_InFrontPart)
				{
					timeRate = nowCount / maxTime;

					//x座標のプラス方向へイージング
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, -2000, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
						}
					}

					//カメラのターゲットのイージング
					float afterCameraPos = -400.0f;
					camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, afterCameraPos, timeRate));

					//イージングのカウント
					nowCount++;

					//イージングが終わったので場所をセットして次に行く
					if (nowCount > maxTime)
					{
						for (int i = 0; i < bossPartNum; i++)
						{
							fallPlace[i] = false;
						}

						camera.beforePosition = camera.Position;
						camera.beforeTarget = camera.Target;
						partBreakScene[2]++;
						nowCount = 0;
					}
				}

				//視界を戻す
				else if (partBreakScene[2] == Boss_RF_Broken::RF_ReturnEye)
				{
					nowCount++;
					int maxCount = 80;
					timeRate = nowCount / maxCount;

					camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, player.position.x, timeRate));
					camera.Position.y = static_cast<float>(EASE::easeIn(camera.beforePosition.y, player.position.y, timeRate));
					camera.Position.z = static_cast<float>(EASE::easeIn(camera.beforePosition.z, player.position.z, timeRate));

					camera.Target.x = static_cast<float>(EASE::easeIn(camera.beforeTarget.x, camera.afterTarget.x, timeRate));
					camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, easeAfterTargetY, timeRate));
					camera.Target.z = static_cast<float>(EASE::easeIn(camera.beforeTarget.z, camera.afterTarget.z, timeRate));

					//ムービーが終わるので操作可能にする
					if (nowCount > maxCount) {
						for (int i = 0; i < bossPartNum; i++)
						{
							fallPlace[i] = false;
						}

						nowMovie = false;
						partBreakScene[2]++;
						nowCount = 0;
					}
				}

				//x座標のマイナス方向からプラス方向へ飛ばす
				else if (partBreakScene[2] == Boss_RF_Broken::RF_AttackBuckPart)
				{
					nowCount++;
					const float fallSpeed = 20;
					int nowFall = static_cast<int>(nowCount) % bossPartNum;//今落ちているブロックの数
					int chack = 0;//動いている数
					const int maxMove = 700;
					//動かす処理
					for (int i = 0; i < maxMove; i++)
					{
						if (i < nowFall && nowCount < 1000)
						{
							if (fallPlace[i] == false)
							{
								boss[i].position.y = static_cast<float>((rand() % 300) - 2);
								boss[i].position.z = static_cast<float>((rand() % 4000) - 1000);
								//セットしたのでtrueにする
								fallPlace[i] = true;
							}
						}

						if (fallPlace[i] == true)
						{
							//落とす
							boss[i].position.x = boss[i].position.x + fallSpeed;

							//一定値以上行ったら配置しなおす
							if (boss[i].position.x > 950)
							{
								boss[i].position.x = -2000;
								fallPlace[i] = false;
							}
							chack++;
						}
					}

					//全てが止まったため次に行く
					if (nowCount > 1000 && chack == 0)
					{
						//落とす前の高さと大きさに戻してイージングの準備
						for (int i = 0; i < bossPartNum; i++)
						{
							boss[i].position.x = -2000;
							if (boss[i].isAlive == true)
							{
								boss[i].beforePosition = boss[i].position;
							}
							fallPlace[i] = true;
						}

						partBreakScene[2]++;
						nowCount = 0;
					}
				}

				//元の位置に戻す
				else if (partBreakScene[2] == Boss_RF_Broken::RF_Undo)
				{
					timeRate = nowCount / maxTime;

					//元の位置にイージング
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
						}
					}

					//イージングのカウント
					nowCount++;

					//イージングが終わったので場所をセットして次に行く
					if (nowCount > maxTime)
					{
						for (int i = 0; i < bossPartNum; i++)
						{
							//攻撃に使用したボスパーツの攻撃中フラグをおろす
							boss[i].isAttack = false;

							//ボスパーツの色を赤に戻す
							boss[i].color = baseColor;
						}

						partBreakScene[2] = Boss_RF_Broken::RF_BreakSet;
						nowCount = 0;
						howAttack = BossAttack::NONE;
					}
				}
			}

			//左足破壊
			else if (howAttack == BossAttack::LeftLegBreak)
			{
				//落ちるパーツの番号
				int partNumberStart = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum;
				int partNumberLast = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum;

				//部位が落ちるセット
				if (partBreakScene[3] == Boss_LF_Broken::LF_BreakSet)
				{
					int partNum = partNumberLast - partNumberStart;

					//まだ落ちる処理のセットが完了して無ければセットする
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						if (boss[i].isBreak == 0)
						{
							boss[i].isBreak = rand() % partNum;
							boss[i].beforePosition = boss[i].position;
						}
					}

					//ムービー系
					nowMovie = true;
					camera.afterPosition = camera.Position;//元に戻すように位置を記録
					camera.afterTarget = camera.Target;//元に戻すように位置を記録
					camera.Position = MovieCameraPos;//ムービー時の位置に移動
					camera.Target = MovieCameratarget;//ムービー時の位置に移動
					camera.beforePosition = camera.Position;//イージングように元の位置を記録
					camera.beforeTarget = camera.Target;//イージングように元の位置を記録

					for (int i = 0; i < bossPartNum; i++)
					{
						fallPlace[i] = true;
					}

					//セットしたので次に行く
					partBreakScene[3]++;
					nowCount = 0;
				}

				//部位が落ちる更新
				else if (partBreakScene[3] == Boss_LF_Broken::LF_BreakUpdate)
				{
					const int maxCount = 100;
					//落ちる処理
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						float timeRate = (nowCount - boss[i].isBreak) / maxCount;

						if (boss[i].isAlive == true && nowCount - boss[i].isBreak > 0)
						{
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, 10, timeRate));

							if (timeRate > 1.0f)
							{
								boss[i].isAlive = false;
							}
						}
					}

					//カウントを進める
					nowCount++;

					//落ち終わったら次に行く
					if (nowCount > (partNumberLast - partNumberStart) * 3 + maxCount)
					{
						partBreakScene[3]++;
						nowCount = 0;
					}
				}

				//イージング準備
				else if (partBreakScene[3] == Boss_LF_Broken::LF_Init)
				{
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].beforePosition = boss[i].position;
							fallPlace[i] = true;

							//攻撃に使用するボスパーツは攻撃中にしておく
							boss[i].isAttack = true;

							//ボスパーツの色を変更
							boss[i].color = attackColor;
						}
					}

					partBreakScene[3]++;
					nowCount = 0;
				}

				//中心へイージング
				else if (partBreakScene[3] == Boss_LF_Broken::LF_CenterMove)
				{
					timeRate = nowCount / maxTime;

					//中心に集まるようにイージング
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, 0, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, 10, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, 0, timeRate));
						}
					}

					//カメラのターゲットのイージング
					float afterCameraPos = 100.0f;
					float afterCameraTarget = 90.0f;
					camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, afterCameraPos, timeRate));
					camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, afterCameraPos, timeRate));

					//イージングのカウント
					nowCount++;

					//イージングが終わったので場所をセットして次に行く
					if (nowCount > maxTime)
					{
						for (int i = 0; i < rotationNum; i++)
						{
							boss[i].angle = static_cast<float>(rand() % 360);
							boss[i].distance = static_cast<float>((rand() % 60) - 30);
							fallPlace[i] = true;
						}

						for (int i = rotationNum; i < bossPartNum; i++)
						{
							fallPlace[i] = false;
						}

						camera.beforePosition = camera.Position;
						camera.beforeTarget = camera.Target;
						partBreakScene[3]++;
						nowCount = 0;
					}
				}

				//視界を戻す
				else if (partBreakScene[3] == Boss_LF_Broken::LF_ReturnEye)
				{
					nowCount++;
					int maxCount = 80;
					timeRate = nowCount / maxCount;

					camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, player.position.x, timeRate));
					camera.Position.y = static_cast<float>(EASE::easeIn(camera.beforePosition.y, player.position.y, timeRate));
					camera.Position.z = static_cast<float>(EASE::easeIn(camera.beforePosition.z, player.position.z, timeRate));

					camera.Target.x = static_cast<float>(EASE::easeIn(camera.beforeTarget.x, camera.afterTarget.x, timeRate));
					camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, easeAfterTargetY, timeRate));
					camera.Target.z = static_cast<float>(EASE::easeIn(camera.beforeTarget.z, camera.afterTarget.z, timeRate));

					//ムービーが終わるので操作可能にする
					if (nowCount > maxCount) {
						for (int i = 0; i < rotationNum; i++)
						{
							fallPlace[i] = true;
						}

						for (int i = rotationNum; i < bossPartNum; i++)
						{
							fallPlace[i] = false;
							boss[i].size = { 7.0f,7.0f,7.0f };
							boss[i].radius = 32.0f;
						}

						nowMovie = false;
						partBreakScene[3]++;
						nowCount = 0;
					}
				}

				//円周状に跳ばす
				else if (partBreakScene[3] == Boss_LF_Broken::LF_AttackCircleMove)
				{
					nowCount++;
					int chack = 0;
					int nowMove = static_cast<int>(nowCount) % 30;//今動いている個数
					if (nowMove == 0 && fallPlace[pushCount] == false && nowCount < 800)
					{
						int max = pushCount + 5;
						for (int i = pushCount; i < max; i++)
						{
							boss[i].angle = static_cast<float>(10 * (4 - i));
							boss[i].distance = static_cast<float>(20 + i);
							boss[i].position.y = 10.0f;
							fallPlace[i] = true;
						}

						//カウントを進めて個数以上なら値を戻す
						pushCount += 5;
						if (pushCount > bossPartNum) { pushCount = rotationNum; }
					}

					for (int i = rotationNum; i < bossPartNum; i++)
					{
						if (fallPlace[i] == true)
						{
							boss[i].angle += (1 / boss[i].distance) * 500;
							boss[i].distance += 4;
							boss[i].position.x = cos(boss[i].angle * radian) * boss[i].distance;
							boss[i].position.z = sin(boss[i].angle * radian) * boss[i].distance;
							chack++;
						}
						//一定値行ったら消す
						if (boss[i].distance > 1500)
						{
							fallPlace[i] = false;
						}
					}

					//x0y10z0中心の半径20で回す
					for (int i = 0; i < rotationNum; i++)
					{
						boss[i].angle += 5;
						boss[i].position.x = cos((boss[i].angle + 230) * radian) * boss[i].distance;
						boss[i].position.y = sin(boss[i].angle * radian) * boss[i].distance + 20;
						boss[i].position.z = sin((boss[i].angle + 130) * radian) * boss[i].distance;

						if (boss[i].angle > 360)
						{
							boss[i].angle = 0;
						}
					}

					//一定以上行ったら次に行く
					if (nowCount > 800 && chack == 0)
					{
						for (int i = 0; i < bossPartNum; i++)
						{
							fallPlace[i] = true;
							if (boss[i].isAlive == true)
							{
								float pos = static_cast<float>((rand() % 30) - 15);
								boss[i].position.x = pos;
								pos = static_cast<float>((rand() % 30) - 15);
								boss[i].position.z = pos;
								boss[i].beforePosition = boss[i].position;
								boss[i].size = { 3.0f,3.0f,3.0f };
								boss[i].radius = 15.0f;

							}
						}

						partBreakScene[3]++;
						nowCount = 0;
					}
				}

				//元の位置に戻す
				else if (partBreakScene[3] == Boss_LF_Broken::LF_Undo)
				{
					timeRate = nowCount / maxTime;

					//元の位置にイージング
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
						}
					}

					//イージングのカウント
					nowCount++;

					//イージングが終わったので場所をセットして次に行く
					if (nowCount > maxTime)
					{
						for (int i = 0; i < bossPartNum; i++)
						{
							//攻撃に使用したボスパーツの攻撃中フラグをおろす
							boss[i].isAttack = false;

							//ボスパーツの色を赤に戻す
							boss[i].color = baseColor;
						}

						partBreakScene[3] = Boss_LF_Broken::LF_BreakSet;
						nowCount = 0;
						howAttack = BossAttack::NONE;
					}
				}
			}

			//プレイヤーhpが0になったら負け
			if (player.isAlive == false)
			{
				scene = SCENE::Lose;
			}

			//ボスのhpが0になったら勝ち
			if (isBossAlive == false)
			{
				scene = SCENE::Win;
			}

			//ビューの処理
			if (nowMovie == false)
			{
				camera.Position = player.position;
			}
		}

		//負け
		else if (scene == SCENE::Lose)
		{
			//イージングセット
			if (loseScene == 0)
			{
				//YOUDIEDの文字の透明に戻す
				youDiedColor.w = 0.0f;

				//ムービー系
				nowMovie = true;
				camera.beforeTarget = camera.Target;

				//次のシーンへ
				loseScene = 1;
				nowCount = 0;
			}

			//目線を下に下げる
			else if (loseScene == 1)
			{
				nowCount++;
				timeRate = nowCount / maxTime;

				//カメラのターゲットのイージング
				float afterCameraTarget = 100.0f;
				camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, afterCameraTarget, timeRate));

				//YOUDIEDの文字を段々見せる
				youDiedColor.w = static_cast<float>(EASE::easeIn(0.0f, 1.0f, timeRate));

				//イージングが終わったので場所をセットして次に行く
				if (nowCount > maxTime)
				{
					loseScene = 2;
					nowCount = 0;
				}
			}

			//完全に「YOU DIED」を表示したらタイトルに戻れる
			else if (loseScene == 2)
			{
				//タイトルに戻る
				if (input->TriggerPadButtons(Input::PAD_B) || input->TriggerKey(DIK_RETURN) == true)
				{
					//ボタン効果音
					audio->SoundPlayWava(xAudio2.Get(), Button, false);

					for (int i = 0; i < bossPartNum; i++)
					{
						boss[i].isAlive = false;
					}

					//次使うときのために初期化しておく
					audio->StopSound(BuckSound);
					scene = SCENE::Init;
					nowCount = 0;
					loseScene = 0;
				}
			}
		}

		//勝ち
		else if (scene == SCENE::Win)
		{
			//プレイヤー移動
			PlayerOperationMove(player);
			//プレイヤー視点移動
			PlayerOperationEye(camera, Tgspeed);
			//ビューの処理
			if (nowMovie == false)
			{
				camera.Position = player.position;
			}

			//プレイヤーの攻撃入力
			if (input->TriggerPadButtons(Input::PAD_RB) || input->TriggerKey(DIK_SPACE) == true)
			{
				bulletCount = BulletPreparation(bulletNum, bullet[bulletCount], player, camera.Target, bulletCount);
			}

			//プレイヤーの攻撃更新
			for (int i = 0; i < bulletNum; i++)
			{
				UpDateBullet(bullet[i]);
			}

			//イージング準備
			if (winScene == 0)
			{
				//ポジションをセット
				float dist = 3000;
				float angle = 0;
				for (int i = 0; i < bossPartNum; i++)
				{
					boss[i].beforePosition = boss[i].position;
					angle = static_cast<float>(rand() % 360);
					boss[i].afterPosition.x = (cos(angle * radian) * dist) + boss[i].position.x;
					boss[i].afterPosition.y = boss[i].position.y + rand() % static_cast<int>(dist) + 30;
					angle = static_cast<float>(rand() % 360);
					boss[i].afterPosition.z = (cos(angle * radian) * dist) + boss[i].position.z;

				}

				//ムービー系
				winScene++;
				nowCount = 0;
			}

			//イージングによる疑似爆破
			else if (winScene == 1)
			{
				const int max = 80;
				nowCount++;
				timeRate = nowCount / max;

				for (int i = 0; i < bossPartNum; i++)
				{
					boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
					boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
					boss[i].position.z = static_cast<float>(EASE::easeOut(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
				}

				//ある程度遅くなったのでパーツを消す
				if (nowCount > max - 3)
				{
					for (int i = 0; i < bossPartNum; i++)
					{
						boss[i].isAlive = false;
					}
				}

				//イージングが終わったので次に行く
				if (nowCount > max)
				{
					winScene++;
					nowCount = 0;
				}
			}

			//YOUWINをじわじわ出す
			else if (winScene == 2)
			{
				nowCount++;
				timeRate = nowCount / maxTime;

				//YOUWINの文字を段々見せる
				youWinColor.w = static_cast<float>(EASE::easeOut(0.0f, 1.0f, timeRate));

				//イージングが終わったので場所をセットして次に行く
				if (nowCount > maxTime)
				{
					winScene++;
					nowCount = 0;
				}
			}

			//完全に「YOU WIN」を表示したらタイトルに戻れる
			else if (winScene == 3)
			{
				//タイトルに戻る
				if (input->TriggerPadButtons(Input::PAD_B) || input->TriggerKey(DIK_RETURN) == true)
				{
					//ボタン効果音
					audio->SoundPlayWava(xAudio2.Get(), Button, false);

					for (int i = 0; i < bossPartNum; i++)
					{
						boss[i].isAlive = false;
					}

					//次使うときのために初期化しておく
					audio->StopSound(BuckSound);
					scene = SCENE::Init;
					nowCount = 0;
					winScene = 0;
				}
			}
		}

		//ムービーに入ったらダメージ判定をリセット
		if (nowMovie == true)
		{
			if (isPlayerDamage == true)
			{
				isPlayerDamage = false;
				isPlayerDamageRedShow = false;
				playerDamageTime = 0;
			}
		}

		//ビューの処理
		{
			camera.info->SetPositon(camera.Position);
			camera.info->upDate(camera.Target, cameraSpeed);
		}

		//--------描画-------------//
		//ボスは生きているときだけ描画
		for (int i = 0; i < bossPartNum; i++)
		{
			if (boss[i].isAlive == true)
			{
				//通常時
				if (howAttack < BossAttack::RightArmBreak)
				{
					boss[i].info->draw(boss[i].position, boss[i].rota, boss[i].size, boss[i].color, false);
				}

				//ボスの部位破壊時
				if (howAttack >= BossAttack::RightArmBreak && fallPlace[i] == true)
				{
					boss[i].info->draw(boss[i].position, boss[i].rota, boss[i].size, boss[i].color, false);
				}
			}
		}

		//チュートリアルの的
		for (int i = 0; i < tutorialEnemyNum; i++)
		{
			if (tutorialEnemy[i].isAlive == true)
			{
				tutorialEnemy[i].info->draw(tutorialEnemy[i].position, { 0,0,0 }, { tutorialEnemy[i].size }, tutorialEnemy[i].color, false);
			}
		}

		//-Obj-//
		//playerの攻撃
		for (int i = 0; i < bulletNum; i++)
		{
			if (bullet[i].isAlive == true)
			{
				bullet[i].info->draw(bullet[i].position, bullet[i].rota,
					bulletSize, bullet[i].color, false);
			}
		}

		//ボスの攻撃範囲
		for (int i = 0; i < attackRangeNum; i++)
		{
			if (attackRange[i].isAlive == true)
			{
				attackRange[i].info->draw(attackRange[i].position, attackRange[i].rota,
					attackRange[i].size, attackRange[i].color, false);
			}
		}

		//地面
		wall->draw({ 0,0,0 }, { 0,0,0 }, 200, { 1.0f,1.0f, 1.0f, 1.0f }, false);


		//スプライト描画

		//タイトル
		if (scene == SCENE::Titel)
		{
			//タイトルロゴ
			sprite[0]->draw(gameSystem->getdXCommon(), SpriteName::Title, { window_width / 2 - 10, 175 }, 0, { 1000.0f, 300.0f });

			//タイトルシーンの分岐(A or B)
			sprite[1]->draw(gameSystem->getdXCommon(), SpriteName::TitleBunki, { window_width / 2, 345 }, 0, { 232.0f, 72.0f });
		}

		//チュートリアル
		else if (scene == SCENE::Tutorial)
		{
			//イージング用のポジションセット
			if (tutorialScene == TutorialScene::TUT_Init)
			{
			}

			//viewをプレイヤー視点にイージングで変化させる
			else if (tutorialScene == TutorialScene::TUT_PlayerEyeMove)
			{
			}

			//チュートリアル1移動させる
			else if (tutorialScene == TutorialScene::TUT_TryMove)
			{
				//操作説明その1(移動・カメラ移動)
				sprite[2]->draw(gameSystem->getdXCommon(), SpriteName::Pad1, { 1040.0f, 100.0f }, 0, { 428.0f, 160.0f });

				//操作誘導その1(「まずは移動してみましょう」)
				sprite[5]->draw(gameSystem->getdXCommon(), SpriteName::Step1, { window_width / 2, 50.0f }, 0, { 318.0f, 90.0f });

				//「チュートリアル・歩く」用ゲージ枠
				sprite[10]->draw(gameSystem->getdXCommon(), SpriteName::MoveGaugeOut, { window_width / 2 - moveGaugeSizeXMax / 2 - 6, 170.0f }, 0, { 538.0f, 26.0f });

				//「チュートリアル・歩く」用ゲージ
				sprite[11]->draw(gameSystem->getdXCommon(), SpriteName::MoveGaugeIn, { window_width / 2 - moveGaugeSizeXMax / 2, 173.0f }, 0, { moveGaugeSizeX, 20.0f });
			}

			//チュートリアル2攻撃させる
			else if (tutorialScene == TutorialScene::TUT_TryRedEnemy)
			{
				//操作説明その2(射撃)
				sprite[3]->draw(gameSystem->getdXCommon(), SpriteName::Pad2, { 1040.0f, 100.0f }, 0, { 428.0f, 160.0f });

				//操作誘導その2(「視界を移動させ、攻撃してみましょう」)
				sprite[6]->draw(gameSystem->getdXCommon(), SpriteName::Step2, { window_width / 2 + 0.5f, 50.0f }, 0, { 501.0f, 130.0f });

				//今何体倒したか
				if (checkTutorialEnemy == 0)
				{
					sprite[12]->draw(gameSystem->getdXCommon(), SpriteName::Count0, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				else if (checkTutorialEnemy == 1)
				{
					sprite[13]->draw(gameSystem->getdXCommon(), SpriteName::Count1, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				else if (checkTutorialEnemy == 2)
				{
					sprite[14]->draw(gameSystem->getdXCommon(), SpriteName::Count2, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				else if (checkTutorialEnemy == 3)
				{
					sprite[15]->draw(gameSystem->getdXCommon(), SpriteName::Count3, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				else if (checkTutorialEnemy == 4)
				{
					sprite[16]->draw(gameSystem->getdXCommon(), SpriteName::Count4, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				else if (checkTutorialEnemy == 5)
				{
					sprite[17]->draw(gameSystem->getdXCommon(), SpriteName::Count5, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				//レティクル
				sprite[30]->draw(gameSystem->getdXCommon(), SpriteName::Reticle, { window_width / 2, window_height / 2 }, 0, { 64.0f, 64.0f });
			}

			//黄色の敵をを赤にして倒せるようにする
			else if (tutorialScene == TutorialScene::TUT_TryYellowEnemy)
			{
				//操作説明その2(射撃)
				sprite[3]->draw(gameSystem->getdXCommon(), SpriteName::Pad2, { 1040.0f, 100.0f }, 0, { 428.0f, 160.0f });

				//操作誘導その3(「攻撃可能になったブロックを攻撃しましょう」)
				sprite[7]->draw(gameSystem->getdXCommon(), SpriteName::Step3, { window_width / 2 + 0.5f, 50.0f }, 0, { 591.0f, 90.0f });

				//今何体倒したか
				if (checkTutorialEnemy == 5)
				{
					sprite[12]->draw(gameSystem->getdXCommon(), SpriteName::Count0, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				else if (checkTutorialEnemy == 6)
				{
					sprite[13]->draw(gameSystem->getdXCommon(), SpriteName::Count1, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				else if (checkTutorialEnemy == 7)
				{
					sprite[14]->draw(gameSystem->getdXCommon(), SpriteName::Count2, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				else if (checkTutorialEnemy == 8)
				{
					sprite[15]->draw(gameSystem->getdXCommon(), SpriteName::Count3, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				else if (checkTutorialEnemy == 9)
				{
					sprite[16]->draw(gameSystem->getdXCommon(), SpriteName::Count4, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				else if (checkTutorialEnemy == 10)
				{
					sprite[17]->draw(gameSystem->getdXCommon(), SpriteName::Count5, { window_width / 2, 230.0f }, 0, { 40.0f, 24.0f });
				}

				//レティクル
				sprite[30]->draw(gameSystem->getdXCommon(), SpriteName::Reticle, { window_width / 2, window_height / 2 }, 0, { 64.0f, 64.0f });
			}

			//チュートリアル3好きに行動させ、満足したらタイトルへ
			else if (tutorialScene == TutorialScene::TUT_Undo)
			{
				//操作説明その3(全部)
				sprite[4]->draw(gameSystem->getdXCommon(), SpriteName::Pad3, { 1040.0f, 100.0f }, 0, { 428.0f, 160.0f });

				//操作誘導その4(「好きに動いてみましょう」)
				sprite[8]->draw(gameSystem->getdXCommon(), SpriteName::Step4, { window_width / 2, 50.0f }, 0, { 322.0f, 90.0f });

				//タイトルへ戻る
				sprite[9]->draw(gameSystem->getdXCommon(), SpriteName::TitleBack, { window_width / 2, 200.0f }, 0, { 164.0f, 27.0f });

				//レティクル
				sprite[30]->draw(gameSystem->getdXCommon(), SpriteName::Reticle, { window_width / 2, window_height / 2 }, 0, { 64.0f, 64.0f });
			}
		}

		else if (scene == SCENE::StartMovie)
		{
			//準備
			if (bossMovieScene == BossMovieScene::BM_Init)
			{
			}

			//中央に集まる
			else if (bossMovieScene == BossMovieScene::BM_CenterMove)
			{
			}

			//人型になる
			else if (bossMovieScene == BossMovieScene::BM_CreateHumanModel)
			{
			}

			//視界移動
			else if (bossMovieScene == BossMovieScene::BM_EyeMove)
			{
			}
		}

		else if (scene == SCENE::Battle)
		{
			//レティクル
			if (nowMovie == false)
			{
				sprite[30]->draw(gameSystem->getdXCommon(), SpriteName::Reticle, { window_width / 2, window_height / 2 }, 0, { 64.0f, 64.0f });

				if (isPlayerDamageRedShow == true)
				{
					//ダメージを受けたら画面を赤くする
					sprite[31]->draw(gameSystem->getdXCommon(), SpriteName::DamageRed, { 0.0f, 0.0f }, 0, { 1280.0f, 760.0f }, { 1, 0, 0, 0.5 });
				}
			}

			//ボスのHP用ゲージ枠
			sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

			//ボスのHP用ゲージ
			sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


			//死んだハート
			sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
			sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
			sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
			sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
			sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

			if (playerHP >= 5)
			{
				//HPハート
				sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
			}

			if (playerHP >= 4)
			{
				//HPハート
				sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
			}

			if (playerHP >= 3)
			{
				//HPハート
				sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
			}

			if (playerHP >= 2)
			{
				//HPハート
				sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
			}

			if (playerHP >= 1)
			{
				//HPハート
				sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
			}
		}

		//負け
		else if (scene == SCENE::Lose)
		{
			//イージングセット
			if (loseScene == 0)
			{
				//ボスのHP用ゲージ枠
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//ボスのHP用ゲージ
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//死んだハート
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HPハート
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HPハート
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HPハート
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HPハート
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HPハート
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}
			}

			//目線を下に下げる
			else if (loseScene == 1)
			{
				//ボスのHP用ゲージ枠
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//ボスのHP用ゲージ
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//死んだハート
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HPハート
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HPハート
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HPハート
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HPハート
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HPハート
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				//背景を暗く
				sprite[33]->draw(gameSystem->getdXCommon(), SpriteName::BrackBackGround, { 0, 0 }, 0, { 1280.0f, 760.0f }, youDiedColor);

				//YOUDIED
				sprite[32]->draw(gameSystem->getdXCommon(), SpriteName::YOUDIED, { window_width / 2, 330.0f }, 0, { 508.0f, 76.0f }, youDiedColor);
			}

			//lose表示
			else if (loseScene == 2)
			{
				//ボスのHP用ゲージ枠
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//ボスのHP用ゲージ
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//死んだハート
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HPハート
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HPハート
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HPハート
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HPハート
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HPハート
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				//背景を暗く
				sprite[33]->draw(gameSystem->getdXCommon(), SpriteName::BrackBackGround, { 0, 0 }, 0, { 1280.0f, 760.0f }, youDiedColor);

				//YOUDIED
				sprite[32]->draw(gameSystem->getdXCommon(), SpriteName::YOUDIED, { window_width / 2, 330.0f }, 0, { 508.0f, 76.0f }, youDiedColor);

				//タイトルへ戻る
				sprite[9]->draw(gameSystem->getdXCommon(), SpriteName::TitleBack, { window_width / 2, 440.0f }, 0, { 164.0f, 27.0f });
			}
		}

		//勝ち
		else if (scene == SCENE::Win)
		{
			if (winScene == 0)
			{
				//ボスのHP用ゲージ枠
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//ボスのHP用ゲージ
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//死んだハート
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HPハート
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HPハート
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HPハート
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HPハート
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HPハート
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}
			}

			else if (winScene == 1)
			{
				//ボスのHP用ゲージ枠
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//ボスのHP用ゲージ
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//死んだハート
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HPハート
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HPハート
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HPハート
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HPハート
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HPハート
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}
			}

			else if (winScene == 2)
			{
				//ボスのHP用ゲージ枠
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//ボスのHP用ゲージ
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//死んだハート
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HPハート
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HPハート
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HPハート
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HPハート
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HPハート
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				//YOUWIN
				sprite[34]->draw(gameSystem->getdXCommon(), SpriteName::YOUWIN, { window_width / 2, 330.0f }, 0, { 474.0f, 76.0f }, youWinColor);
			}

			else if (winScene == 3)
			{
				//ボスのHP用ゲージ枠
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//ボスのHP用ゲージ
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//死んだハート
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HPハート
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HPハート
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HPハート
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HPハート
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HPハート
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				//YOUWIN
				sprite[34]->draw(gameSystem->getdXCommon(), SpriteName::YOUWIN, { window_width / 2, 330.0f }, 0, { 474.0f, 76.0f }, youWinColor);

				//タイトルへ戻る
				sprite[9]->draw(gameSystem->getdXCommon(), SpriteName::TitleBack, { window_width / 2, 440.0f }, 0, { 164.0f, 27.0f });
			}
		}

		//デバッグテキスト
		text.DrawAll(gameSystem->getdXCommon());

		//-------------------------------------//
		gameSystem->draw();

		gameSystem->frameRateKeep();
	}

	//解放
	//xAudio2解放
	xAudio2.Reset();

	//音声データ解放
	audio->SoundUnload(&BuckSound);
	audio->SoundUnload(&BossDamage);
	audio->SoundUnload(&PlayerDamage);
	audio->SoundUnload(&Shot);
	audio->SoundUnload(&Button);

	for (int i = 0; i < bulletNum; i++)
	{
		delete bullet[i].info;
	}

	for (int i = 0; i < bossPartNum; i++)
	{
		delete boss[i].info;
	}

	for (int i = 0; i < tutorialEnemyNum; i++)
	{
		delete tutorialEnemy[i].info;
	}

	for (int i = 0; i < attackRangeNum; i++)
	{
		delete attackRange[i].info;
	}

	delete wall;

	for (int i = 0; i < spriteNum; i++)
	{
		delete sprite[i];
	}

	delete camera.info;
	delete audio;

	gameSystem->systemDelete();

	//登録解除
	winApp->winRelease();

	return 0;
}