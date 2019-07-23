//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Shot00.h"
#include  "Task_Map2D.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Player.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-25,-25, 50, 50);
		this->angle_LR = Right;
		this->controller = ge->in1;
		this->hp = 10;

		this->motion = Stand;		//キャラ初期状態
		this->maxSpeed = 5.0f;		//最大移動速度（横）
		this->addSpeed = 1.0f;		//歩行加速度（地面の影響である程度打ち消される
		this->decSpeed = 0.5f;		//接地状態の時の速度減衰量（摩擦
		this->maxFallSpeed = 10.0f;	//最大落下速度
		this->jumpPow = -10.0f;		//ジャンプ力（初速）
		this->gravity = ML::Gravity(32) * 5; //重力加速度＆時間速度による加算量

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		this->moveCnt++;
		this->animCnt++;
		if (this->unHitTime > 0) { this->unHitTime--; }
		//思考・状況判断
		this->Think();
		//現モーションに対応した制御
		this->Move();
		//めり込まない移動
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		//当たり判定
		{
			ML::Box2D  me = this->hitBase.OffsetCopy(this->pos);
			auto  targets = ge->GetTask_Group_G<BChara>("アイテム");
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
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{

		if (this->unHitTime > 0) {
			if ((this->unHitTime / 4) % 2 == 0) {
				return;//8フレーム中4フレーム画像を表示しない
			}
		}

		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//スクロール対応
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src);
	}
	//-----------------------------------------------------------------------------
	//思考＆状況判断　モーション決定
	void  Object::Think()
	{
		auto  inp = this->controller->GetState();
		BChara::Motion  nm = this->motion;	//とりあえず今の状態を指定

		//思考（入力）や状況に応じてモーションを変更する事を目的としている。
		//モーションの変更以外の処理は行わない
		switch (nm) {
		case  Stand:	//立っている
			if (inp.LStick.L.on) { nm = Walk; }
			if (inp.LStick.R.on) { nm = Walk; }
			if (inp.LStick.U.on) { nm = Jump; }
			if (inp.LStick.D.on) { nm = Sit; }
			if (inp.B2.on) { nm = Jump; }
			if (inp.B4.down) { nm = Attack; }
			if (this->CheckFoot() == false) { nm = Fall; }//足元 障害　無し
			break;
		case  Walk:		//歩いている
			if (inp.LStick.L.off && inp.LStick.R.off) { nm = Stand; }
			if (inp.LStick.D.on) { nm = Sit; }
			if (inp.LStick.U.on) { nm = Jump; }
			if (inp.B4.down) { nm = Attack; }
			if (this->CheckFoot() == false) { nm = Fall; }//足元 障害　無し
			break;
		case  Jump:		//上昇中
			if (this->moveVec.y >= 0) { nm = Fall; }

			break;
		case  Fall:		//落下中
			if (this->CheckFoot() == true) { nm = Landing; }//木下　障害あり
			break;
		case  Sit:	//座る
			if (inp.LStick.D.off) { nm = Stand; }
			if (inp.B4.down) { nm = SitAttack; }
			if (this->CheckFoot() == false) { nm = Fall; }//足元 障害　無し
			break;
		case  Landing:	//着地
			if (this->moveCnt >= 3) { nm = Stand; }
			if (this->CheckFoot() == false) { nm = Fall; }//足元 障害　無し
			break;
		case  SitAttack:  //座るながら攻撃
			if (inp.B4.off) { nm = Sit; }
			break;
		
		}
		//モーション更新
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//モーションに対応した処理
	//(モーションは変更しない）
	void  Object::Move()
	{
		auto  inp = this->controller->GetState();

		//重力加速
		switch (this->motion) {
		default:
			//上昇中もしくは足元に地面が無い
			if (this->moveVec.y < 0 ||
				this->CheckFoot() == false) {
				//this->moveVec.y = 1.0f;//仮処理
				this->moveVec.y = min(this->moveVec.y + this->gravity,
														this->maxFallSpeed);
			}
			//地面に接触している
			else {
				this->moveVec.y = 0.0f;
			}
			break;
			//重力加速を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
		case Unnon:	break;
		}

		//移動速度減衰
		switch (this->motion) {
		default:
			if (this->moveVec.x < 0) {
				this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
			}
			else {
				this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
			}
			break;
			//移動速度減衰を無効化する必要があるモーションは下にcaseを書く
		case Unnon:	break;
		}
		//-----------------------------------------------------------------
		//モーション毎に固有の処理
		switch (this->motion) {
		case  Stand:	//立っている
			break;
		case  Walk:		//歩いている
			if (inp.LStick.L.on) {
				this->angle_LR = Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.R.on) {
				this->angle_LR = Right;
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Fall:		//落下中
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
		case  Jump:		//上昇中c
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
		case  Attack:	//攻撃中
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
	//アニメーション制御
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(10, 12, 63, 60), defColor },	//停止1
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(90, 12, 63, 60), defColor },	//停止2
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(170, 12, 63, 60), defColor },	//停止3
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(250, 12, 63, 60), defColor },	//停止4
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(330, 12, 63, 60), defColor },	//停止5
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(410, 12, 63, 60), defColor },	//停止6
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(490, 12, 63, 60), defColor },	//停止7
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(570, 12, 63, 60), defColor },	//停止8
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(10, 96, 60, 60), defColor },	//歩行１
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(90, 96, 60, 60), defColor },	//歩行２
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(170, 96, 60, 60), defColor },	//歩行３
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(250, 96, 60, 60), defColor },	//歩行4
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(330, 96, 60, 60), defColor },	//歩行5										
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(410, 96, 60, 60), defColor },	//歩行6
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(10, 176, 65, 60), defColor },	//攻撃
			{ ML::Box2D(-29, -25, 58, 50), ML::Box2D(170, 250, 70, 60), defColor },	//ジャンプ
			{ ML::Box2D(-29, -25, 58, 50), ML::Box2D(90, 250, 70, 60), defColor },	//Fall
			{ ML::Box2D(-25, -18, 50, 43), ML::Box2D(96, 335, 60, 50), defColor },	//座る
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(0, 80, 60, 60), defColor },	//飛び立つ直前
			{ ML::Box2D(-25, -25, 50, 50), ML::Box2D(144, 80,60, 60), defColor },	//着地
			{ ML::Box2D(-22, -18, 62, 43), ML::Box2D(8, 335, 74, 50), defColor },	//座るながら攻撃
		};
		BChara::DrawInfo  rtv;
		int  walk, stand,jump;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
		//	ジャンプ------------------------------------------------------------------------
		case  Jump:		
			rtv = imageTable[15];
			break;
		//　	座る----------------------------------------------------------------------------
		case  Sit:	rtv = imageTable[17];	break;
		//　着地時--------------------------------------------------------------------------
		case Landing:	rtv = imageTable[7];	break;
		//	停止---------------------------------------------------------------------------
		case  Stand:	
			stand = this->animCnt / 8;
			stand %= 8;
			rtv = imageTable[stand];
			break;
		//　歩くながら攻撃--------------------------------------------------------------------------
		case Attack:
			rtv = imageTable[14];
			break;
		
		//	歩行----------------------------------------------------------------------------
		case  Walk:
			walk = this->animCnt / 8;
			walk %= 6;
			rtv = imageTable[walk + 8];
			break;
		//	落下----------------------------------------------------------------------------
		case  Fall:		rtv = imageTable[16];	break;
		//座るながら攻撃---------------------------------------------------------------------
		case  SitAttack:		rtv = imageTable[20];	break;
		}
		//	向きに応じて画像を左右反転する
		if (Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x ;
			rtv.draw.w = -rtv.draw.w;
		}
		return rtv;
	}
	//-------------------------------------------------------------------
	//接触時の応答処理（必ず受け身の処理として実装する）
	void  Object::Received(BChara*  from_, AttackInfo  at_)
	{
		if (this->unHitTime > 0) {
			return;//無敵時間中はダメージを受けない
		}
		this->unHitTime = 400;
		this->hp -= at_.power;
		if (this->hp <= 0) {
			this->Kill();
		}
		//吹き飛ばされる
		if (this->pos.x > from_->pos.x) {
			this->moveVec = ML::Vec2(+4, -9);
		}
		else {
			this->moveVec = ML::Vec2(-4, -9);
		}
		
		//from_は攻撃してきた相手、カウンター等で逆にダメージを与えたいときに使う
	}
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
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
	//リソースクラスの生成
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