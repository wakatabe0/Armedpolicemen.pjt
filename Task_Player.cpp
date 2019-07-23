//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Shot00.h"
#include  "Task_Map2D.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Player.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-25,-25, 50, 50);
		this->angle_LR = Right;
		this->controller = ge->in1;
		this->hp = 10;

		this->motion = Stand;		//�L�����������
		this->maxSpeed = 5.0f;		//�ő�ړ����x�i���j
		this->addSpeed = 1.0f;		//���s�����x�i�n�ʂ̉e���ł�����x�ł��������
		this->decSpeed = 0.5f;		//�ڒn��Ԃ̎��̑��x�����ʁi���C
		this->maxFallSpeed = 10.0f;	//�ő嗎�����x
		this->jumpPow = -10.0f;		//�W�����v�́i�����j
		this->gravity = ML::Gravity(32) * 5; //�d�͉����x�����ԑ��x�ɂ����Z��

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		this->moveCnt++;
		this->animCnt++;
		if (this->unHitTime > 0) { this->unHitTime--; }
		//�v�l�E�󋵔��f
		this->Think();
		//�����[�V�����ɑΉ���������
		this->Move();
		//�߂荞�܂Ȃ��ړ�
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		//�����蔻��
		{
			ML::Box2D  me = this->hitBase.OffsetCopy(this->pos);
			auto  targets = ge->GetTask_Group_G<BChara>("�A�C�e��");
			for (auto it = targets->begin();
				it != targets->end();
				++it) {
				if ((*it)->CheckHit(me)) {
					BChara::AttackInfo	at = { 0,0,0 };
					(*it)->Received(this, at);
					break;
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{

		if (this->unHitTime > 0) {
			if ((this->unHitTime / 4) % 2 == 0) {
				return;//8�t���[����4�t���[���摜��\�����Ȃ�
			}
		}

		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//�X�N���[���Ή�
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src);
	}
	//-----------------------------------------------------------------------------
	//�v�l���󋵔��f�@���[�V��������
	void  Object::Think()
	{
		auto  inp = this->controller->GetState();
		BChara::Motion  nm = this->motion;	//�Ƃ肠�������̏�Ԃ��w��

		//�v�l�i���́j��󋵂ɉ����ă��[�V������ύX���鎖��ړI�Ƃ��Ă���B
		//���[�V�����̕ύX�ȊO�̏����͍s��Ȃ�
		switch (nm) {
		case  Stand:	//�����Ă���
			if (inp.LStick.L.on) { nm = Walk; }
			if (inp.LStick.R.on) { nm = Walk; }
			if (inp.LStick.U.on) { nm = Jump; }
			if (inp.LStick.D.on) { nm = Sit; }
			if (inp.B2.on) { nm = Jump; }
			if (inp.B4.down) { nm = Attack; }
			if (this->CheckFoot() == false) { nm = Fall; }//���� ��Q�@����
			break;
		case  Walk:		//�����Ă���
			if (inp.LStick.L.off && inp.LStick.R.off) { nm = Stand; }
			if (inp.LStick.D.on) { nm = Sit; }
			if (inp.LStick.U.on) { nm = Jump; }
			if (inp.B4.down) { nm = Attack; }
			if (this->CheckFoot() == false) { nm = Fall; }//���� ��Q�@����
			break;
		case  Jump:		//�㏸��
			if (this->moveVec.y >= 0) { nm = Fall; }

			break;
		case  Fall:		//������
			if (this->CheckFoot() == true) { nm = Landing; }//�؉��@��Q����
			break;
		case  Sit:	//����
			if (inp.LStick.D.off) { nm = Stand; }
			if (inp.B4.down) { nm = SitAttack; }
			if (this->CheckFoot() == false) { nm = Fall; }//���� ��Q�@����
			break;
		case  Landing:	//���n
			if (this->moveCnt >= 3) { nm = Stand; }
			if (this->CheckFoot() == false) { nm = Fall; }//���� ��Q�@����
			break;
		case  SitAttack:  //����Ȃ���U��
			if (inp.B4.off) { nm = Sit; }
			break;
		
		}
		//���[�V�����X�V
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//���[�V�����ɑΉ���������
	//(���[�V�����͕ύX���Ȃ��j
	void  Object::Move()
	{
		auto  inp = this->controller->GetState();

		//�d�͉���
		switch (this->motion) {
		default:
			//�㏸���������͑����ɒn�ʂ�����
			if (this->moveVec.y < 0 ||
				this->CheckFoot() == false) {
				//this->moveVec.y = 1.0f;//������
				this->moveVec.y = min(this->moveVec.y + this->gravity,
														this->maxFallSpeed);
			}
			//�n�ʂɐڐG���Ă���
			else {
				this->moveVec.y = 0.0f;
			}
			break;
			//�d�͉����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
		case Unnon:	break;
		}

		//�ړ����x����
		switch (this->motion) {
		default:
			if (this->moveVec.x < 0) {
				this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
			}
			else {
				this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
			}
			break;
			//�ړ����x�����𖳌�������K�v�����郂�[�V�����͉���case������
		case Unnon:	break;
		}
		//-----------------------------------------------------------------
		//���[�V�������ɌŗL�̏���
		switch (this->motion) {
		case  Stand:	//�����Ă���
			break;
		case  Walk:		//�����Ă���
			if (inp.LStick.L.on) {
				this->angle_LR = Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.R.on) {
				this->angle_LR = Right;
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Fall:		//������
			if (inp.LStick.L.on) {
				this->angle_LR = Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.R.on) {
				this->angle_LR = Right;
				this->moveVec.x = this->maxSpeed;
			}
			if (inp.B4.down) {
				auto shot = Shot00::Object::Create(true);
				if (this->angle_LR == Left) {
					shot->pos.x = this->pos.x - 25;
					shot->moveVec = ML::Vec2(-8, 0);
				}
				else {
					shot->pos.x = this->pos.x + 25;
					shot->moveVec = ML::Vec2(+8, -0);
				}
				shot->pos.y = this->pos.y - 8;
			}
			break;
		case  Jump:		//�㏸��c
			if (inp.LStick.L.on) {
				this->angle_LR = Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.R.on) {
				this->angle_LR = Right;
				this->moveVec.x = this->maxSpeed;
			}
			if (CheckHead()) {
				this->moveVec.y = 0;
			}
			if (moveCnt == 0) {
				this->moveVec.y = this->jumpPow;
			}
			if(inp.B4.down){
				auto shot = Shot00::Object::Create(true);
				if (this->angle_LR == Left) {
					shot->pos.x = this->pos.x - 25;
					shot->moveVec = ML::Vec2(-8, 0);
				}
				else {
					shot->pos.x = this->pos.x + 25;
					shot->moveVec = ML::Vec2(+8, -0);
				}
				shot->pos.y = this->pos.y - 8;
			}
			break;
		case  Attack:	//�U����
			if (inp.LStick.L.on) {
				this->angle_LR = Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.R.on) {
				this->angle_LR = Right;
				this->moveVec.x = this->maxSpeed;
			}
			if (this->moveCnt == 4) {
				auto shot = Shot00::Object::Create(true);

				if (this->angle_LR == Left) {
					shot->pos.x = this->pos.x - 25;
					shot->moveVec = ML::Vec2(-8, 0);
				}
				else {
					shot->pos.x = this->pos.x + 25;
					shot->moveVec = ML::Vec2(+8, -0);
				}
				shot->pos.y = this->pos.y - 12;

			}
			else if (this->moveCnt >= 8) {
				this->motion = Stand;
			}

			break;
		case Sit:
			if (inp.LStick.L.on) {
				this->angle_LR = Left;		
			}
			if (inp.LStick.R.on) {
				this->angle_LR = Right;
			}
			break;
		case SitAttack:
			if (this->moveCnt ==3) {
				auto shot = Shot00::Object::Create(true);

				if (this->angle_LR == Left) {
					shot->pos.x = this->pos.x - 30;
					shot->moveVec = ML::Vec2(-8, 0);
				}
				else {
					shot->pos.x = this->pos.x + 30;
					shot->moveVec = ML::Vec2(+8, -0);
				}
				shot->pos.y = this->pos.y ;

			}
			else if (this->moveCnt >= 6) {
				this->motion = Sit;
			}
			break;
		}
	}
	//-----------------------------------------------------------------------------
	//�A�j���[�V��������
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(10, 12, 63, 60), defColor },	//��~1
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(90, 12, 63, 60), defColor },	//��~2
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(170, 12, 63, 60), defColor },	//��~3
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(250, 12, 63, 60), defColor },	//��~4
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(330, 12, 63, 60), defColor },	//��~5
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(410, 12, 63, 60), defColor },	//��~6
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(490, 12, 63, 60), defColor },	//��~7
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(570, 12, 63, 60), defColor },	//��~8
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(10, 96, 60, 60), defColor },	//���s�P
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(90, 96, 60, 60), defColor },	//���s�Q
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(170, 96, 60, 60), defColor },	//���s�R
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(250, 96, 60, 60), defColor },	//���s4
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(330, 96, 60, 60), defColor },	//���s5										
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(410, 96, 60, 60), defColor },	//���s6
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(10, 176, 65, 60), defColor },	//�U��
			{ ML::Box2D(-29, -25, 58, 50), ML::Box2D(170, 250, 70, 60), defColor },	//�W�����v
			{ ML::Box2D(-29, -25, 58, 50), ML::Box2D(90, 250, 70, 60), defColor },	//Fall
			{ ML::Box2D(-25, -18, 50, 43), ML::Box2D(96, 335, 60, 50), defColor },	//����
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(0, 80, 60, 60), defColor },	//��ї����O
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(144, 80,60, 60), defColor },	//���n
			{ ML::Box2D(-22, -18, 62, 43), ML::Box2D(8, 335, 74, 50), defColor },	//����Ȃ���U��
		};
		BChara::DrawInfo  rtv;
		int  walk, stand,jump;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
		//	�W�����v------------------------------------------------------------------------
		case  Jump:		
			rtv = imageTable[15];
			break;
		//�@	����----------------------------------------------------------------------------
		case  Sit:	rtv = imageTable[17];	break;
		//�@���n��--------------------------------------------------------------------------
		case Landing:	rtv = imageTable[7];	break;
		//	��~---------------------------------------------------------------------------
		case  Stand:	
			stand = this->animCnt / 8;
			stand %= 8;
			rtv = imageTable[stand];
			break;
		//�@�����Ȃ���U��--------------------------------------------------------------------------
		case Attack:
			rtv = imageTable[14];
			break;
		
		//	���s----------------------------------------------------------------------------
		case  Walk:
			walk = this->animCnt / 8;
			walk %= 6;
			rtv = imageTable[walk + 8];
			break;
		//	����----------------------------------------------------------------------------
		case  Fall:		rtv = imageTable[16];	break;
		//����Ȃ���U��---------------------------------------------------------------------
		case  SitAttack:		rtv = imageTable[20];	break;
		}
		//	�����ɉ����ĉ摜�����E���]����
		if (Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x ;
			rtv.draw.w = -rtv.draw.w;
		}
		return rtv;
	}
	//-------------------------------------------------------------------
	//�ڐG���̉��������i�K���󂯐g�̏����Ƃ��Ď�������j
	void  Object::Received(BChara*  from_, AttackInfo  at_)
	{
		if (this->unHitTime > 0) {
			return;//���G���Ԓ��̓_���[�W���󂯂Ȃ�
		}
		this->unHitTime = 400;
		this->hp -= at_.power;
		if (this->hp <= 0) {
			this->Kill();
		}
		//������΂����
		if (this->pos.x > from_->pos.x) {
			this->moveVec = ML::Vec2(+4, -9);
		}
		else {
			this->moveVec = ML::Vec2(-4, -9);
		}
		
		//from_�͍U�����Ă�������A�J�E���^�[���ŋt�Ƀ_���[�W��^�������Ƃ��Ɏg��
	}
	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//���\�[�X�N���X�̐���
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}