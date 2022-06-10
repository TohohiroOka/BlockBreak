#include "WindowApp.h"
#include "GameSystem.h"
#include"Camera.h"
#include"Easing.h"

const float radian = XM_PI / 180.0f;//���W�A��

//�J�����̏��i�����ȁj
static struct CAMERA
{
	Camera* info = nullptr;
	XMFLOAT3 Position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 Target = { 0.0f,0.0f,0.0f };

	//�J�����̃C�[�W���O�p
	XMFLOAT3 beforePosition = { 0.0f,0.0f,0.0f };
	XMFLOAT3 beforeTarget = { 0.0f,0.0f,0.0f };
	XMFLOAT3 afterPosition = { 0.0f,0.0f,0.0f };
	XMFLOAT3 afterTarget = { 0.0f,0.0f,0.0f };
}camera;

//�v���C���[�̏��
static struct PLAYER
{
	bool isAlive = true;
	XMFLOAT3 position = { 0.0f, 50.0f, 0.0f };
	float size = 10.0f;
	float radius = 20.0f;
}player;

//���̏��
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

//�`���[�g���A���̓G�̏��
const int tutorialEnemyNum = 10;
static struct TUTORIAL_ENEMY
{
	Object3d* info = nullptr;
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 size = { 3.0f, 3.0f, 3.0f };
	XMFLOAT4 color = { 1.0f, 0.0f, 0.0f, 1.0f };//�F(��)
	float radius = 8.0f;//�{�X�p�[�c�̔��a
	bool isAlive = false;
	bool isDamageColor = false;
	int damageTime = 0;
}tutorialEnemy[tutorialEnemyNum];

//�{�X�̏��
//��p�[�c�̌�
const int bossFacePartNum = 343;
//�̃p�[�c�̌�
const int bossBodyPartNum = 231;
//�E���p�[�c�̌�
const int bossRightFootPartNum = 48;
//�����p�[�c�̌�
const int bossLeftFootPartNum = 48;
//�E�r�p�[�c�̌�
const int bossRightArmPartNum = 90;
//���r�p�[�c�̌�
const int bossLeftArmPartNum = 90;
//���v
const int bossPartNum = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum +
bossLeftFootPartNum + bossRightArmPartNum + bossLeftArmPartNum;
static struct BOSS_PART
{
	Object3d* info = nullptr;
	bool isAlive = false;
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };//�ʒu
	XMFLOAT3 rota = { 0.0f, 270.0f, 0.0f };//��]
	XMFLOAT3 size = { 3.0f, 3.0f, 3.0f };//�T�C�Y	
	float radius = 15.0f;//�{�X�p�[�c�̔��a
	XMFLOAT4 color = { 1.0f, 0.0f, 0.0f, 1.0f };//�F(��)
	bool isAttack = false;

	//�C�[�W���O�p�̈���
	float angle = 0;//�~�`�ɉ񂷈ׂ̊p�x
	float distance = 0;//���S����̋���
	float beforeDistance = 0;//�C�[�W���O�̏I�_�p
	XMFLOAT3 beforePosition = { 0.0f,0.0f,0.0f };//�C�[�W���O�̎n�_�p
	XMFLOAT3 afterPosition = { 0.0f,0.0f,0.0f };//�C�[�W���O�̏I�_�p

	//���ʔj�󎞎g�p
	int isBreak = 0;
}boss[bossPartNum];

const int attackRangeNum = 8;
static struct ATTACK_RANGE
{
	Obj* info = nullptr;
	bool isAlive = false;
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };//�ʒu
	XMFLOAT3 rota = { 0.0f, 0.0f, 0.0f };//��]
	float size = 1.0f;//�T�C�Y
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };//�F(��)

}attackRange[attackRangeNum];

//�{�X�̏������i�u���b�N�̌�������ϐ��j
void InitBoss(BOSS_PART* boss, float* timeRateAttack, float* nowCountAttack, bool* bossIsShot, bool* fallPlace)
{
	//�{�X�p�[�c�̏��
	for (int i = 0; i < bossPartNum; i++)
	{
		boss[i].isAlive = false;
		boss[i].position = {};//�ʒu
		boss[i].rota = { 0.0f, 270.0f, 0.0f };//��]
		boss[i].size = { 3.0f, 3.0f, 3.0f };//�T�C�Y	
		boss[i].radius = 15.0f;//�{�X�p�[�c�̔��a
		boss[i].color = { 1.0f, 0.0f, 0.0f, 1.0f };//�F(��)
		boss[i].isAttack = false;

		//�C�[�W���O�p�̈���
		boss[i].angle = 0;//�~�`�ɉ񂷈ׂ̊p�x
		boss[i].distance = 0;//���S����̋���
		boss[i].beforeDistance = 0;//�C�[�W���O�̏I�_�p
		boss[i].beforePosition = { 0.0f,0.0f,0.0f };//�C�[�W���O�̎n�_�p
		boss[i].afterPosition = { 0.0f,0.0f,0.0f };//�C�[�W���O�̏I�_�p

		//���ʔj�󎞎g�p
		boss[i].isBreak = 0;

		timeRateAttack[i] = 0;
		nowCountAttack[i] = 0;
		bossIsShot[i] = false;

		fallPlace[i] = false;
	}
};

//�ړ�����
bool PlayerOperationMove(PLAYER& player)
{
	Input* input = Input::GetInstance();

	bool isMove = false;

	//player�ړ�
	float Pspeed = 5.0f;
	//�E����
	if (input->LEFT_PadStickX() == +1 || input->PushKey(DIK_D) == true) {
		player.position.x += Pspeed * cos((camera.Target.x + 90) * radian);
		player.position.z += Pspeed * sin((camera.Target.x + 90) * radian);
		isMove = true;
	}
	//������
	if (input->LEFT_PadStickX() == -1 || input->PushKey(DIK_A) == true) {
		player.position.x -= Pspeed * cos((camera.Target.x + 90) * radian);
		player.position.z -= Pspeed * sin((camera.Target.x + 90) * radian);
		isMove = true;
	}
	//������
	if (input->LEFT_PadStickY() == +1 || input->PushKey(DIK_W) == true) {
		player.position.x -= Pspeed * cos(camera.Target.x * radian);
		player.position.z -= Pspeed * sin(camera.Target.x * radian);
		isMove = true;
	}
	//�����
	if (input->LEFT_PadStickY() == -1 || input->PushKey(DIK_S) == true) {
		player.position.x += Pspeed * cos(camera.Target.x * radian);
		player.position.z += Pspeed * sin(camera.Target.x * radian);
		isMove = true;
	}

	//�v���C���[�̍s���ł���͈͂�ݒ�B����ȏ�i�߂Ȃ��悤�ɂ���
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

//���E����
void PlayerOperationEye(CAMERA& camera, const float Tgspeed)
{
	Input* input = Input::GetInstance();

	//���E�ړ�
	if (input->RIGHT_PadStickX() == +1 || input->PushKey(DIK_RIGHT) == true) { camera.Target.x -= Tgspeed; }//�E����
	if (input->RIGHT_PadStickX() == -1 || input->PushKey(DIK_LEFT) == true) { camera.Target.x += Tgspeed; }//������
	if (input->RIGHT_PadStickY() == +1 || input->PushKey(DIK_UP) == true) { camera.Target.y += Tgspeed; }//�����
	if (input->RIGHT_PadStickY() == -1 || input->PushKey(DIK_DOWN) == true) { camera.Target.y -= Tgspeed; }//������

	//�㉺�����̊p�x����
	if (camera.Target.y < 85) { camera.Target.y = 86; }//������
	if (camera.Target.y > 251) { camera.Target.y = 250; }//�㐧��

	//���̐���
	if (camera.Target.x > 360) { camera.Target.x = 0; }
	if (camera.Target.x < -360) { camera.Target.x = 0; }
};

//�v���C���[�ƃ{�X�p�[�c�̓����蔻��
bool CollisionPlayerBossPartCheck(PLAYER player, BOSS_PART bossPart)
{
	//�{�X�p�[�c�������Ă���Ȃ�
	if (bossPart.isAlive == true)
	{
		//�����蔻��
		float lenght;
		//���@�ƓG�̋������v�Z
		lenght = (player.position.x - bossPart.position.x) * (player.position.x - bossPart.position.x)
			+ (player.position.y - bossPart.position.y) * (player.position.y - bossPart.position.y)
			+ (player.position.z - bossPart.position.z) * (player.position.z - bossPart.position.z);

		//���@�ƓG���Փ˂��Ă����ꍇ
		if (lenght <= (player.radius + bossPart.radius) * (player.radius + bossPart.radius))
		{
			return true;
		}
	}

	return false;
};

//�e�ƃ{�X�p�[�c�̓����蔻��
bool CollisionBulletBossPartCheck(BULLET bullet, BOSS_PART bossPart)
{
	//�e�����ˏ�ԂȂ灕�{�X�p�[�c�������Ă���Ȃ�
	if (bullet.isAlive == true && bossPart.isAlive == true)
	{
		//�����蔻��
		float lenght;
		//���@�ƓG�̋������v�Z
		lenght = (bullet.position.x - bossPart.position.x) * (bullet.position.x - bossPart.position.x)
			+ (bullet.position.y - bossPart.position.y) * (bullet.position.y - bossPart.position.y)
			+ (bullet.position.z - bossPart.position.z) * (bullet.position.z - bossPart.position.z);

		//���@�ƓG���Փ˂��Ă����ꍇ
		if (lenght <= (bullet.radius + bossPart.radius) * (bullet.radius + bossPart.radius))
		{
			return true;
		}
	}

	return false;
};

//�e�ƃ`���[�g���A���p�G�l�~�[�̓����蔻��
bool CollisionPlayerTutorialEnemyCheck(BULLET bullet, TUTORIAL_ENEMY tutorialEnemy)
{
	//�e�����ˏ�ԂȂ灕�`���[�g���A���p�̓G�������Ă���Ȃ�
	if (bullet.isAlive == true && tutorialEnemy.isAlive == true)
	{
		//�����蔻��
		float lenght;
		//���@�ƓG�̋������v�Z
		lenght = (bullet.position.x - tutorialEnemy.position.x) * (bullet.position.x - tutorialEnemy.position.x)
			+ (bullet.position.y - tutorialEnemy.position.y) * (bullet.position.y - tutorialEnemy.position.y)
			+ (bullet.position.z - tutorialEnemy.position.z) * (bullet.position.z - tutorialEnemy.position.z);

		//���@�ƓG���Փ˂��Ă����ꍇ
		if (lenght <= (player.radius + tutorialEnemy.radius) * (player.radius + tutorialEnemy.radius))
		{
			return true;
		}
	}

	return false;
};

//�v���C���[�̍U����������
int BulletPreparation(const int bulletNum, BULLET& bullet, const PLAYER& player, XMFLOAT3 target, int bulletCount)
{
	if (bullet.isAlive == false)
	{
		//�v���C���[�̈ʒu�ɋ����Z�b�g
		bullet.position = player.position;

		const float bulletSpeed = 30.0f;

		//�ړ��������Z�b�g
		bullet.movement = { cos(target.x * radian) * -bulletSpeed ,
			sin(target.y * radian) * -bulletSpeed ,sin(target.x * radian) * -bulletSpeed };

		//�������Z�b�g
		bullet.rota = { cos(target.x * radian) , sin(target.y * radian),0 };

		//�\����true��
		bullet.isAlive = true;

		//���Ԃɂ���
		bulletCount = bulletCount + 1;

		//�g�p�ԍ������v������Ȃ�0�ɒ���
		if (bulletCount >= bulletNum)
		{
			bulletCount = 0;
		}
	}

	return bulletCount;
};

//�v���C���[�̍U���X�V����
void UpDateBullet(BULLET& bullet)
{
	//���˂���Ă���Ȃ�ړ�
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

	//���͈͊O�Ȃ����
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
	// �E�B���h�E�T�C�Y
	const int window_width = 1280;// ����
	const int window_height = 720;// �c��

	//�Q�[����
	const wchar_t* gameName = L"�u���b�N�����̋t�P";


	//�E�B���h�E������
	WindowApp* winApp = nullptr;
	winApp = new WindowApp();
	winApp->winStart(window_width, window_height, gameName);

	//�S�̂̏�����
	GameSystem* gameSystem = nullptr;
	gameSystem = new GameSystem();
	gameSystem->init(winApp, window_width, window_height, gameName);

	//key
	Input* input = Input::GetInstance();
	input->init(winApp);

	//�J����
	camera.info = new Camera();
	camera.info->init(winApp);

	//-------------------------------------------------------------------------------------------------//

	//�����錾
	//3D�I�u�W�F�N�g����

	//�摜�ǂݍ���
	Object3d::LoadTexture(winApp, 0, L"Resources/enemy.png");

	//�{�X
	for (int i = 0; i < bossPartNum; i++)
	{
		boss[i].info = new Object3d();
		boss[i].info->Create(0);
	}

	//�`���[�g���A���̓I
	for (int i = 0; i < tutorialEnemyNum; i++)
	{
		tutorialEnemy[i].info = new Object3d();
		tutorialEnemy[i].info->Create(0);
	}

	//-------------------------------------------------------------------------------------------------//

	//obj����
	enum OBJ
	{
		Bullet,
		Wall,
		attack
	};

	Obj::LoadObj(Bullet, "Bullet");//�v���C���[�̍U��
	Obj::LoadObj(Wall, "wall");//�n��
	Obj::LoadObj(attack, "attack");//�U���͈�

	//�n��
	Obj* wall = nullptr;
	wall = new Obj();
	wall->CreateModel(Wall);

	//�v���C���[��
	for (int i = 0; i < bulletNum; i++)
	{
		bullet[i].info = new Obj();
		bullet[i].info->CreateModel(Bullet);
	}

	//�{�X�̍U���͈�
	for (int i = 0; i < attackRangeNum; i++)
	{
		attackRange[i].info = new Obj();
		attackRange[i].info->CreateModel(attack);
	}
	//-------------------------------------------------------------------------------------------------//

	//�X�v���C�g����
	const int spriteNum = 35;

	//�X�v���C�g�̏�����
	Sprite* sprite[spriteNum] = { nullptr };

	for (int i = 0; i < spriteNum; i++)
	{
		sprite[i] = new Sprite();
	}

	//�X�v���C�g�̐���
	{
		sprite[0]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�^�C�g�����S
		sprite[1]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.0f }); //�^�C�g����ʂ�AorB
		sprite[2]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�������1(�p�b�h�̊G1)
		sprite[3]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�������2(�p�b�h�̊G2)
		sprite[4]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�������3(�p�b�h�̊G3)
		sprite[5]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.0f }); //����U��1
		sprite[6]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.0f }); //����U��2
		sprite[7]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.0f }); //����U��3
		sprite[8]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.0f }); //����U��4
		sprite[9]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�u�^�C�g���֖߂�v
		sprite[10]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //�u�`���[�g���A���E�����v�p�Q�[�W�g
		sprite[11]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //�u�`���[�g���A���E�����v�p�Q�[�W
		sprite[12]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�u�`���[�g���A���E�|���v�p�J�E���g0
		sprite[13]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�u�`���[�g���A���E�|���v�p�J�E���g1
		sprite[14]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�u�`���[�g���A���E�|���v�p�J�E���g2
		sprite[15]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�u�`���[�g���A���E�|���v�p�J�E���g3
		sprite[16]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�u�`���[�g���A���E�|���v�p�J�E���g4
		sprite[17]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�u�`���[�g���A���E�|���v�p�J�E���g5
		sprite[18]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //�{�X��HP�p�Q�[�W�g
		sprite[19]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //�{�X��HP�p�Q�[�W
		sprite[20]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //HP�n�[�g
		sprite[21]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //HP�n�[�g
		sprite[22]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //HP�n�[�g
		sprite[23]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //HP�n�[�g
		sprite[24]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //HP�n�[�g
		sprite[25]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�n�[�g�g
		sprite[26]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�n�[�g�g
		sprite[27]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�n�[�g�g
		sprite[28]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�n�[�g�g
		sprite[29]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //�n�[�g�g
		sprite[30]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //���e�B�N��
		sprite[31]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //�_���[�W�󂯂����ɐԂ��Ȃ�p
		sprite[32]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //YOUDIED
		sprite[33]->init(winApp, gameSystem->getdXCommon(), { 0.0f, 0.0f }); //�w�i���Â�
		sprite[34]->init(winApp, gameSystem->getdXCommon(), { 0.5f, 0.5f }); //YOUWIN
	}

	//�X�v���C�g�ԍ��ɖ��O��t����
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

	//�X�v���C�g�p�̃e�N�X�`���ǂݍ���/texNum��1����
	{
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 1, L"Resources/title.png");	//�^�C�g�����S
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 2, L"Resources/titleBunki.png");//�^�C�g����ʂ�AorB
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 3, L"Resources/pad1.png");		//�������1
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 4, L"Resources/pad2.png");		//�������2
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 5, L"Resources/pad3.png");		//�������3
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 6, L"Resources/step1.png");	//����U��1
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 7, L"Resources/step2.png");	//����U��2
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 8, L"Resources/step3.png");	//����U��3
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 9, L"Resources/step4.png");	//����U��4
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 10, L"Resources/titleBack.png");//�u�^�C�g���֖߂�v
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 11, L"Resources/moveGaugeOut.png");//�u�`���[�g���A���E�����v�p�Q�[�W�g
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 12, L"Resources/moveGaugeIn.png");//�u�`���[�g���A���E�����v�p�Q�[�W
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 13, L"Resources/count0.png");//�u�`���[�g���A���E�|���v�p�J�E���g0
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 14, L"Resources/count1.png");//�u�`���[�g���A���E�|���v�p�J�E���g1
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 15, L"Resources/count2.png");//�u�`���[�g���A���E�|���v�p�J�E���g2
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 16, L"Resources/count3.png");//�u�`���[�g���A���E�|���v�p�J�E���g3
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 17, L"Resources/count4.png");//�u�`���[�g���A���E�|���v�p�J�E���g4
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 18, L"Resources/count5.png");//�u�`���[�g���A���E�|���v�p�J�E���g5
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 19, L"Resources/bossHPGaugeOut.png");//�{�X��HP�p�Q�[�W�g
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 20, L"Resources/bossHPGaugeIn.png");//�{�X��HP�p�Q�[�W
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 21, L"Resources/heart.png");//HP�n�[�g
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 22, L"Resources/heartEnpty.png");//�n�[�g�g
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 23, L"Resources/reticle.png");//���e�B�N��
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 24, L"Resources/playerDamageRed.png");//�_���[�W�󂯂����ɐԂ��Ȃ�p
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 25, L"Resources/youdied.png");//YOU DIED
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 26, L"Resources/blackBackGround.png");//�w�i���Â�
		Sprite::LoadTexture(winApp, gameSystem->getdXCommon()->getdev(), 27, L"Resources/youwin.png");//YOUWIN
	}

	//�f�o�b�O�e�L�X�g
	DebugText text;
	text.init(winApp, gameSystem->getdXCommon());

	//-------------------------------------------------------------------------------------------------//
	//�T�E���h�p
	ComPtr<IXAudio2>xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);//XAudio�G���W���̃C���X�^���X����
	xAudio2->CreateMasteringVoice(&masterVoice);//�}�X�^�[�{�C�X����
	masterVoice->SetVolume(0.1f);//�S�̂̉���
	Audio* audio = nullptr;
	audio = new Audio();
	//-------------------------------------------------------------------------------------------------//

	//-------------wav�ǂݍ���----------------//
	SoundData BuckSound = audio->SoundLoadWave(xAudio2.Get(), "Resources/Sound/BuckSound.wav");//BGM
	SoundData BossDamage = audio->SoundLoadWave(xAudio2.Get(), "Resources/Sound/BossDamage.wav");//�{�X��damage��
	SoundData PlayerDamage = audio->SoundLoadWave(xAudio2.Get(), "Resources/Sound/PlayerDamage.wav");//�v���C���[��damage��
	SoundData Shot = audio->SoundLoadWave(xAudio2.Get(), "Resources/Sound/Shot.wav");//����
	SoundData Button = audio->SoundLoadWave(xAudio2.Get(), "Resources/Sound/Button.wav");//button���ʉ�

	//---------------�ϐ��錾--------------//
	//�V�X�e���p�ϐ�
	float length = 0.0f;

	//�C�[�W���O�̐i�s�x�p
	float nowCount = 0.0f;
	float timeRate = 0.0f;

	//�J�����̈���
	float cameraSpeed = 1;//�J�����̑��x

	//�Q�[���V�[��
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

	//�e�V�[���̒��Ŏ��s���鏈���̏���
	//�g�����V�[���̍Ō�ɕK��0�ɂ��鏈��������
	int step = 0;

	//�`���[�g���A���p
	enum TutorialScene
	{
		TUT_Init,//�C�[�W���O�p�̃|�W�V�����Z�b�g
		TUT_PlayerEyeMove,//view���v���C���[���_�ɃC�[�W���O�ŕω�������
		TUT_TryMove,//�ړ�������
		TUT_TryRedEnemy,//���X�Ԃ̓G���U��������
		TUT_TryYellowEnemy,//���F�̓G����Ԃɂ��ē|����悤�ɂ���
		TUT_Undo//�D���ɍs�������A����������^�C�g����
	};
	int tutorialScene = TutorialScene::TUT_Init;
	int moveChack = 0;//�ǂ̂��炢��������
	const int moveCheckMax = 100;//�����ŃQ�[�WMAX�ɂȂ�̂�
	float moveGaugeSizeX = 0.0f;//�u�`���[�g���A���E�����v�p�Q�[�W�̑傫��
	const float moveGaugeSizeXMax = 526.0f;//�u�`���[�g���A���E�����v�p�Q�[�W�̑傫��MAX
	int checkTutorialEnemy = 0;//�`���[�g���A���p�̓G�|������


	//���[�r�[���̓{�X�ւ̃_���[�W���薳��
	bool nowMovie = true;

	//player
	const float Tgspeed = 1.0f;

	const int playerHPMax = 5;//�v���C���[�̍ő�HP
	int playerHP = playerHPMax;//�v���C���[��HP
	bool isPlayerDamage = false;//�v���C���[���_���[�W��H����Ă��邩
	int playerDamageTime = 0;	//�v���C���[���_���[�W��H����Ă���̎���
	const int playerDamageTimeMax = 180;//���G����
	bool isPlayerDamageRedShow = false;//��ʂ�Ԃ����邩
	const int playerDamageRedShowTime = 60;//��ʂ�Ԃ����鎞��


	//playerBullet
	const float bulletSize = 1;
	int bulletCount = 0;

	//�{�X�������[�r�[�p
	enum BossMovieScene
	{
		BM_Init,//����
		BM_CenterMove,//���S�փC�[�W���O
		BM_CreateHumanModel,//�l�^�ɂȂ�
		BM_EyeMove//���E�ړ�
	};
	int bossMovieScene = BossMovieScene::BM_Init;
	const float  maxTime = 300.0f;//�S�̎���
	const int afterY = 300;

	//�{�X���[�r�[���̃J����
	const XMFLOAT3 MovieCameraPos = { 1000,300,0 };
	const XMFLOAT3 MovieCameratarget = { 0,180,0 };
	const float easeAfterTargetY = 180;

	//�{�X
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

	//�{�X�̍U���p�^�[��
	enum BossAttack
	{
		NONE,
		SlidePart,//�ߋ����U��1
		machineGun,//�ߋ����U��2
		LongDisAttack1,//�������U��1
		LongDisAttack2,//�������U��2
		RightArmBreak,//�E�r�j��
		LeftArmBreak,//���r�j��
		RightLegBreak,//�E���j��
		LeftLegBreak//�����j��
	};
	int howAttack = NONE;

	//�E�r�U���̃V�[��(�ߋ����U��1)
	int machineGunScene = 0;

	//�����œガ�����U���̃V�[��(�ߋ����U��2)
	int slidePartScene = 0;

	//�������U��1�̃V�[��
	int longDisAttack1Scene = 0;

	//�������U��2�̃V�[��
	int longDisAttack2Scene = 0;
	float partSpeedX = 0.0f;
	float partSpeedZ = 0.0f;

	//�E�r�j�󎞂̃V�[��
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

	//���r�j�󎞃V�[��
	enum Boss_LA_Broken
	{
		LA_BreakSet,//���ʂ�������Z�b�g
		LA_BreakUpdate,//���ʂ�������X�V
		LA_Init,//�C�[�W���O�̏���
		LA_UpMove,//��ֈړ�����
		LA_AttackBigPartFall,//�ォ��������Ƃ�
		LA_Undo,//�C�[�W���O�Ō��̈ʒu�ɖ߂�
	};

	//�E���j�󎞃V�[��
	enum Boss_RF_Broken
	{
		RF_BreakSet,//���ʂ�������Z�b�g
		RF_BreakUpdate,//���ʂ�������X�V
		RF_Init,//�C�[�W���O�̏���
		RF_InFrontPart,//x���W�̃}�C�i�X�����փC�[�W���O
		RF_ReturnEye,//���E��߂�
		RF_AttackBuckPart,//x���W�̃}�C�i�X��������v���X�����֔�΂��U��
		RF_Undo,//�C�[�W���O�Ō��̈ʒu�ɖ߂�
	};

	//�����j�󎞃V�[��
	enum Boss_LF_Broken
	{
		LF_BreakSet,//���ʂ�������Z�b�g
		LF_BreakUpdate,//���ʂ�������X�V
		LF_Init,//�C�[�W���O�̏���
		LF_CenterMove,//���S�փC�[�W���O
		LF_ReturnEye,//���E��߂�
		LF_AttackCircleMove,//�~����ɒ��΂�
		LF_Undo,//�C�[�W���O�Ō��̈ʒu�ɖ߂�
	};

	/// <summary>
	/// ���ʔj�󎞂̃V�[���������/
	/// 0->�E�r/1->�E�r/2->�E��/3->����
	/// </summary>
	int partBreakScene[4] = {
		Boss_RA_Broken::RA_BreakSet ,Boss_LA_Broken::LA_BreakSet,Boss_RF_Broken::RF_BreakSet,Boss_LF_Broken::LF_BreakSet };


	//������ꏊ���v���C���[�̍��W�ɂ��邽�ߋ������o��
	XMFLOAT3 fallPosDistance = { 0.0f, 0.0f, 0.0f };

	//-���r�j�󎞂Ɏg�p����-//
	//�~�点��p�[�c�̌�
	const int fallNum = 8;
	//�ꏊ�Z�b�g
	bool fallPlace[bossPartNum] = { false };

	//-�����j�󎞂Ɏg�p����-//
	const int rotationNum = 20;//��]�������
	int pushCount = rotationNum;//���˂��鋅�̔z��̈ʒu

	//�{�X�p�[�c�̒��a
	const float bossPartDiameter = 30.0f;

	//�{�X�̃p�[�c�����ʒu
	XMFLOAT3 bossPartFirstPosition[bossPartNum];
	for (int i = 0; i < bossPartNum; i++)
	{
		//��
		if (i < bossFacePartNum)
		{
			bossPartFirstPosition[i].x = -220.0f - (i % 49 / 7) * bossPartDiameter;
			bossPartFirstPosition[i].y = 760.0f - (i / 49) * bossPartDiameter;
			bossPartFirstPosition[i].z = -90.0f + (i % 7) * bossPartDiameter;
		}

		//����
		else if (i < bossFacePartNum + bossBodyPartNum)
		{
			//�O�̃p�[�c�ԍ��������p�ϐ�
			int partNumDiff = bossFacePartNum;

			bossPartFirstPosition[i].x = -280.0f - ((i - partNumDiff) % 21 / 7) * bossPartDiameter;
			bossPartFirstPosition[i].y = 550.0f - ((i - partNumDiff) / 21) * bossPartDiameter;
			bossPartFirstPosition[i].z = -90.0f + ((i - partNumDiff) % 7) * bossPartDiameter;
		}

		//�E��
		else if (i < bossFacePartNum + bossBodyPartNum + bossRightFootPartNum)
		{
			//�O�̃p�[�c�ԍ��������p�ϐ�
			int partNumDiff = bossFacePartNum + bossBodyPartNum;

			bossPartFirstPosition[i].x = -280.0f - ((i - partNumDiff) % 6 / 2) * bossPartDiameter;
			bossPartFirstPosition[i].y = 220.0f - ((i - partNumDiff) / 6) * bossPartDiameter;
			bossPartFirstPosition[i].z = -90.0f + ((i - partNumDiff) % 2) * bossPartDiameter;
		}

		//����
		else if (i < bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum)
		{
			//�O�̃p�[�c�ԍ��������p�ϐ�
			int partNumDiff = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum;

			bossPartFirstPosition[i].x = -280.0f - ((i - partNumDiff) % 6 / 2) * bossPartDiameter;
			bossPartFirstPosition[i].y = 220.0f - ((i - partNumDiff) / 6) * bossPartDiameter;
			bossPartFirstPosition[i].z = 60.0f + ((i - partNumDiff) % 2) * bossPartDiameter;
		}

		//�E�r
		else if (i < bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum)
		{
			//�O�̃p�[�c�ԍ��������p�ϐ�
			int partNumDiff = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum;

			bossPartFirstPosition[i].x = -280.0f - ((i - partNumDiff) % 30 / 10) * bossPartDiameter;
			bossPartFirstPosition[i].y = 550.0f - ((i - partNumDiff) / 30) * bossPartDiameter;
			bossPartFirstPosition[i].z = -120.0f - ((i - partNumDiff) % 10) * bossPartDiameter;
		}

		//���r
		else if (i < bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum + bossLeftArmPartNum)
		{
			//�O�̃p�[�c�ԍ��������p�ϐ�
			int partNumDiff = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum;

			bossPartFirstPosition[i].x = -280.0f - ((i - partNumDiff) % 30 / 10) * bossPartDiameter;
			bossPartFirstPosition[i].y = 550.0f - ((i - partNumDiff) / 30) * bossPartDiameter;
			bossPartFirstPosition[i].z = 120.0f + ((i - partNumDiff) % 10) * bossPartDiameter;
		}
	}

	//�{�X�p�[�c�̐F
	const XMFLOAT4 baseColor = { 1.0f, 0.0f, 0.0f, 1.0f };//�ʏ펞�̐F(��)
	const XMFLOAT4 attackColor = { 1.0f, 1.0f, 0.0f, 1.0f };//�U�����̐F(���F)
	const XMFLOAT4 damageColor = { 1.0f, 0.5f, 0.5f, 1.0f };//�_���[�W��H������Ƃ��̐F(������)


	//�{�X�������Ă��邩
	bool isBossAlive = false;

	const int bossHPMax = 400;//�{�X�̍ő�HP
	int bossHP = bossHPMax;//�{�X��HP
	const float bossHPGaugeSizeXMax = 694.0f;//�{�X��HP�p�Q�[�W�̑傫��MAX
	float bossHPGaugeSizeX = bossHPGaugeSizeXMax;//�{�X��HP�p�Q�[�W�̑傫�� 
	bool isBossDamageColor = false;//�{�X���_���[�W���󂯂����̐F�ɂ��邩
	int bossDamageTime = 0;//�{�X���_���[�W���󂯂Ă��牽�b�o������
	const int bossDamageColorTime = 5;//�{�X���_���[�W���󂯂����̐F�ɂ��鎞��

	const int tutorialDamageColorTime = 20;//�{�X���_���[�W���󂯂����̐F�ɂ��鎞��

	//�s�k�V�[��
	int loseScene = 0;

	//�����V�[��
	int winScene = 0;

	//�X�v���C�g�֌W
	XMFLOAT4 youDiedColor = { 1.0f, 1.0f, 1.0f, 0.0f };//YOUDIED�����킶��o�����߂̕ϐ�
	XMFLOAT4 youWinColor = { 1.0f, 1.0f, 1.0f, 0.0f };//YOUWIN�����킶��o�����߂̕ϐ�

	//-------------------------------------------------------------------------------------------------//

	gameSystem->afterInit(winApp);

	while (true)
	{
		input->upDate();
		//�G�X�P�[�v���~�������ꂽ�Ƃ��Q�[���I��
		if (input->PushKey(DIK_ESCAPE)) { break; }
		if (gameSystem->gameFin(winApp) == true) { break; }

		//�V�X�e���A�b�v�f�[�g
		gameSystem->upDate(winApp);

		//-------------------�X�V����------------------//
		// ������
		if (scene == SCENE::Init)
		{
			//�Ȃɂ������̂�������Ȃ��Ȃ��Ă�������ォ��S�����������Ƃ���
			//const���ĂȂ����̈ȊO�S��

			//�J�����̏��
			camera.Position = { 0.0f,0.0f,0.0f };
			camera.Target = { 0.0f,0.0f,0.0f };

			//�J�����̃C�[�W���O�p
			camera.beforePosition = { 0.0f,0.0f,0.0f };
			camera.beforeTarget = { 0.0f,0.0f,0.0f };
			camera.afterPosition = { 0.0f,0.0f,0.0f };
			camera.afterTarget = { 0.0f,0.0f,0.0f };

			//�v���C���[�̏��
			player.isAlive = true;
			player.position = { 0.0f, 50.0f, 0.0f };
			player.size = 10.0f;
			player.radius = 20.0f;

			//�e�̏��
			for (int i = 0; i < bulletNum; i++)
			{
				bullet[i].isAlive = false;
				bullet[i].position = { 0.0f, -50.0f, 0.0f };
				bullet[i].movement = { 0.0f,0.0f, 0.0f };
				bullet[i].rota = { 0.0f,0.0f, 0.0f };
				bullet[i].size = 1.0f;
				bullet[i].radius = 2.6f;
			}

			//�`���[�g���A���p�̓G�̏��
			for (int i = 0; i < tutorialEnemyNum; i++)
			{
				tutorialEnemy[i].position = { 0.0f,0.0f,0.0f };
				tutorialEnemy[i].size = { 3.0f, 3.0f, 3.0f };
				tutorialEnemy[i].color = { 1.0f, 0.0f, 0.0f, 1.0f };//�F(��)
				tutorialEnemy[i].radius = 8.0f;//�{�X�p�[�c�̔��a
				tutorialEnemy[i].isAlive = false;
				tutorialEnemy[i].isDamageColor = false;
				tutorialEnemy[i].damageTime = 0;
			}

			//�V�X�e���p�ϐ�
			length = 0.0f;

			//�C�[�W���O�̐i�s�x�p
			nowCount = 0.0f;
			timeRate = 0.0f;

			//�J�����̈���
			cameraSpeed = 1;//�J�����̑��x

			//�`���[�g���A���p
			tutorialScene = TutorialScene::TUT_Init;
			moveChack = 0;//�ǂ̂��炢��������
			moveGaugeSizeX = 0.0f;//�u�`���[�g���A���E�����v�p�Q�[�W�̑傫��
			checkTutorialEnemy = 0;//�`���[�g���A���p�̓G�|������

			//���[�r�[���̓{�X�ւ̃_���[�W���薳��
			nowMovie = true;

			//player
			playerHP = playerHPMax;//�v���C���[��HP
			isPlayerDamage = false;//�v���C���[���_���[�W��H����Ă��邩
			playerDamageTime = 0;	//�v���C���[���_���[�W��H����Ă���̎���
			isPlayerDamageRedShow = false;//��ʂ�Ԃ����邩

			//playerBullet
			bulletCount = 0;

			//�{�X�������[�r�[�p
			bossMovieScene = BossMovieScene::BM_Init;

			//�{�X
			InitBoss(boss, timeRateAttack, nowCountAttack, bossIsShot, fallPlace);

			armAttackCount = 0;
			howBossBody = 5;
			oldHowbossbody = 5;
			bossAiRand = 0;
			bossBulletCount = 0;
			bossAttackCoolTime = 0;
			moveAi = false;
			isBossAttack = false;

			//�{�X�̍U���p�^�[��
			howAttack = NONE;

			//�E�r�U���̃V�[��(�ߋ����U��1)
			machineGunScene = 0;

			//�����œガ�����U���̃V�[��(�ߋ����U��2)
			slidePartScene = 0;

			//�������U��1�̃V�[��
			longDisAttack1Scene = 0;

			//�������U��2�̃V�[��
			longDisAttack2Scene = 0;
			partSpeedX = 0.0f;
			partSpeedZ = 0.0f;

			partBreakScene[0] = Boss_RA_Broken::RA_BreakSet;
			partBreakScene[1] = Boss_LA_Broken::LA_BreakSet;
			partBreakScene[2] = Boss_RF_Broken::RF_BreakSet;
			partBreakScene[3] = Boss_LF_Broken::LF_BreakSet;


			//������ꏊ���v���C���[�̍��W�ɂ��邽�ߋ������o��
			fallPosDistance = { 0.0f, 0.0f, 0.0f };

			//-�����j�󎞂Ɏg�p����-//
			pushCount = rotationNum;//���˂��鋅�̔z��̈ʒu

			//�{�X�������Ă��邩
			isBossAlive = false;

			bossHP = bossHPMax;//�{�X��HP
			bossHPGaugeSizeX = bossHPGaugeSizeXMax;//�{�X��HP�p�Q�[�W�̑傫�� 
			isBossDamageColor = false;//�{�X���_���[�W���󂯂����̐F�ɂ��邩
			bossDamageTime = 0;//�{�X���_���[�W���󂯂Ă��牽�b�o������

			for (int i = 0; i < attackRangeNum; i++)
			{
				attackRange[i].isAlive = false;
				attackRange[i].position = { 0.0f,0.0f,0.0f };
				attackRange[i].size = 1.0f;
			}

			//�s�k�V�[��
			loseScene = 0;

			//�����V�[��
			winScene = 0;

			//�X�v���C�g�֌W
			youDiedColor = { 1.0f, 1.0f, 1.0f, 0.0f };//YOUDIED�����킶��o�����߂̕ϐ�
			youWinColor = { 1.0f, 1.0f, 1.0f, 0.0f };//YOUWIN�����킶��o�����߂̕ϐ�

			//�^�C�g���ɃV�[���ύX
			scene = Title;
		}

		//�^�C�g��
		else if (scene == SCENE::Titel)
		{
			//�{�X���[�r�[���̃J�����ɂ���
			camera.Position = MovieCameraPos;
			camera.Target = MovieCameratarget;
			cameraSpeed = 1;
			//�`���[�g���A���Ŏg�����ϐ��̏�����
			player.position = { 400.0f, 50.0f, 0.0f };

			//�`���[�g���A��
			if (input->TriggerPadButtons(Input::PAD_A) || input->TriggerKey(DIK_RETURN) == true)
			{
				//�{�^�����ʉ�
				audio->SoundPlayWava(xAudio2.Get(), Button, false);
				scene = SCENE::Tutorial;
				nowCount = 0;
			}

			//�{�X��
			if (input->TriggerPadButtons(Input::PAD_B) || input->TriggerKey(DIK_SPACE) == true)
			{
				//�{�^�����ʉ�
				audio->SoundPlayWava(xAudio2.Get(), Button, false);
				//BGM���[�v
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

		//�`���[�g���A��
		else if (scene == SCENE::Tutorial)
		{
			//�C�[�W���O�p�̃|�W�V�����Z�b�g
			if (tutorialScene == TutorialScene::TUT_Init)
			{
				camera.beforePosition = camera.Position;
				camera.beforeTarget = camera.Target;

				tutorialScene++;
			}

			//view���v���C���[���_�ɃC�[�W���O�ŕω�������
			else if (tutorialScene == TutorialScene::TUT_PlayerEyeMove)
			{
				timeRate = nowCount / maxTime;

				camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, player.position.x, timeRate));
				camera.Position.y = static_cast<float>(EASE::easeIn(camera.beforePosition.y, player.position.y, timeRate));
				camera.Position.z = static_cast<float>(EASE::easeIn(camera.beforePosition.z, player.position.z, timeRate));

				camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y / 2, 180, timeRate));

				//�C�[�W���O�̃J�E���g
				nowCount++;

				//���E�ړ����I������̂Ŏ���step��
				if (nowCount > maxTime) {
					tutorialScene++;
					nowCount = 0;
					cameraSpeed = 5;
				}
			}

			//�ړ�������
			else if (tutorialScene == TutorialScene::TUT_TryMove)
			{
				//player�ړ�
				moveChack += PlayerOperationMove(player);
				//���E�ړ�
				PlayerOperationEye(camera, Tgspeed);

				///�܂��͓����Ă݂܂��傤
				///�p�b�h��\�������X�e�B�b�N��player�ړ��A���E�ړ��A�ƕ\��
				//�u�`���[�g���A���E�����v�p�Q�[�W�̑傫��
				moveGaugeSizeX = moveChack * moveGaugeSizeXMax / moveCheckMax;

				//�r���[�̏���
				{
					camera.Position = player.position;
				}


				//���ȏ㓮�����玟�ɍs��
				if (moveChack > 100) {
					float dist = 500;//�v���C���[�ƃG�l�~�[�̋���

					//tutorial�̓I�ݒu
					for (int i = 0; i < tutorialEnemyNum; i++)
					{
						tutorialEnemy[i].position.x = player.position.x + cos(i * 36 * radian) * dist;
						tutorialEnemy[i].position.y = 20;
						tutorialEnemy[i].position.z = player.position.z + sin(i * 36 * radian) * dist;
						tutorialEnemy[i].isAlive = true;
						//�F�ς��i���F�ǉ��jI����Ȃ物�F�Ő����̂ł�
						if (i % 2 == 1)
						{
							//���F�ɂ��鏈��
							tutorialEnemy[i].color = attackColor;
						}
					}

					moveChack = 0;
					moveGaugeSizeX = 0.0f;
					tutorialScene++;
				}
			}

			//���X�Ԃ̓G���U��������
			else if (tutorialScene == TutorialScene::TUT_TryRedEnemy)
			{
				//���E�ړ�
				PlayerOperationEye(camera, Tgspeed);

				//�r���[�̏���
				{
					camera.Position = player.position;
				}

				//�v���C���[�̍U������
				if (input->TriggerPadButtons(Input::PAD_RB) || input->TriggerKey(DIK_SPACE) == true)
				{
					//�U����SE
					audio->SoundPlayWava(xAudio2.Get(), Shot, false);
					bulletCount = BulletPreparation(bulletNum, bullet[bulletCount], player, camera.Target, bulletCount);
				}

				//�v���C���[�̍U���X�V
				for (int i = 0; i < bulletNum; i++)
				{
					UpDateBullet(bullet[i]);
				}

				//�I�ƒe�̓����蔻��
				//�Ԃ��G�Ȃ瓖���蔻���ʂ�
				for (int j = 0; j < tutorialEnemyNum / 2; j++)
				{
					for (int i = 0; i < bulletNum; i++)
					{
						//�������Ă��邩�H
						bool isCollision = CollisionPlayerTutorialEnemyCheck(bullet[i], tutorialEnemy[j * 2]);

						//�������Ă�����
						if (isCollision == true)
						{
							//�G�ɍU����^�����Ƃ���SE
							audio->SoundPlayWava(xAudio2.Get(), BossDamage, false);

							//�e�͏���
							bullet[i].isAlive = false;
							tutorialEnemy[j * 2].isAlive = false;

							//�`���[�g���A���p�̓G�|�����������Z
							checkTutorialEnemy++;
						}
					}
				}

				//�Ԃ��S���|�����玟�ɍs��
				if (checkTutorialEnemy == 5)
				{
					//���F��Ԃɂ��鏈��
					for (int j = 0; j < tutorialEnemyNum / 2; j++)
					{
						tutorialEnemy[(j * 2) + 1].color = baseColor;
					}

					tutorialScene++;
				}
			}

			//���F�̓G����Ԃɂ��ē|����悤�ɂ���
			else if (tutorialScene == TutorialScene::TUT_TryYellowEnemy)
			{
				//���E�ړ�
				PlayerOperationEye(camera, Tgspeed);

				//�r���[�̏���
				{
					camera.Position = player.position;
				}

				//�v���C���[�̍U������
				if (input->TriggerPadButtons(Input::PAD_RB) || input->TriggerKey(DIK_SPACE) == true)
				{
					//�U����SE
					audio->SoundPlayWava(xAudio2.Get(), Shot, false);
					bulletCount = BulletPreparation(bulletNum, bullet[bulletCount], player, camera.Target, bulletCount);
				}

				//�v���C���[�̍U���X�V
				for (int i = 0; i < bulletNum; i++)
				{
					UpDateBullet(bullet[i]);
				}

				//�I�ƒe�̓����蔻��
				//�Ԃ��G�Ȃ瓖���蔻���ʂ�
				for (int j = 0; j < tutorialEnemyNum / 2; j++)
				{
					for (int i = 0; i < bulletNum; i++)
					{
						//�������Ă��邩�H
						bool isCollision = CollisionPlayerTutorialEnemyCheck(bullet[i], tutorialEnemy[(j * 2) + 1]);

						//�������Ă�����
						if (isCollision == true)
						{
							//�G�ɍU����^�����Ƃ���SE
							audio->SoundPlayWava(xAudio2.Get(), BossDamage, false);

							//�e�͏���
							bullet[i].isAlive = false;
							tutorialEnemy[(j * 2) + 1].isAlive = false;

							//�`���[�g���A���p�̓G�|�����������Z
							checkTutorialEnemy++;
						}
					}
				}

				//�Ԃ��S���|�����玟�ɍs��
				if (checkTutorialEnemy == 10)
				{
					tutorialScene++;
					float dist = 1000;//�v���C���[�ƃG�l�~�[�̋���

					//tutorial�̓I�ݒu
					for (int i = 0; i < tutorialEnemyNum; i++)
					{
						tutorialEnemy[i].position.x = cos(i * 36 * radian) * dist;
						tutorialEnemy[i].position.y = 150;
						tutorialEnemy[i].position.z = sin(i * 36 * radian) * dist;
						tutorialEnemy[i].isAlive = true;
					}
				}
			}

			//�D���ɍs�������A����������^�C�g����
			else if (tutorialScene == TutorialScene::TUT_Undo)
			{
				//player�ړ�
				PlayerOperationMove(player);
				//���E�ړ�
				PlayerOperationEye(camera, Tgspeed);

				//�r���[�̏���
				{
					camera.Position = player.position;
				}

				//�v���C���[�̍U������
				if (input->TriggerPadButtons(Input::PAD_RB) || input->TriggerKey(DIK_SPACE)  == true)
				{
					//�U����SE
					audio->SoundPlayWava(xAudio2.Get(), Shot, false);
					bulletCount = BulletPreparation(bulletNum, bullet[bulletCount], player, camera.Target, bulletCount);
				}

				//�v���C���[�̍U���X�V
				for (int i = 0; i < bulletNum; i++)
				{
					UpDateBullet(bullet[i]);
				}

				//�����蔻��
				for (int j = 0; j < tutorialEnemyNum; j++)
				{
					for (int i = 0; i < bulletNum; i++)
					{
						//�������Ă��邩�H
						bool isCollision = CollisionPlayerTutorialEnemyCheck(bullet[i], tutorialEnemy[j]);

						//�������Ă�����
						if (isCollision == true)
						{
							//�G�ɍU����^�����Ƃ���SE
							audio->SoundPlayWava(xAudio2.Get(), BossDamage, false);

							//�F�ς���
							if (tutorialEnemy[j].isDamageColor == false)
							{
								tutorialEnemy[j].isDamageColor = true;
							}

							//�e�͏���
							bullet[i].isAlive = false;

							break;
						}
					}
				}

				for (int i = 0; i < tutorialEnemyNum; i++)
				{
					if (tutorialEnemy[i].isDamageColor == true)
					{
						//�{�X���_���[�W��H����Ă���̎��Ԃ��v�Z
						tutorialEnemy[i].damageTime++;

						//�`���[�g���A���p�̓G�̐F��ς���
						tutorialEnemy[i].color = damageColor;



						//�w��̎��Ԃ܂Ői�񂾂猳�̐F�ɖ߂�
						if (tutorialEnemy[i].damageTime > tutorialDamageColorTime)
						{
							tutorialEnemy[i].color = baseColor;

							tutorialEnemy[i].damageTime = 0;

							//�F�ς��I��
							tutorialEnemy[i].isDamageColor = false;
						}
					}
				}

				//�p�b�hB�{�^������������^�C�g����
				if (input->TriggerPadButtons(Input::PAD_B) == true || input->TriggerKey(DIK_RETURN)  == true)
				{
					//�{�^�����ʉ�
					audio->SoundPlayWava(xAudio2.Get(), Button, false);

					for (int i = 0; i < bulletNum; i++)
					{
						bullet[i].isAlive = false;
					}

					for (int j = 0; j < tutorialEnemyNum; j++)
					{
						tutorialEnemy[j].isAlive = false;
					}
					//�`���[�g���A���p�̓G�|��������0�ɖ߂�
					checkTutorialEnemy = 0;

					nowCount = 0;
					tutorialScene = TutorialScene::TUT_Init;
					scene = SCENE::Init;
				}
			}
		}

		//�{�X���[�r�[
		else if (scene == SCENE::StartMovie)
		{
			//����
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

			//�����ɏW�܂�
			else if (bossMovieScene == BossMovieScene::BM_CenterMove)
			{
				timeRate = nowCount / maxTime;

				//���S�ɏW�܂�悤�ɃC�[�W���O
				for (int i = 0; i < bossPartNum; i++)
				{
					boss[i].angle++;
					boss[i].distance = static_cast<float>(EASE::easeIn(boss[i].beforeDistance, 0, timeRate));

					//position���f
					boss[i].position.x = cos(boss[i].angle * radian) * boss[i].distance;
					boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, afterY, timeRate));
					boss[i].position.z = sin(boss[i].angle * radian) * boss[i].distance;
				}

				//�C�[�W���O�̃J�E���g
				nowCount++;

				//���S�ɏW�܂����̂�,���̃C�[�W���O�̂��߂Ɉʒu���Z�b�g���Ď��ɍs��
				if (nowCount > maxTime) {
					for (int i = 0; i < bossPartNum; i++)
					{
						boss[i].beforePosition = boss[i].position;
					}

					bossMovieScene++;
					nowCount = 0;
				}
			}

			//�l�^�ɂȂ�
			else if (bossMovieScene == BossMovieScene::BM_CreateHumanModel)
			{
				timeRate = nowCount / maxTime;

				for (int i = 0; i < bossPartNum; i++)
				{
					//position���f
					boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
					boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
					boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
				}

				//�C�[�W���O�̃J�E���g
				nowCount++;

				//�J�E���g�_�E�����}�b�N�X�Ȃ�battle�Ɉڂ�
				if (nowCount > maxTime) {
					bossMovieScene++;
					nowCount = 0;
					camera.beforePosition = { player.position.x ,camera.Position.y,player.position.z };
					camera.beforeTarget = camera.Target;
				}
			}

			//���E�ړ�
			else if (bossMovieScene == BossMovieScene::BM_EyeMove)
			{
				timeRate = nowCount / maxTime;

				camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, player.position.x, timeRate));
				camera.Position.y = static_cast<float>(EASE::easeIn(camera.beforePosition.y / 2, player.position.y, timeRate));
				camera.Position.z = static_cast<float>(EASE::easeIn(camera.beforePosition.z, player.position.z, timeRate));

				camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y * 1.5, 180, timeRate));

				//�C�[�W���O�̃J�E���g
				nowCount++;

				//�J�E���g�_�E�����}�b�N�X�Ȃ�battle�Ɉڂ�
				if (nowCount > maxTime) {
					scene = SCENE::Battle;
					bossMovieScene = BossMovieScene::BM_Init;
					cameraSpeed = 5;
					nowMovie = false;
				}
			}
		}

		//�o�g��
		else if (scene == SCENE::Battle)
		{
			//���[�r�[�łȂ���΃v���C���[�͍s���ł���
			if (nowMovie == false)
			{
				//�v���C���[�ړ�
				PlayerOperationMove(player);
				//�v���C���[���_�ړ�
				PlayerOperationEye(camera, Tgspeed);

				//�v���C���[�̍U������
				if (input->TriggerPadButtons(Input::PAD_RB) || input->TriggerKey(DIK_SPACE) == true)
				{
					//�U����SE
					audio->SoundPlayWava(xAudio2.Get(), Shot, false);

					bulletCount = BulletPreparation(bulletNum, bullet[bulletCount], player, camera.Target, bulletCount);
				}

				//�v���C���[���_���[�W��̖��G�ł͂Ȃ���Γ����蔻����쓮
				if (isPlayerDamage == false)
				{
					//�v���C���[�ƃ{�X�̓����蔻��
					for (int i = 0; i < bossPartNum; i++)
					{
						//�������Ă��邩�H
						bool isCollision = CollisionPlayerBossPartCheck(player, boss[i]);

						//�������Ă�����
						if (isCollision == true)
						{
							//�v���C���[��damage���󂯂��Ƃ���SE
							audio->SoundPlayWava(xAudio2.Get(), PlayerDamage, false);

							//�v���C���[�Ƀ_���[�W
							playerHP--;

							//�v���C���[���_���[�W��H������疳�G���Ԃ�
							isPlayerDamage = true;

							//�v���C���[���_���[�W��H��������ʂ�Ԃ�����
							isPlayerDamageRedShow = true;

							//���i�q�b�g���Ȃ��悤�Ƀ��[�v�𔲂���
							break;
						}
					}
				}

				//�v���C���[�̃_���[�W��̖��G���Ԍv�Z
				else if (isPlayerDamage == true)
				{
					playerDamageTime++;

					//�w�肵�����Ԃ�������
					if (playerDamageTime >= playerDamageRedShowTime)
					{
						//��ʐԂ��̏I��
						isPlayerDamageRedShow = false;
					}

					//���G���Ԃ��ő�܂ł�������
					if (playerDamageTime >= playerDamageTimeMax)
					{
						//���G�I��
						isPlayerDamage = false;

						//���̎��̂��߂ɏ�����
						playerDamageTime = 0;
					}
				}

				//HP��0�ȉ��ɂȂ�ƃv���C���[���S
				if (playerHP <= 0)
				{
					//0�ȉ��ɂȂ�Ȃ��悤��
					playerHP = 0;

					player.isAlive = false;
				}
			}
			//�v���C���[�̍U���X�V
			for (int i = 0; i < bulletNum; i++)
			{
				UpDateBullet(bullet[i]);
			}

			//�{�X�������Ă�����
			if (isBossAlive == true)
			{
				//�{�X�ƒe�̓����蔻��
				for (int j = 0; j < bossPartNum; j++)
				{
					//�������Ă��邩�H
					bool isCollision = false;
					for (int i = 0; i < bulletNum; i++)
					{
						isCollision = CollisionBulletBossPartCheck(bullet[i], boss[j]);
						//�������Ă�����
						if (isCollision == true)
						{
							//�e�͏���
							bullet[i].isAlive = false;

							//�{�X�p�[�c���U�����ł͂Ȃ����
							if (boss[j].isAttack == false)
							{
								//�G�ɍU����^�����Ƃ���SE
								audio->SoundPlayWava(xAudio2.Get(), BossDamage, false);

								//�G�Ƀ_���[�W
								bossHP--;

								//�{�X�̐F���_���[�W�󂯂����̐F�ɕύX
								isBossDamageColor = true;

								//�_���[�W��H����Ă���̎��Ԃ�0�ɖ߂�
								bossDamageTime = 0;
							}

							//���i�q�b�g���Ȃ��悤�Ƀ��[�v�𔲂���
							break;
						}
					}

					//�������Ă�����
					if (isCollision == true)
					{
						//���i�q�b�g���Ȃ��悤�Ƀ��[�v�𔲂���
						break;
					}
				}

				if (isBossDamageColor == true)
				{
					//�{�X���_���[�W��H����Ă���̎��Ԃ��v�Z
					bossDamageTime++;

					for (int i = 0; i < bossPartNum; i++)
					{
						//�����Ă��違�U�����ł͂Ȃ��p�[�c�̐F��ς���
						if (boss[i].isAlive == true && boss[i].isAttack == false)
						{
							boss[i].color = damageColor;
						}
					}

					//�w��̎��Ԃ܂Ői�񂾂猳�̐F�ɖ߂�
					if (bossDamageTime > bossDamageColorTime)
					{
						for (int i = 0; i < bossPartNum; i++)
						{
							//�����Ă��違�U�����ł͂Ȃ��p�[�c�̐F��߂�
							if (boss[i].isAlive == true && boss[i].isAttack == false)
							{
								boss[i].color = baseColor;
							}
						}

						//�F�ς��I��
						isBossDamageColor = false;
					}
				}

				//HP��0�ȉ��ɂȂ�ƃ{�X���S
				if (bossHP <= 0)
				{
					//0�ȉ��ɂȂ�Ȃ��悤��
					bossHP = 0;

					isBossAlive = false;
				}
			}

			//�{�X��HP�p�Q�[�W�̑傫��
			bossHPGaugeSizeX = bossHP * bossHPGaugeSizeXMax / bossHPMax;

			//���ʔj�󂳂��HP�����ׂ邽�߂Ɍ��t���[����HP�c�ʂ��L�^����
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

			//�E�r���j�󂳂�Ă���
			if (oldHowbossbody == 5 && howBossBody == 4)
			{
				for (int i = 0; i < 850; i++)
				{
					boss[i].position = bossPartFirstPosition[i];
					boss[i].beforePosition = { 0,0,0 };
					boss[i].afterPosition = { 0,0,0 };

					//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
					boss[i].isAttack = false;

					//�{�X�p�[�c�̐F��Ԃɖ߂�
					boss[i].color = baseColor;
				}
				howAttack = BossAttack::RightArmBreak;
				slidePartScene = 0;
				machineGunScene = 0;
				longDisAttack1Scene = 0;
				longDisAttack2Scene = 0;
			}
			//���r���j�󂳂�Ă���
			else if (oldHowbossbody == 4 && howBossBody == 3)
			{
				for (int i = 0; i < 850; i++)
				{
					boss[i].position = bossPartFirstPosition[i];
					boss[i].beforePosition = { 0,0,0 };
					boss[i].afterPosition = { 0,0,0 };

					//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
					boss[i].isAttack = false;

					//�{�X�p�[�c�̐F��Ԃɖ߂�
					boss[i].color = baseColor;
				}
				howAttack = BossAttack::LeftArmBreak;
				slidePartScene = 0;
				machineGunScene = 0;
				longDisAttack1Scene = 0;
				longDisAttack2Scene = 0;
			}
			//�E�����j�󂳂�Ă���
			else if (oldHowbossbody == 3 && howBossBody == 2)
			{
				for (int i = 0; i < 850; i++)
				{
					boss[i].position = bossPartFirstPosition[i];
					boss[i].beforePosition = { 0,0,0 };
					boss[i].afterPosition = { 0,0,0 };

					//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
					boss[i].isAttack = false;

					//�{�X�p�[�c�̐F��Ԃɖ߂�
					boss[i].color = baseColor;
				}
				howAttack = BossAttack::RightLegBreak;
				slidePartScene = 0;
				machineGunScene = 0;
				longDisAttack1Scene = 0;
				longDisAttack2Scene = 0;
			}
			//�������j�󂳂�Ă���
			else if (oldHowbossbody == 2 && howBossBody == 1)
			{
				for (int i = 0; i < 850; i++)
				{
					boss[i].position = bossPartFirstPosition[i];
					boss[i].beforePosition = { 0,0,0 };
					boss[i].afterPosition = { 0,0,0 };

					//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
					boss[i].isAttack = false;

					//�{�X�p�[�c�̐F��Ԃɖ߂�
					boss[i].color = baseColor;
				}
				howAttack = BossAttack::LeftLegBreak;
				slidePartScene = 0;
				machineGunScene = 0;
				longDisAttack1Scene = 0;
				longDisAttack2Scene = 0;
			}
			//�S�Ĕj�󂳂�Ă���
			else if (oldHowbossbody == 1 && howBossBody == 0)
			{
				for (int i = 0; i < 850; i++)
				{
					boss[i].position = bossPartFirstPosition[i];
					boss[i].beforePosition = { 0,0,0 };
					boss[i].afterPosition = { 0,0,0 };

					//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
					boss[i].isAttack = false;

					//�{�X�p�[�c�̐F��Ԃɖ߂�
					boss[i].color = baseColor;
				}

				slidePartScene = 0;
				machineGunScene = 0;
				longDisAttack1Scene = 0;
				longDisAttack2Scene = 0;
			}

			//old��howBossBody����
			oldHowbossbody = howBossBody;

			//�{�X�ƃv���C���[�Ƃ̋���
			if (isBossAlive == true)
			{
				length = sqrtf((player.position.x - boss[25].position.x) * (player.position.x - boss[25].position.x) + (player.position.z - boss[25].position.z) * (player.position.z - boss[25].position.z));
			}


			//�{�X�̍U������
			if (isBossAttack == true)
			{
				//�s���p�^�[���p�����̐���
				moveAi = true;
				if (moveAi == true)
				{
					bossAiRand = rand() % 100;
				}
				//�v���C���[�ƃ{�X�Ƃ̋������߂��Ƃ�
				if (length < 500)
				{
					if (howBossBody >= 3)
					{
						if (bossAiRand < 30)
						{
							//�������U��1
							howAttack = LongDisAttack1;
						} else if (bossAiRand < 60)
						{
							//�������U��2
							howAttack = LongDisAttack2;
						} else if (bossAiRand < 80)
						{
							//�ߋ����U��2
							howAttack = machineGun;
						} else
						{
							//�ߋ����U��1
							howAttack = SlidePart;
						}
					} else if (howBossBody <= 2)
					{
						if (bossAiRand < 45)
						{
							//�������U��2
							howAttack = LongDisAttack2;
						} else if (bossAiRand < 75)
						{
							//�������U��1
							howAttack = LongDisAttack1;
						} else
						{
							//�ߋ����U��2
							howAttack = machineGun;
						}
					}
					moveAi = false;
					isBossAttack = false;
				}
				//�v���C���[�ƃ{�X�Ƃ̋����������Ƃ�
				else
				{
					if (howBossBody >= 3)
					{
						if (bossAiRand < 35)
						{
							//�ߋ����U��1
							howAttack = SlidePart;
						} else if (bossAiRand < 70)
						{
							//�ߋ����U��2
							howAttack = machineGun;
						} else if (bossAiRand < 85)
						{
							//�������U��1
							howAttack = LongDisAttack1;
						} else
						{
							//�������U��2
							howAttack = LongDisAttack2;
						}
					} else if (howBossBody <= 2)
					{
						if (bossAiRand < 25)
						{
							//�������U��1
							howAttack = LongDisAttack1;
						} else if (bossAiRand < 50)
						{
							//�������U��2
							howAttack = LongDisAttack2;
						} else
						{
							//�ߋ����U��2
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

			//�ߋ����U��1
			if (howAttack == BossAttack::SlidePart)
			{
				if (slidePartScene == 0)
				{
					nowCount = 0;
					slidePartScene = 1;
					for (int i = 574; i < 670; i++)
					{
						//�C�[�W���O�����p�̑��
						boss[i].beforePosition = boss[i].position;
						boss[i].afterPosition.x = 800.0f - ((i - 574) % 20) * bossPartDiameter;
						boss[i].afterPosition.y = 74.0f - ((i - 574) / 30) * bossPartDiameter;
						boss[i].afterPosition.z = -800.0f + ((i - 574) % 30 / 10) * bossPartDiameter;

						//�U���Ɏg�p����{�X�p�[�c�͍U�����ɂ��Ă���
						boss[i].isAttack = true;

						//�{�X�p�[�c�̐F��ύX
						boss[i].color = attackColor;
					}
				} else if (slidePartScene == 1)
				{
					int countNum = 100;
					timeRate = nowCount / countNum;
					//��������v���C���[�̉E���ɔ�΂�
					for (int i = 574; i < 670; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//���S�ɏW�܂����̂Ŏ��ɍs��
					if (nowCount > countNum)
					{
						//�C�[�W���O�����p�̑��
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
					//�ガ��������
					for (int i = 574; i < 670; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//���S�ɏW�܂����̂Ŏ��ɍs��
					if (nowCount > countNum)
					{
						//�C�[�W���O�����p�̑��
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
					//�ŏ��̈ʒu�ɋA���Ă���
					for (int i = 574; i < 670; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//���S�ɏW�܂����̂Ŏ��ɍs��
					if (nowCount > countNum)
					{
						for (int i = 574; i < 670; i++)
						{
							//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
							boss[i].isAttack = false;

							//�{�X�p�[�c�̐F��Ԃɖ߂�
							boss[i].color = baseColor;
						}

						nowCount = 0;
						//�U�����Ȃ��ɂ���
						howAttack = NONE;
						//�U���V�[����0��
						slidePartScene = 0;
					}
				}
			}

			//���𗐎˂���U��
			if (howAttack == BossAttack::machineGun)
			{
				if (machineGunScene == 0)
				{
					nowCount = 0;
					machineGunScene = 1;
					//�C�[�W���O�����p�̑��
					for (int i = 0; i < 343; i++)
					{
						boss[i].beforePosition = boss[i].position;
						boss[i].afterPosition = boss[171].position;

						//�U���Ɏg�p����{�X�p�[�c�͍U�����ɂ��Ă���
						boss[i].isAttack = true;

						//�{�X�p�[�c�̐F��ύX
						boss[i].color = attackColor;
					}
				} else if (machineGunScene == 1)
				{
					int countNum = 100;
					timeRate = nowCount / countNum;
					//�E�r���ꂩ���ɏW�߂鏈��
					for (int i = 0; i < 343; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//���S�ɏW�܂����̂Ŏ��ɍs��
					if (nowCount > countNum)
					{
						//�C�[�W���O�����p�̑��
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
					//�}�V���K���̂悤�ɔ�΂�����(�኱�̃z�[�~���O����)
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
						//�C�[�W���O�����p�̑��
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
					//�v���C���[�ɓ�����Ȃ��悤�ɏ�ɔ�΂�
					for (int i = 0; i < 343; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//���S�ɏW�܂����̂Ŏ��ɍs��
					if (nowCount > returnCount)
					{
						nowCount = 0;
						machineGunScene = 4;
						//�C�[�W���O�����p�̑��
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
					//���̈ʒu�ɖ߂�
					for (int i = 0; i < 343; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
					}
					nowCount++;

					//���S�ɏW�܂����̂Ŏ��ɍs��
					if (nowCount > returnArmCount)
					{
						nowCount = 0;
						howAttack = NONE;
						machineGunScene = 0;
						for (int i = 0; i < 343; i++)
						{
							//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
							boss[i].isAttack = false;

							//�{�X�p�[�c�̐F��Ԃɖ߂�
							boss[i].color = baseColor;
						}
					}
				}
			}

			//�������U��1
			else if (howAttack == BossAttack::LongDisAttack1)
			{
				//������
				if (longDisAttack1Scene == 0)
				{
					nowCount = 0;
					longDisAttack1Scene = 1;

					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//���̂̐^�񒆂̕����������w��
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
							boss[i].beforePosition = boss[i].position;

							//�Y��ɉ~��ɕ��ׂ�
							boss[i].angle = ((i - (bossFacePartNum + 63)) - (((i - (bossFacePartNum + 63)) / 7) * 4)) / 36.0f * 360.0f;

							//�C�[�W���O�G���h�͖��t���[���v�Z����ƃo�O��̂ł����ŕʂ̕ϐ��ɑ��
							boss[i].afterPosition.x = cos(boss[i].angle * radian) * 200 + bossPartFirstPosition[25].x;
							boss[i].afterPosition.y = 45;
							boss[i].afterPosition.z = sin(boss[i].angle * radian) * 200;

							//�U���Ɏg�p����{�X�p�[�c�͍U�����ɂ��Ă���
							boss[i].isAttack = true;

							//�{�X�p�[�c�̐F��ύX
							boss[i].color = attackColor;
						}
					}
				}

				//�{�X�p�[�c���~��Ɉړ�������
				else if (longDisAttack1Scene == 1)
				{
					int maxCount = 120;
					timeRate = nowCount / maxCount;
					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//���̂̐^�񒆂̕����������w��
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeOut(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
						}
					}

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�J�E���g�_�E�����}�b�N�X�Ȃ甭�˂Ɉڂ�
					if (nowCount > maxCount) {
						longDisAttack1Scene = 2;
						nowCount = 0;

						for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
						{
							//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
							boss[i].beforePosition = boss[i].position;

							//���̂̐^�񒆂̕����������w��
							if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
							{
								//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
								boss[i].beforePosition = boss[i].position;

								//�Y��ɉ~��ɕ��ׂ�
								boss[i].angle = ((i - (bossFacePartNum + 63)) - (((i - (bossFacePartNum + 63)) / 7) * 4)) / 36.0f * 360.0f;

								//�C�[�W���O�G���h�͖��t���[���v�Z����ƃo�O��̂ł����ŕʂ̕ϐ��ɑ��
								boss[i].afterPosition.x = cos(boss[i].angle * radian) * 1500 + bossPartFirstPosition[25].x;
								boss[i].afterPosition.y = 45;
								boss[i].afterPosition.z = sin(boss[i].angle * radian) * 1500;
							}
						}
					}
				}

				//�{�X�p�[�c���~��Ɉړ�������
				else if (longDisAttack1Scene == 2)
				{
					int maxCount = 90;
					timeRate = nowCount / maxCount;
					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//���̂̐^�񒆂̕����������w��
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							boss[i].position.x = static_cast<float>(EASE::lerp(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::lerp(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::lerp(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
						}
					}

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�J�E���g�_�E�����}�b�N�X�Ȃ�㏸�Ɉڂ�
					if (nowCount > maxCount) {
						longDisAttack1Scene = 3;
						nowCount = 0;

						for (int i = 0; i < 36; i++)
						{
							//���̂̐^�񒆂̕����������w��
							if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
							{
								//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
								boss[i].beforePosition = boss[i].position;
							}
						}
					}
				}

				//�A��Ƃ��Ƀv���C���[�ɓ�����Ȃ��悤�ɏ㏸
				else if (longDisAttack1Scene == 3)
				{
					int maxCount = 100;
					timeRate = nowCount / maxCount;
					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//���̂̐^�񒆂̕����������w��
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, 300, timeRate));
						}
					}

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�J�E���g�_�E�����}�b�N�X�Ȃ�Back�Ɉڂ�
					if (nowCount > maxCount) {
						longDisAttack1Scene = 4;
						nowCount = 0;

						for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
						{
							//���̂̐^�񒆂̕����������w��
							if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
							{
								//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
								boss[i].beforePosition = boss[i].position;
							}
						}
					}
				}

				//�U��΂������̂����̏ꏊ�ɖ߂�
				else if (longDisAttack1Scene == 4)
				{
					int maxCount = 200;
					timeRate = nowCount / maxCount;
					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//���̂̐^�񒆂̕����������w��
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeOut(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
						}
					}

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�J�E���g�_�E�����}�b�N�X�Ȃ�End�Ɉڂ�
					if (nowCount > maxCount) {
						longDisAttack1Scene = 5;
						nowCount = 0;
					}
				}

				//�������U��1�I��
				else if (longDisAttack1Scene == 5)
				{
					for (int i = bossFacePartNum + 63; i < bossFacePartNum + 63 + 84; i++)
					{
						//���̂̐^�񒆂̕����������w��
						if ((i % 7) == 2 || (i % 7) == 3 || (i % 7) == 4)
						{
							//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
							boss[i].isAttack = false;

							//�{�X�p�[�c�̐F��Ԃɖ߂�
							boss[i].color = baseColor;
						}
					}

					//���̂Ƃ��̂��߂ɖ߂��Ă���
					longDisAttack1Scene = 0;

					howAttack = BossAttack::NONE;
				}
			}

			//�������U��2
			else if (howAttack == BossAttack::LongDisAttack2)
			{
				//������
				if (longDisAttack2Scene == 0)
				{
					nowCount = 0;
					longDisAttack2Scene = 1;

					for (int i = 0; i < bossFacePartNum; i++)
					{
						//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
						boss[i].beforePosition = boss[i].position;

						//�C�[�W���O�G���h�͖��t���[���v�Z����ƃo�O��̂ł����ŕʂ̕ϐ��ɑ��
						boss[i].afterPosition.x = boss[i].position.x + 100;
						boss[i].afterPosition.y = boss[i].position.y - 570;

						//�U���Ɏg�p����{�X�p�[�c�͍U�����ɂ��Ă���
						boss[i].isAttack = true;

						//�{�X�p�[�c�̐F��ύX
						boss[i].color = attackColor;
					}
				}

				//�{�X�̓����O������
				else if (longDisAttack2Scene == 1)
				{
					int maxCount = 60;
					timeRate = nowCount / maxCount;
					for (int i = 0; i < bossFacePartNum; i++)
					{
						boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
						boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
					}

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�J�E���g�_�E�����}�b�N�X�Ȃ�ړ��Ɉڂ�
					if (nowCount > maxCount) {
						longDisAttack2Scene = 2;
						nowCount = 0;

						//���ʔj��x�����ɂ���đ��x���قȂ�
						//���N��
						if (howBossBody == 5)
						{
							partSpeedX = 10.0f;
							partSpeedZ = 6.0f;
						}

						//�E�r�j��
						else if (howBossBody == 4)
						{
							partSpeedX = 11.0f;
							partSpeedZ = -7.0f;
						}

						//���r�j��
						else if (howBossBody == 3)
						{
							partSpeedX = 12.0f;
							partSpeedZ = 8.0f;
						}

						//�E���j��
						else if (howBossBody == 2)
						{
							partSpeedX = 13.0f;
							partSpeedZ = -9.0f;
						}

						//�����j��
						else if (howBossBody == 1)
						{
							partSpeedX = 20.0f;
							partSpeedZ = 15.0f;
						}
					}
				}

				//�{�X�̓����ړ�������
				else if (longDisAttack2Scene == 2)
				{
					//�{�X�̓��𓮂���
					for (int i = 0; i < bossFacePartNum; i++)
					{
						boss[i].position.x += partSpeedX;
						boss[i].position.z += partSpeedZ;
					}

					//��x�ǂɓ���������V�[����ύX
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

				//�{�X�̓��𕽍s�Ɉړ�������
				else if (longDisAttack2Scene == 3)
				{
					//���ʂɃJ�E���g
					nowCount++;

					//�J�E���g�I���t���[����
					int maxCount = 600;

					//�{�X�𓮂���
					for (int i = 0; i < bossFacePartNum; i++)
					{
						boss[i].position.x += partSpeedX;
						boss[i].position.z += partSpeedZ;
					}

					//�ǂɓ��������甽�˂�����
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

					//�J�E���g�_�E������莞�Ԍo������ꎞ��~�Ɉڂ�
					if (nowCount > maxCount) {
						longDisAttack2Scene = 4;
						nowCount = 0;
					}
				}

				//�{�X���ꎞ��~������
				else if (longDisAttack2Scene == 4)
				{
					//���ʂɃJ�E���g
					nowCount++;

					//�J�E���g�I���t���[����
					int maxCount = 60;

					//�J�E���g�_�E������莞�Ԍo������Back�Ɉڂ�
					if (nowCount > maxCount) {
						longDisAttack2Scene = 5;
						nowCount = 0;

						for (int i = 0; i < bossPartNum; i++)
						{
							//�����Ă���p�[�c����
							if (boss[i].isAlive == true)
							{
								//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
								boss[i].beforePosition = boss[i].position;
							}
						}
					}
				}

				//�{�X�����̏�Ԃɖ߂�
				else if (longDisAttack2Scene == 5)
				{
					int maxCount = 200;
					timeRate = nowCount / maxCount;

					//�C�[�W���O�����Ĉړ�
					for (int i = 0; i < bossPartNum; i++)
					{
						//�����Ă���p�[�c����
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeOut(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeOut(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeOut(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
						}
					}

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�J�E���g�_�E�����}�b�N�X�Ȃ�End�Ɉڂ�
					if (nowCount > maxCount) {
						longDisAttack2Scene = 6;
						nowCount = 0;
					}
				}

				//�������U��2�I��
				else if (longDisAttack2Scene == 6)
				{
					for (int i = 0; i < bossFacePartNum; i++)
					{
						//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
						boss[i].isAttack = false;

						//�{�X�p�[�c�̐F��Ԃɖ߂�
						boss[i].color = baseColor;
					}

					//���̂Ƃ��̂��߂ɖ߂��Ă���
					longDisAttack2Scene = 0;

					howAttack = BossAttack::NONE;
				}
			}

			//-HP���ȉ��ɂȂ������̓��ʍs��-//
			//�E�r�j��
			else if (howAttack == BossAttack::RightArmBreak)
			{
				//������p�[�c�̔ԍ�
				int partNumberStart = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum;
				int partNumberLast = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum;

				//���ʂ�������Z�b�g
				if (partBreakScene[0] == Boss_RA_Broken::RA_BreakSet)
				{
					int partNum = partNumberLast - partNumberStart;

					//�܂������鏈���̃Z�b�g���������Ė�����΃Z�b�g����
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						if (boss[i].isBreak == 0)
						{
							boss[i].isBreak = rand() % partNum;
							boss[i].beforePosition = boss[i].position;
						}
					}

					//���[�r�[�n
					nowMovie = true;
					camera.Position = MovieCameraPos;
					camera.afterTarget = camera.Target;//���ɖ߂��悤�Ɉʒu���L�^
					camera.Target = MovieCameratarget;
					camera.beforeTarget = camera.Target;//�C�[�W���O�悤�Ɍ��̈ʒu���L�^

					//�Z�b�g�����̂Ŏ��ɍs��
					partBreakScene[0] = Boss_RA_Broken::RA_BreakUpdate;
					nowCount = 0;

					//������悤�ɂ���
					for (int i = 0; i < bossPartNum; i++)
					{
						fallPlace[i] = true;
					}
				}

				//���ʂ�������X�V
				else if (partBreakScene[0] == Boss_RA_Broken::RA_BreakUpdate)
				{
					const int maxCount = 100;
					//�����鏈��
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

					//�J�E���g��i�߂�
					nowCount++;

					//�����I������玟�ɍs��
					if (nowCount > (partNumberLast - partNumberStart) * 3 + maxCount)
					{
						partBreakScene[0] = Boss_RA_Broken::RA_Init;
					}
				}

				//������
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Init)
				{
					for (int i = 574; i < 670; i++)
					{
						//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
						boss[i].beforePosition = boss[i].position;
					}

					partBreakScene[0] = Boss_RA_Broken::RA_Move;
					nowCount = 0;
				}

				//���̃p�[�c���E�r�Ɏ����Ă���
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

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�J�E���g�_�E�����}�b�N�X�Ȃ�Stay1�Ɉڂ�
					if (nowCount > maxCount) {
						for (int i = 0; i < bossPartNum; i++)
						{
							//�U���Ɏg�p����{�X�p�[�c�͍U�����ɂ��Ă���
							boss[i].isAttack = true;

							//�{�X�p�[�c�̐F��ύX
							boss[i].color = attackColor;
						}

						partBreakScene[0] = Boss_RA_Broken::RA_Stay1;
						nowCount = 0;
					}
				}

				//��莞�Ԏ~�܂�
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Stay1)
				{
					//���ʂɃJ�E���g
					nowCount++;

					//�J�E���g�I���t���[����
					int maxCount = 100;

					//�J�E���g�_�E������莞�Ԍo������Fall�Ɉڂ�
					if (nowCount > maxCount) {
						for (int i = 0; i < bossPartNum; i++)
						{
							//�����Ă���p�[�c����
							if (boss[i].isAlive == true)
							{
								//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
								boss[i].beforePosition = boss[i].position;

								//�C�[�W���O�G���h�͖��t���[���v�Z����ƃo�O��̂ł����ŕʂ̕ϐ��ɑ��
								boss[i].afterPosition.y = boss[i].position.y + 2000;
							}
						}

						partBreakScene[0] = Boss_RA_Broken::RA_Fry;
						nowCount = 0;
					}
				}

				//�^��ɔ��
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Fry)
				{
					int maxCount = 60;
					timeRate = nowCount / maxCount;

					//�C�[�W���O�����Ĉړ�
					for (int i = 0; i < bossPartNum; i++)
					{
						//�����Ă���p�[�c����
						if (boss[i].isAlive == true)
						{
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						}
					}

					//�J�����̃^�[�Q�b�g�̃C�[�W���O
					float afterCameraPos = 200.0f;
					camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, afterCameraPos, timeRate));

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�J�E���g�_�E�����}�b�N�X�Ȃ�Stay2�Ɉڂ�
					if (nowCount > maxCount) {
						partBreakScene[0] = Boss_RA_Broken::RA_Stay2;
						nowCount = 0;
						camera.beforeTarget = camera.Target;

						//�{�X�̑傫����3�{�ɂ���
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

				//��莞�Ԏ~�܂�
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Stay2)
				{
					//���ʂɃJ�E���g
					nowCount++;

					//�J�E���g�I���t���[����
					int maxCount = 400;

					timeRate = nowCount / (maxCount / 2.0f);

					//�J�����C�[�W���O
					if (nowMovie == true)
					{
						camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, player.position.x, timeRate));
						camera.Position.y = static_cast<float>(EASE::easeIn(camera.beforePosition.y, player.position.y, timeRate));
						camera.Position.z = static_cast<float>(EASE::easeIn(camera.beforePosition.z, player.position.z, timeRate));

						camera.Target.x = static_cast<float>(EASE::easeIn(camera.beforeTarget.x, camera.afterTarget.x, timeRate));
						camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, easeAfterTargetY, timeRate));
						camera.Target.z = static_cast<float>(EASE::easeIn(camera.beforeTarget.z, camera.afterTarget.z, timeRate));

						//���[�r�[���I���̂ő���\�ɂ���
						if (nowCount > maxCount / 2) { nowMovie = false; }
					}

					//�����Ă���ڈ����v���C���[�̍��W�Ɉړ�������
					attackRange[0].position.x = player.position.x;
					attackRange[0].position.y = 1.0f;
					attackRange[0].position.z = player.position.z;
					attackRange[0].size = 150.0f;
					attackRange[0].color = { 0.9, 0.9, 0.1, 1 };
					attackRange[0].isAlive = true;

					//�J�E���g�_�E������莞�Ԍo������Fall�Ɉڂ�
					if (nowCount > maxCount) {
						//������ꏊ���v���C���[�̍��W�ɂ��邽�ߋ������o��
						//�{�X�p�[�c25�����傤�ǃ{�X��x,z�̒��S
						fallPosDistance.x = boss[24].position.x - player.position.x;
						fallPosDistance.z = boss[24].position.z - player.position.z;

						//�����Ă���ڈ��̐F��ς���
						attackRange[0].color = { 0.9, 0.1, 0.1, 1 };

						for (int i = 0; i < bossPartNum; i++)
						{
							//�����Ă���p�[�c����
							if (boss[i].isAlive == true)
							{
								//�{�X�̃p�[�c���v���C���[�̍��W��
								boss[i].position.x -= fallPosDistance.x;
								boss[i].position.z -= fallPosDistance.z;

								//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
								boss[i].beforePosition = boss[i].position;

								//�C�[�W���O�G���h�͖��t���[���v�Z����ƃo�O��̂ł����ŕʂ̕ϐ��ɑ��
								boss[i].afterPosition.y = boss[i].position.y - 2250;
							}
						}

						partBreakScene[0] = Boss_RA_Broken::RA_Fall;
						nowCount = 0;
					}
				}

				//�v���C���[�̍��W�ɗ����Ă���
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Fall)
				{
					int maxCount = 60;
					timeRate = nowCount / maxCount;

					//�C�[�W���O�����Ĉړ�
					for (int i = 0; i < bossPartNum; i++)
					{
						//�����Ă���p�[�c����
						if (boss[i].isAlive == true)
						{
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
						}
					}

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�J�E���g�_�E�����}�b�N�X�Ȃ�Stay3�Ɉڂ�
					if (nowCount > maxCount) {
						partBreakScene[0] = Boss_RA_Broken::RA_Stay3;
						nowCount = 0;
					}
				}

				//��莞�Ԏ~�܂�
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Stay3)
				{
					//���ʂɃJ�E���g
					nowCount++;

					//�J�E���g�I���t���[����
					int maxCount = 200;

					//�J�E���g�_�E������莞�Ԍo������Back�Ɉڂ�
					if (nowCount > maxCount) {
						for (int i = 0; i < bossPartNum; i++)
						{
							//�����Ă���p�[�c����
							if (boss[i].isAlive == true)
							{
								//�C�[�W���O�̃X�^�[�g���W�͓����Ȃ��悤�ɕʂ̕ϐ��ɑ��
								boss[i].beforePosition = boss[i].position;
							}
						}
						//�����Ă���ڈ��̂��̂�����
						attackRange[0].isAlive = false;
						partBreakScene[0] = Boss_RA_Broken::RA_Back;
						nowCount = 0;
					}
				}

				//�E�r�ƂȂ��Ă����������̏ꏊ�ɖ߂�
				else if (partBreakScene[0] == Boss_RA_Broken::RA_Back)
				{
					int maxCount = 200;
					timeRate = nowCount / maxCount;

					//�C�[�W���O�����Ĉړ�
					for (int i = 0; i < bossPartNum; i++)
					{
						//�����Ă���p�[�c����
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

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�J�E���g�_�E�����}�b�N�X�Ȃ�End�Ɉڂ�
					if (nowCount > maxCount) {
						partBreakScene[0] = Boss_RA_Broken::RA_End;
						nowCount = 0;
					}
				}

				//�E�r�j�󎞂̓��ʍU���I��
				else if (partBreakScene[0] == Boss_RA_Broken::RA_End)
				{
					for (int i = 0; i < bossPartNum; i++)
					{
						fallPlace[i] = false;

						//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
						boss[i].isAttack = false;

						//�{�X�p�[�c�̐F��Ԃɖ߂�
						boss[i].color = baseColor;
					}

					howAttack = BossAttack::NONE;
					partBreakScene[0] = Boss_RA_Broken::RA_BreakSet;
				}
			}

			//���r�j��
			else if (howAttack == BossAttack::LeftArmBreak)
			{
				//������p�[�c�̔ԍ�
				int partNumberStart = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum;
				int partNumberLast = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum + bossRightArmPartNum + bossLeftArmPartNum;

				//���ʂ�������Z�b�g
				if (partBreakScene[1] == Boss_LA_Broken::LA_BreakSet)
				{
					int partNum = partNumberLast - partNumberStart;

					//�܂������鏈���̃Z�b�g���������Ė�����΃Z�b�g����
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						if (boss[i].isBreak == 0)
						{
							boss[i].isBreak = rand() % partNum;
							boss[i].beforePosition = boss[i].position;
						}
					}

					//���[�r�[�n
					nowMovie = true;
					camera.Position = MovieCameraPos;
					camera.afterTarget = camera.Target;//���ɖ߂��悤�Ɉʒu���L�^
					camera.Target = MovieCameratarget;
					camera.beforeTarget = camera.Target;//�C�[�W���O�悤�Ɍ��̈ʒu���L�^

					for (int i = 0; i < bossPartNum; i++)
					{
						fallPlace[i] = true;
					}

					//�Z�b�g�����̂Ŏ��ɍs��
					partBreakScene[1]++;
					nowCount = 0;
				}

				//���ʂ�������X�V
				else if (partBreakScene[1] == Boss_LA_Broken::LA_BreakUpdate)
				{
					const int maxCount = 100;
					//�����鏈��
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

					//�J�E���g��i�߂�
					nowCount++;

					//�����I������玟�ɍs��
					if (nowCount > (partNumberLast - partNumberStart) * 3 + maxCount)
					{
						partBreakScene[1]++;
						nowCount = 0;
					}
				}

				//�C�[�W���O�̏���
				else if (partBreakScene[1] == Boss_LA_Broken::LA_Init)
				{
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].beforePosition = boss[i].position;
							boss[i].afterPosition.x = static_cast<float>((rand() % 4000) - 2000);
							boss[i].afterPosition.z = static_cast<float>((rand() % 4000) - 2000);

							//�U���Ɏg�p����{�X�p�[�c�͍U�����ɂ��Ă���
							boss[i].isAttack = true;

							//�{�X�p�[�c�̐F��ύX
							boss[i].color = attackColor;
						}
					}

					partBreakScene[1]++;
					nowCount = 0;
				}

				//��ֈړ�����
				else if (partBreakScene[1] == Boss_LA_Broken::LA_UpMove)
				{
					timeRate = nowCount / maxTime;

					//\��ɂ΂炯�Ȃ���C�[�W���O
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, boss[i].afterPosition.x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, 2000, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
						}
					}

					//�J�����̃^�[�Q�b�g�̃C�[�W���O
					float afterCameraTarget = 200.0f;
					camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, afterCameraTarget, timeRate));

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�C�[�W���O���I������̂ŏꏊ���Z�b�g���Ď��ɍs��
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

				//�ォ��������Ƃ�
				else if (partBreakScene[1] == Boss_LA_Broken::LA_AttackBigPartFall)
				{
					nowCount++;
					int maxCount = 80;
					timeRate = nowCount / maxCount;

					//�J�����C�[�W���O
					if (nowMovie == true)
					{
						camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, player.position.x, timeRate));
						camera.Position.y = static_cast<float>(EASE::easeIn(camera.beforePosition.y, player.position.y, timeRate));
						camera.Position.z = static_cast<float>(EASE::easeIn(camera.beforePosition.z, player.position.z, timeRate));

						camera.Target.x = static_cast<float>(EASE::easeIn(camera.beforeTarget.x, camera.afterTarget.x, timeRate));
						camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, easeAfterTargetY, timeRate));
						camera.Target.z = static_cast<float>(EASE::easeIn(camera.beforeTarget.z, camera.afterTarget.z, timeRate));

						//���[�r�[���I���̂ő���\�ɂ���
						if (nowCount > maxCount) { nowMovie = false; }
					}


					const float fallSpeed = 10;
					int nowFall = static_cast<int>(nowCount) / 100;//�������Ă���u���b�N�̐�
					//���̃J�E���g�ȉ��Ȃ痎�Ƃ�
					for (int i = 0; i < fallNum; i++)
					{
						if (i < nowFall && fallPlace[i] == false)
						{
							//�v���C���[��ŏꏊ���Z�b�gX
							int random = (rand() % 100) - 50;
							boss[i].position.x = player.position.x + random;
							//�����Ă���ڈ����v���C���[�̍��W�Ɉړ�������
							attackRange[i].position.x = player.position.x + random;

							//�v���C���[��ŏꏊ���Z�b�gZ
							random = (rand() % 100) - 50;
							boss[i].position.z = player.position.z + random;
							attackRange[i].position.y = 1.0f;
							attackRange[i].position.z = player.position.z + random;
							attackRange[i].size = 150.0f;
							attackRange[i].color = { 0.9, 0.1, 0.1, 1 };
							attackRange[i].isAlive = true;

							//�Z�b�g�����̂�true�ɂ���
							fallPlace[i] = true;
						}

						//���Ƃ�
						else if (fallPlace[i] == true)
						{
							boss[i].position.y = boss[i].position.y - fallSpeed;
							if (boss[i].position.y < 10&& attackRange[i].isAlive == true)
							{
								attackRange[i].isAlive = false;
							}
						}
					}

					//�S�ė��Ƃ��I�����̂Ŏ��ɍs��
					if (boss[fallNum - 1].position.y < -500)
					{
						//���Ƃ��O�̍����Ƒ傫���ɖ߂�
						for (int i = 0; i < fallNum; i++)
						{
							boss[i].position.y = 900;
							boss[i].size = { 3.0f,3.0f,3.0f };
							boss[i].radius = 15.0f;
							fallPlace[i] = false;
						}

						//���̃C�[�W���O����
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

				//�C�[�W���O�Ō��̈ʒu�ɖ߂�
				else if (partBreakScene[1] == Boss_LA_Broken::LA_Undo)
				{
					timeRate = nowCount / maxTime;

					//���̈ʒu�ɃC�[�W���O
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
						}
					}

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�C�[�W���O���I������̂ŏꏊ���Z�b�g���Ď��ɍs��
					if (nowCount > maxTime)
					{
						for (int i = 0; i < bossPartNum; i++)
						{
							fallPlace[i] = false;

							//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
							boss[i].isAttack = false;

							//�{�X�p�[�c�̐F��Ԃɖ߂�
							boss[i].color = baseColor;
						}

						nowCount = 0;
						partBreakScene[1] = Boss_LA_Broken::LA_BreakSet;
						howAttack = BossAttack::NONE;
					}
				}
			}

			//�E���j��
			else if (howAttack == BossAttack::RightLegBreak)
			{
				//������p�[�c�̔ԍ�
				int partNumberStart = bossFacePartNum + bossBodyPartNum;
				int partNumberLast = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum;

				//���ʂ�������Z�b�g
				if (partBreakScene[2] == Boss_RF_Broken::RF_BreakSet)
				{
					int partNum = partNumberLast - partNumberStart;

					//�܂������鏈���̃Z�b�g���������Ė�����΃Z�b�g����
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						if (boss[i].isBreak == 0)
						{
							boss[i].isBreak = rand() % partNum;
							boss[i].beforePosition = boss[i].position;
						}
					}

					//���[�r�[�n
					nowMovie = true;
					camera.afterPosition = camera.Position;//���ɖ߂��悤�Ɉʒu���L�^
					camera.afterTarget = camera.Target;//���ɖ߂��悤�Ɉʒu���L�^
					camera.Position = MovieCameraPos;//���[�r�[���̈ʒu�Ɉړ�
					camera.Target = MovieCameratarget;//���[�r�[���̈ʒu�Ɉړ�
					camera.beforePosition = camera.Position;//�C�[�W���O�悤�Ɍ��̈ʒu���L�^
					camera.beforeTarget = camera.Target;//�C�[�W���O�悤�Ɍ��̈ʒu���L�^

					for (int i = 0; i < bossPartNum; i++)
					{
						fallPlace[i] = true;
					}

					//�Z�b�g�����̂Ŏ��ɍs��
					partBreakScene[2]++;
					nowCount = 0;
				}

				//���ʂ�������X�V
				else if (partBreakScene[2] == Boss_RF_Broken::RF_BreakUpdate)
				{
					const int maxCount = 100;
					//�����鏈��
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

					//�J�E���g��i�߂�
					nowCount++;

					//�����I������玟�ɍs��
					if (nowCount > (partNumberLast - partNumberStart) * 3 + maxCount)
					{
						partBreakScene[2]++;
						nowCount = 0;
					}
				}

				//�C�[�W���O����
				else if (partBreakScene[2] == Boss_RF_Broken::RF_Init)
				{
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].beforePosition = boss[i].position;
							boss[i].afterPosition.y = static_cast<float>((rand() % 2000) + 10);
							boss[i].afterPosition.z = static_cast<float>((rand() % 4000) - 2000);

							//�U���Ɏg�p����{�X�p�[�c�͍U�����ɂ��Ă���
							boss[i].isAttack = true;

							//�{�X�p�[�c�̐F��ύX
							boss[i].color = attackColor;
						}
						fallPlace[i] = true;
					}

					partBreakScene[2]++;
					nowCount = 0;
				}

				//x���W�̃}�C�i�X�����փC�[�W���O
				else if (partBreakScene[2] == Boss_RF_Broken::RF_InFrontPart)
				{
					timeRate = nowCount / maxTime;

					//x���W�̃v���X�����փC�[�W���O
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, -2000, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, boss[i].afterPosition.y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, boss[i].afterPosition.z, timeRate));
						}
					}

					//�J�����̃^�[�Q�b�g�̃C�[�W���O
					float afterCameraPos = -400.0f;
					camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, afterCameraPos, timeRate));

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�C�[�W���O���I������̂ŏꏊ���Z�b�g���Ď��ɍs��
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

				//���E��߂�
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

					//���[�r�[���I���̂ő���\�ɂ���
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

				//x���W�̃}�C�i�X��������v���X�����֔�΂�
				else if (partBreakScene[2] == Boss_RF_Broken::RF_AttackBuckPart)
				{
					nowCount++;
					const float fallSpeed = 20;
					int nowFall = static_cast<int>(nowCount) % bossPartNum;//�������Ă���u���b�N�̐�
					int chack = 0;//�����Ă��鐔
					const int maxMove = 700;
					//����������
					for (int i = 0; i < maxMove; i++)
					{
						if (i < nowFall && nowCount < 1000)
						{
							if (fallPlace[i] == false)
							{
								boss[i].position.y = static_cast<float>((rand() % 300) - 2);
								boss[i].position.z = static_cast<float>((rand() % 4000) - 1000);
								//�Z�b�g�����̂�true�ɂ���
								fallPlace[i] = true;
							}
						}

						if (fallPlace[i] == true)
						{
							//���Ƃ�
							boss[i].position.x = boss[i].position.x + fallSpeed;

							//���l�ȏ�s������z�u���Ȃ���
							if (boss[i].position.x > 950)
							{
								boss[i].position.x = -2000;
								fallPlace[i] = false;
							}
							chack++;
						}
					}

					//�S�Ă��~�܂������ߎ��ɍs��
					if (nowCount > 1000 && chack == 0)
					{
						//���Ƃ��O�̍����Ƒ傫���ɖ߂��ăC�[�W���O�̏���
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

				//���̈ʒu�ɖ߂�
				else if (partBreakScene[2] == Boss_RF_Broken::RF_Undo)
				{
					timeRate = nowCount / maxTime;

					//���̈ʒu�ɃC�[�W���O
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
						}
					}

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�C�[�W���O���I������̂ŏꏊ���Z�b�g���Ď��ɍs��
					if (nowCount > maxTime)
					{
						for (int i = 0; i < bossPartNum; i++)
						{
							//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
							boss[i].isAttack = false;

							//�{�X�p�[�c�̐F��Ԃɖ߂�
							boss[i].color = baseColor;
						}

						partBreakScene[2] = Boss_RF_Broken::RF_BreakSet;
						nowCount = 0;
						howAttack = BossAttack::NONE;
					}
				}
			}

			//�����j��
			else if (howAttack == BossAttack::LeftLegBreak)
			{
				//������p�[�c�̔ԍ�
				int partNumberStart = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum;
				int partNumberLast = bossFacePartNum + bossBodyPartNum + bossRightFootPartNum + bossLeftFootPartNum;

				//���ʂ�������Z�b�g
				if (partBreakScene[3] == Boss_LF_Broken::LF_BreakSet)
				{
					int partNum = partNumberLast - partNumberStart;

					//�܂������鏈���̃Z�b�g���������Ė�����΃Z�b�g����
					for (int i = partNumberStart; i < partNumberLast; i++)
					{
						if (boss[i].isBreak == 0)
						{
							boss[i].isBreak = rand() % partNum;
							boss[i].beforePosition = boss[i].position;
						}
					}

					//���[�r�[�n
					nowMovie = true;
					camera.afterPosition = camera.Position;//���ɖ߂��悤�Ɉʒu���L�^
					camera.afterTarget = camera.Target;//���ɖ߂��悤�Ɉʒu���L�^
					camera.Position = MovieCameraPos;//���[�r�[���̈ʒu�Ɉړ�
					camera.Target = MovieCameratarget;//���[�r�[���̈ʒu�Ɉړ�
					camera.beforePosition = camera.Position;//�C�[�W���O�悤�Ɍ��̈ʒu���L�^
					camera.beforeTarget = camera.Target;//�C�[�W���O�悤�Ɍ��̈ʒu���L�^

					for (int i = 0; i < bossPartNum; i++)
					{
						fallPlace[i] = true;
					}

					//�Z�b�g�����̂Ŏ��ɍs��
					partBreakScene[3]++;
					nowCount = 0;
				}

				//���ʂ�������X�V
				else if (partBreakScene[3] == Boss_LF_Broken::LF_BreakUpdate)
				{
					const int maxCount = 100;
					//�����鏈��
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

					//�J�E���g��i�߂�
					nowCount++;

					//�����I������玟�ɍs��
					if (nowCount > (partNumberLast - partNumberStart) * 3 + maxCount)
					{
						partBreakScene[3]++;
						nowCount = 0;
					}
				}

				//�C�[�W���O����
				else if (partBreakScene[3] == Boss_LF_Broken::LF_Init)
				{
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].beforePosition = boss[i].position;
							fallPlace[i] = true;

							//�U���Ɏg�p����{�X�p�[�c�͍U�����ɂ��Ă���
							boss[i].isAttack = true;

							//�{�X�p�[�c�̐F��ύX
							boss[i].color = attackColor;
						}
					}

					partBreakScene[3]++;
					nowCount = 0;
				}

				//���S�փC�[�W���O
				else if (partBreakScene[3] == Boss_LF_Broken::LF_CenterMove)
				{
					timeRate = nowCount / maxTime;

					//���S�ɏW�܂�悤�ɃC�[�W���O
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, 0, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, 10, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, 0, timeRate));
						}
					}

					//�J�����̃^�[�Q�b�g�̃C�[�W���O
					float afterCameraPos = 100.0f;
					float afterCameraTarget = 90.0f;
					camera.Position.x = static_cast<float>(EASE::easeIn(camera.beforePosition.x, afterCameraPos, timeRate));
					camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, afterCameraPos, timeRate));

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�C�[�W���O���I������̂ŏꏊ���Z�b�g���Ď��ɍs��
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

				//���E��߂�
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

					//���[�r�[���I���̂ő���\�ɂ���
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

				//�~����ɒ��΂�
				else if (partBreakScene[3] == Boss_LF_Broken::LF_AttackCircleMove)
				{
					nowCount++;
					int chack = 0;
					int nowMove = static_cast<int>(nowCount) % 30;//�������Ă����
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

						//�J�E���g��i�߂Č��ȏ�Ȃ�l��߂�
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
						//���l�s���������
						if (boss[i].distance > 1500)
						{
							fallPlace[i] = false;
						}
					}

					//x0y10z0���S�̔��a20�ŉ�
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

					//���ȏ�s�����玟�ɍs��
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

				//���̈ʒu�ɖ߂�
				else if (partBreakScene[3] == Boss_LF_Broken::LF_Undo)
				{
					timeRate = nowCount / maxTime;

					//���̈ʒu�ɃC�[�W���O
					for (int i = 0; i < bossPartNum; i++)
					{
						if (boss[i].isAlive == true)
						{
							boss[i].position.x = static_cast<float>(EASE::easeIn(boss[i].beforePosition.x, bossPartFirstPosition[i].x, timeRate));
							boss[i].position.y = static_cast<float>(EASE::easeIn(boss[i].beforePosition.y, bossPartFirstPosition[i].y, timeRate));
							boss[i].position.z = static_cast<float>(EASE::easeIn(boss[i].beforePosition.z, bossPartFirstPosition[i].z, timeRate));
						}
					}

					//�C�[�W���O�̃J�E���g
					nowCount++;

					//�C�[�W���O���I������̂ŏꏊ���Z�b�g���Ď��ɍs��
					if (nowCount > maxTime)
					{
						for (int i = 0; i < bossPartNum; i++)
						{
							//�U���Ɏg�p�����{�X�p�[�c�̍U�����t���O�����낷
							boss[i].isAttack = false;

							//�{�X�p�[�c�̐F��Ԃɖ߂�
							boss[i].color = baseColor;
						}

						partBreakScene[3] = Boss_LF_Broken::LF_BreakSet;
						nowCount = 0;
						howAttack = BossAttack::NONE;
					}
				}
			}

			//�v���C���[hp��0�ɂȂ����畉��
			if (player.isAlive == false)
			{
				scene = SCENE::Lose;
			}

			//�{�X��hp��0�ɂȂ����珟��
			if (isBossAlive == false)
			{
				scene = SCENE::Win;
			}

			//�r���[�̏���
			if (nowMovie == false)
			{
				camera.Position = player.position;
			}
		}

		//����
		else if (scene == SCENE::Lose)
		{
			//�C�[�W���O�Z�b�g
			if (loseScene == 0)
			{
				//YOUDIED�̕����̓����ɖ߂�
				youDiedColor.w = 0.0f;

				//���[�r�[�n
				nowMovie = true;
				camera.beforeTarget = camera.Target;

				//���̃V�[����
				loseScene = 1;
				nowCount = 0;
			}

			//�ڐ������ɉ�����
			else if (loseScene == 1)
			{
				nowCount++;
				timeRate = nowCount / maxTime;

				//�J�����̃^�[�Q�b�g�̃C�[�W���O
				float afterCameraTarget = 100.0f;
				camera.Target.y = static_cast<float>(EASE::easeIn(camera.beforeTarget.y, afterCameraTarget, timeRate));

				//YOUDIED�̕�����i�X������
				youDiedColor.w = static_cast<float>(EASE::easeIn(0.0f, 1.0f, timeRate));

				//�C�[�W���O���I������̂ŏꏊ���Z�b�g���Ď��ɍs��
				if (nowCount > maxTime)
				{
					loseScene = 2;
					nowCount = 0;
				}
			}

			//���S�ɁuYOU DIED�v��\��������^�C�g���ɖ߂��
			else if (loseScene == 2)
			{
				//�^�C�g���ɖ߂�
				if (input->TriggerPadButtons(Input::PAD_B) || input->TriggerKey(DIK_RETURN) == true)
				{
					//�{�^�����ʉ�
					audio->SoundPlayWava(xAudio2.Get(), Button, false);

					for (int i = 0; i < bossPartNum; i++)
					{
						boss[i].isAlive = false;
					}

					//���g���Ƃ��̂��߂ɏ��������Ă���
					audio->StopSound(BuckSound);
					scene = SCENE::Init;
					nowCount = 0;
					loseScene = 0;
				}
			}
		}

		//����
		else if (scene == SCENE::Win)
		{
			//�v���C���[�ړ�
			PlayerOperationMove(player);
			//�v���C���[���_�ړ�
			PlayerOperationEye(camera, Tgspeed);
			//�r���[�̏���
			if (nowMovie == false)
			{
				camera.Position = player.position;
			}

			//�v���C���[�̍U������
			if (input->TriggerPadButtons(Input::PAD_RB) || input->TriggerKey(DIK_SPACE) == true)
			{
				bulletCount = BulletPreparation(bulletNum, bullet[bulletCount], player, camera.Target, bulletCount);
			}

			//�v���C���[�̍U���X�V
			for (int i = 0; i < bulletNum; i++)
			{
				UpDateBullet(bullet[i]);
			}

			//�C�[�W���O����
			if (winScene == 0)
			{
				//�|�W�V�������Z�b�g
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

				//���[�r�[�n
				winScene++;
				nowCount = 0;
			}

			//�C�[�W���O�ɂ��^�����j
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

				//������x�x���Ȃ����̂Ńp�[�c������
				if (nowCount > max - 3)
				{
					for (int i = 0; i < bossPartNum; i++)
					{
						boss[i].isAlive = false;
					}
				}

				//�C�[�W���O���I������̂Ŏ��ɍs��
				if (nowCount > max)
				{
					winScene++;
					nowCount = 0;
				}
			}

			//YOUWIN�����킶��o��
			else if (winScene == 2)
			{
				nowCount++;
				timeRate = nowCount / maxTime;

				//YOUWIN�̕�����i�X������
				youWinColor.w = static_cast<float>(EASE::easeOut(0.0f, 1.0f, timeRate));

				//�C�[�W���O���I������̂ŏꏊ���Z�b�g���Ď��ɍs��
				if (nowCount > maxTime)
				{
					winScene++;
					nowCount = 0;
				}
			}

			//���S�ɁuYOU WIN�v��\��������^�C�g���ɖ߂��
			else if (winScene == 3)
			{
				//�^�C�g���ɖ߂�
				if (input->TriggerPadButtons(Input::PAD_B) || input->TriggerKey(DIK_RETURN) == true)
				{
					//�{�^�����ʉ�
					audio->SoundPlayWava(xAudio2.Get(), Button, false);

					for (int i = 0; i < bossPartNum; i++)
					{
						boss[i].isAlive = false;
					}

					//���g���Ƃ��̂��߂ɏ��������Ă���
					audio->StopSound(BuckSound);
					scene = SCENE::Init;
					nowCount = 0;
					winScene = 0;
				}
			}
		}

		//���[�r�[�ɓ�������_���[�W��������Z�b�g
		if (nowMovie == true)
		{
			if (isPlayerDamage == true)
			{
				isPlayerDamage = false;
				isPlayerDamageRedShow = false;
				playerDamageTime = 0;
			}
		}

		//�r���[�̏���
		{
			camera.info->SetPositon(camera.Position);
			camera.info->upDate(camera.Target, cameraSpeed);
		}

		//--------�`��-------------//
		//�{�X�͐����Ă���Ƃ������`��
		for (int i = 0; i < bossPartNum; i++)
		{
			if (boss[i].isAlive == true)
			{
				//�ʏ펞
				if (howAttack < BossAttack::RightArmBreak)
				{
					boss[i].info->draw(boss[i].position, boss[i].rota, boss[i].size, boss[i].color, false);
				}

				//�{�X�̕��ʔj��
				if (howAttack >= BossAttack::RightArmBreak && fallPlace[i] == true)
				{
					boss[i].info->draw(boss[i].position, boss[i].rota, boss[i].size, boss[i].color, false);
				}
			}
		}

		//�`���[�g���A���̓I
		for (int i = 0; i < tutorialEnemyNum; i++)
		{
			if (tutorialEnemy[i].isAlive == true)
			{
				tutorialEnemy[i].info->draw(tutorialEnemy[i].position, { 0,0,0 }, { tutorialEnemy[i].size }, tutorialEnemy[i].color, false);
			}
		}

		//-Obj-//
		//player�̍U��
		for (int i = 0; i < bulletNum; i++)
		{
			if (bullet[i].isAlive == true)
			{
				bullet[i].info->draw(bullet[i].position, bullet[i].rota,
					bulletSize, bullet[i].color, false);
			}
		}

		//�{�X�̍U���͈�
		for (int i = 0; i < attackRangeNum; i++)
		{
			if (attackRange[i].isAlive == true)
			{
				attackRange[i].info->draw(attackRange[i].position, attackRange[i].rota,
					attackRange[i].size, attackRange[i].color, false);
			}
		}

		//�n��
		wall->draw({ 0,0,0 }, { 0,0,0 }, 200, { 1.0f,1.0f, 1.0f, 1.0f }, false);


		//�X�v���C�g�`��

		//�^�C�g��
		if (scene == SCENE::Titel)
		{
			//�^�C�g�����S
			sprite[0]->draw(gameSystem->getdXCommon(), SpriteName::Title, { window_width / 2 - 10, 175 }, 0, { 1000.0f, 300.0f });

			//�^�C�g���V�[���̕���(A or B)
			sprite[1]->draw(gameSystem->getdXCommon(), SpriteName::TitleBunki, { window_width / 2, 345 }, 0, { 232.0f, 72.0f });
		}

		//�`���[�g���A��
		else if (scene == SCENE::Tutorial)
		{
			//�C�[�W���O�p�̃|�W�V�����Z�b�g
			if (tutorialScene == TutorialScene::TUT_Init)
			{
			}

			//view���v���C���[���_�ɃC�[�W���O�ŕω�������
			else if (tutorialScene == TutorialScene::TUT_PlayerEyeMove)
			{
			}

			//�`���[�g���A��1�ړ�������
			else if (tutorialScene == TutorialScene::TUT_TryMove)
			{
				//�����������1(�ړ��E�J�����ړ�)
				sprite[2]->draw(gameSystem->getdXCommon(), SpriteName::Pad1, { 1040.0f, 100.0f }, 0, { 428.0f, 160.0f });

				//����U������1(�u�܂��͈ړ����Ă݂܂��傤�v)
				sprite[5]->draw(gameSystem->getdXCommon(), SpriteName::Step1, { window_width / 2, 50.0f }, 0, { 318.0f, 90.0f });

				//�u�`���[�g���A���E�����v�p�Q�[�W�g
				sprite[10]->draw(gameSystem->getdXCommon(), SpriteName::MoveGaugeOut, { window_width / 2 - moveGaugeSizeXMax / 2 - 6, 170.0f }, 0, { 538.0f, 26.0f });

				//�u�`���[�g���A���E�����v�p�Q�[�W
				sprite[11]->draw(gameSystem->getdXCommon(), SpriteName::MoveGaugeIn, { window_width / 2 - moveGaugeSizeXMax / 2, 173.0f }, 0, { moveGaugeSizeX, 20.0f });
			}

			//�`���[�g���A��2�U��������
			else if (tutorialScene == TutorialScene::TUT_TryRedEnemy)
			{
				//�����������2(�ˌ�)
				sprite[3]->draw(gameSystem->getdXCommon(), SpriteName::Pad2, { 1040.0f, 100.0f }, 0, { 428.0f, 160.0f });

				//����U������2(�u���E���ړ������A�U�����Ă݂܂��傤�v)
				sprite[6]->draw(gameSystem->getdXCommon(), SpriteName::Step2, { window_width / 2 + 0.5f, 50.0f }, 0, { 501.0f, 130.0f });

				//�����̓|������
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

				//���e�B�N��
				sprite[30]->draw(gameSystem->getdXCommon(), SpriteName::Reticle, { window_width / 2, window_height / 2 }, 0, { 64.0f, 64.0f });
			}

			//���F�̓G����Ԃɂ��ē|����悤�ɂ���
			else if (tutorialScene == TutorialScene::TUT_TryYellowEnemy)
			{
				//�����������2(�ˌ�)
				sprite[3]->draw(gameSystem->getdXCommon(), SpriteName::Pad2, { 1040.0f, 100.0f }, 0, { 428.0f, 160.0f });

				//����U������3(�u�U���\�ɂȂ����u���b�N���U�����܂��傤�v)
				sprite[7]->draw(gameSystem->getdXCommon(), SpriteName::Step3, { window_width / 2 + 0.5f, 50.0f }, 0, { 591.0f, 90.0f });

				//�����̓|������
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

				//���e�B�N��
				sprite[30]->draw(gameSystem->getdXCommon(), SpriteName::Reticle, { window_width / 2, window_height / 2 }, 0, { 64.0f, 64.0f });
			}

			//�`���[�g���A��3�D���ɍs�������A����������^�C�g����
			else if (tutorialScene == TutorialScene::TUT_Undo)
			{
				//�����������3(�S��)
				sprite[4]->draw(gameSystem->getdXCommon(), SpriteName::Pad3, { 1040.0f, 100.0f }, 0, { 428.0f, 160.0f });

				//����U������4(�u�D���ɓ����Ă݂܂��傤�v)
				sprite[8]->draw(gameSystem->getdXCommon(), SpriteName::Step4, { window_width / 2, 50.0f }, 0, { 322.0f, 90.0f });

				//�^�C�g���֖߂�
				sprite[9]->draw(gameSystem->getdXCommon(), SpriteName::TitleBack, { window_width / 2, 200.0f }, 0, { 164.0f, 27.0f });

				//���e�B�N��
				sprite[30]->draw(gameSystem->getdXCommon(), SpriteName::Reticle, { window_width / 2, window_height / 2 }, 0, { 64.0f, 64.0f });
			}
		}

		else if (scene == SCENE::StartMovie)
		{
			//����
			if (bossMovieScene == BossMovieScene::BM_Init)
			{
			}

			//�����ɏW�܂�
			else if (bossMovieScene == BossMovieScene::BM_CenterMove)
			{
			}

			//�l�^�ɂȂ�
			else if (bossMovieScene == BossMovieScene::BM_CreateHumanModel)
			{
			}

			//���E�ړ�
			else if (bossMovieScene == BossMovieScene::BM_EyeMove)
			{
			}
		}

		else if (scene == SCENE::Battle)
		{
			//���e�B�N��
			if (nowMovie == false)
			{
				sprite[30]->draw(gameSystem->getdXCommon(), SpriteName::Reticle, { window_width / 2, window_height / 2 }, 0, { 64.0f, 64.0f });

				if (isPlayerDamageRedShow == true)
				{
					//�_���[�W���󂯂����ʂ�Ԃ�����
					sprite[31]->draw(gameSystem->getdXCommon(), SpriteName::DamageRed, { 0.0f, 0.0f }, 0, { 1280.0f, 760.0f }, { 1, 0, 0, 0.5 });
				}
			}

			//�{�X��HP�p�Q�[�W�g
			sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

			//�{�X��HP�p�Q�[�W
			sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


			//���񂾃n�[�g
			sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
			sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
			sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
			sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
			sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

			if (playerHP >= 5)
			{
				//HP�n�[�g
				sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
			}

			if (playerHP >= 4)
			{
				//HP�n�[�g
				sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
			}

			if (playerHP >= 3)
			{
				//HP�n�[�g
				sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
			}

			if (playerHP >= 2)
			{
				//HP�n�[�g
				sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
			}

			if (playerHP >= 1)
			{
				//HP�n�[�g
				sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
			}
		}

		//����
		else if (scene == SCENE::Lose)
		{
			//�C�[�W���O�Z�b�g
			if (loseScene == 0)
			{
				//�{�X��HP�p�Q�[�W�g
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//�{�X��HP�p�Q�[�W
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//���񂾃n�[�g
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HP�n�[�g
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HP�n�[�g
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HP�n�[�g
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HP�n�[�g
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HP�n�[�g
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}
			}

			//�ڐ������ɉ�����
			else if (loseScene == 1)
			{
				//�{�X��HP�p�Q�[�W�g
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//�{�X��HP�p�Q�[�W
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//���񂾃n�[�g
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HP�n�[�g
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HP�n�[�g
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HP�n�[�g
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HP�n�[�g
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HP�n�[�g
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				//�w�i���Â�
				sprite[33]->draw(gameSystem->getdXCommon(), SpriteName::BrackBackGround, { 0, 0 }, 0, { 1280.0f, 760.0f }, youDiedColor);

				//YOUDIED
				sprite[32]->draw(gameSystem->getdXCommon(), SpriteName::YOUDIED, { window_width / 2, 330.0f }, 0, { 508.0f, 76.0f }, youDiedColor);
			}

			//lose�\��
			else if (loseScene == 2)
			{
				//�{�X��HP�p�Q�[�W�g
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//�{�X��HP�p�Q�[�W
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//���񂾃n�[�g
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HP�n�[�g
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HP�n�[�g
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HP�n�[�g
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HP�n�[�g
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HP�n�[�g
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				//�w�i���Â�
				sprite[33]->draw(gameSystem->getdXCommon(), SpriteName::BrackBackGround, { 0, 0 }, 0, { 1280.0f, 760.0f }, youDiedColor);

				//YOUDIED
				sprite[32]->draw(gameSystem->getdXCommon(), SpriteName::YOUDIED, { window_width / 2, 330.0f }, 0, { 508.0f, 76.0f }, youDiedColor);

				//�^�C�g���֖߂�
				sprite[9]->draw(gameSystem->getdXCommon(), SpriteName::TitleBack, { window_width / 2, 440.0f }, 0, { 164.0f, 27.0f });
			}
		}

		//����
		else if (scene == SCENE::Win)
		{
			if (winScene == 0)
			{
				//�{�X��HP�p�Q�[�W�g
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//�{�X��HP�p�Q�[�W
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//���񂾃n�[�g
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HP�n�[�g
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HP�n�[�g
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HP�n�[�g
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HP�n�[�g
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HP�n�[�g
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}
			}

			else if (winScene == 1)
			{
				//�{�X��HP�p�Q�[�W�g
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//�{�X��HP�p�Q�[�W
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//���񂾃n�[�g
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HP�n�[�g
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HP�n�[�g
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HP�n�[�g
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HP�n�[�g
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HP�n�[�g
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}
			}

			else if (winScene == 2)
			{
				//�{�X��HP�p�Q�[�W�g
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//�{�X��HP�p�Q�[�W
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//���񂾃n�[�g
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HP�n�[�g
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HP�n�[�g
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HP�n�[�g
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HP�n�[�g
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HP�n�[�g
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				//YOUWIN
				sprite[34]->draw(gameSystem->getdXCommon(), SpriteName::YOUWIN, { window_width / 2, 330.0f }, 0, { 474.0f, 76.0f }, youWinColor);
			}

			else if (winScene == 3)
			{
				//�{�X��HP�p�Q�[�W�g
				sprite[18]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeOut, { window_width / 2 - bossHPGaugeSizeXMax / 2 - 3, 18.0f }, 0, { 700.0f, 70.0f });

				//�{�X��HP�p�Q�[�W
				sprite[19]->draw(gameSystem->getdXCommon(), SpriteName::BossGaugeIn, { window_width / 2 - bossHPGaugeSizeXMax / 2, 65.0f }, 0, { bossHPGaugeSizeX, 20.0f });


				//���񂾃n�[�g
				sprite[25]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[26]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[27]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[28]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				sprite[29]->draw(gameSystem->getdXCommon(), SpriteName::HeartEnpty, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });

				if (playerHP >= 5)
				{
					//HP�n�[�g
					sprite[20]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 170.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 4)
				{
					//HP�n�[�g
					sprite[21]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 140.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 3)
				{
					//HP�n�[�g
					sprite[22]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 110.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 2)
				{
					//HP�n�[�g
					sprite[23]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 80.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				if (playerHP >= 1)
				{
					//HP�n�[�g
					sprite[24]->draw(gameSystem->getdXCommon(), SpriteName::Heart, { 50.0f, 50.0f }, 0, { 23.0f, 20.0f });
				}

				//YOUWIN
				sprite[34]->draw(gameSystem->getdXCommon(), SpriteName::YOUWIN, { window_width / 2, 330.0f }, 0, { 474.0f, 76.0f }, youWinColor);

				//�^�C�g���֖߂�
				sprite[9]->draw(gameSystem->getdXCommon(), SpriteName::TitleBack, { window_width / 2, 440.0f }, 0, { 164.0f, 27.0f });
			}
		}

		//�f�o�b�O�e�L�X�g
		text.DrawAll(gameSystem->getdXCommon());

		//-------------------------------------//
		gameSystem->draw();

		gameSystem->frameRateKeep();
	}

	//���
	//xAudio2���
	xAudio2.Reset();

	//�����f�[�^���
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

	//�o�^����
	winApp->winRelease();

	return 0;
}