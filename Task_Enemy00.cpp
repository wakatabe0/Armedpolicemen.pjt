//-------------------------------------------------------------------
//敵 00（スライム）
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Enemy00.h"

namespace  Enemy00
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Enemy00.png");
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
		this->render2D_Priority[1] = 0.6f;
		this->hitBase = ML::Box2D(-28, -22, 56, 45);
		this->angle_LR = Left;
		this->motion = Stand;
		this->maxSpeed = 2.0f;		//最大移動速度（横）
		this->addSpeed = 0.7f;		//歩行加速度（地面の影響である程度打ち消される）
		this->decSpeed = 0.5f;		//接地状態の時の速度減衰量（摩擦）
		this->maxFallSpeed = 10.0f; //最大落下速度
		this->jumpPow = -6.0f;		//ジャンプ力（初速）
		this->gravity = ML::Gravity(32);	//重力加速度＆時間速度による加算量
		this->hp = 20;

		
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
			auto  targets = ge->GetTask_Group_G<BChara>("プレイヤ");
			for (auto it = targets->begin();
				it != targets->end();
				++it) {
				//相手に接触の有無を確認させる
				if ((*it)->CheckHit(me)) {
					BChara::AttackInfo  at = { 1,0,0 };
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
		BChara::Motion  nm = this->motion;	//とりあえず今の状態を指定

		//思考（入力）や状況に応じてモーションを変更する事を目的としている。
		//モーションの変更以外の処理は行わない
		switch (nm) {
		case  Stand:	//立っている
			nm = Walk;
			if (CheckFoot() == false) { nm = Fall; }//足元 障害 なし
			break;
		case  Walk:		//歩いている
			if (this->CheckFront_LR() == true) { nm = Turn; }
			if (CheckFoot() == false) { nm = Fall; }//足元 障害 なし
			break;
		case  Turn:
			if (this->moveCnt >= 5) { nm = Stand; }
			break;
		case  Jump:		//上昇中
			if (this->moveVec.y >= 0) { nm = Fall; }
			break;
		case  Fall:		//落下中
			if (this->CheckFoot() == true) { nm = Stand; }
			break;
		case  Attack:	//攻撃中
			break;
		case  Sit:	//座る
			break;
		case  Landing:	//着地
			if (this->CheckFoot() == true) { nm = Stand; }
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
		//重力加速
		switch (this->motion) {
		default:
			//上昇中もしくは足元に地面が無い
			if (this->moveVec.y < 0 ||
				this->CheckFoot() == false) {
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
			//移動速度減衰を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
		case Unnon:	break;
		}
		//-----------------------------------------------------------------
		//モーション毎に固有の処理
		switch (this->motion) {
		case  Stand:	//立っている
			break;
		case  Walk:		//歩いている
			if (this->angle_LR == Left) {
				this->moveVec.x =
					max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (this->angle_LR == Right) {
				this->moveVec.x =
					min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case  Turn:
			if (this->moveCnt == 3) {
				if (this->angle_LR == Left) { this->angle_LR = Right; }
				else { this->angle_LR = Left; }
			}
			break;
		case  Fall:		//落下中
			if (this->angle_LR == Left) {
				this->moveVec.x =
					max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (this->angle_LR == Right) {
				this->moveVec.x =
					min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case  Jump:		//上昇中
			break;
		case  Attack:	//攻撃中
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
			{ ML::Box2D(-32, -24, 64, 48), ML::Box2D(0, 0, 64, 48), defColor },	//停止
			{ ML::Box2D(-32, -32, 64, 64), ML::Box2D(128, 48, 64, 64), defColor },	//落下
		};
		BChara::DrawInfo  rtv;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	ジャンプ------------------------------------------------------------------------
		case  Jump:		rtv = imageTable[0];	break;
			//	停止----------------------------------------------------------------------------
		case  Stand:	rtv = imageTable[0];	break;
			//	歩行----------------------------------------------------------------------------
		case  Walk:		rtv = imageTable[0];	break;
			//	落下----------------------------------------------------------------------------
		case  Fall:		rtv = imageTable[1];	break;
		}
		//	向きに応じて画像を左右反転する
		if (Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}
		return rtv;
	}
	//-------------------------------------------------------------------
	//接触時の応答処理（必ず受け身の処理として実装する）
	void  Object::Received(BChara*  from_, AttackInfo  at_)
	{
		this->hp -= at_.power;
		if (this->hp <= 0) {
			this->Kill();
		}
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