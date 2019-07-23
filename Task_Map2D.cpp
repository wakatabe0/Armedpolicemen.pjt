//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Map2D.h"

namespace  Map2D
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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
		//マップのゼロクリア
		for (int y = 0; y < 100; ++y) {
			for (int x = 0; x < 100; ++x) {
				this->arr[y][x] = 0;
			}
		}
		this->sizeX = 0;
		this->sizeY = 0;
		this->hitBase = ML::Box2D(0, 0, 0, 0);

		//マップチップ情報の初期化
		for (int c = 0; c < 15; ++c) {
			int  x = (c % 5);
			int  y = (c / 5);
			this->chip[c] = ML::Box2D(x * 256, y * 256, 256, 256);
		}
		
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		this->img.reset();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//カメラが完全にマップ外を指しているか調べる
		if (false == this->hitBase.Hit(ge->camera2D)) {
			return; //完全に外に出ていたらその時点で描画処理を取りやめる
		}

		//カメラとマップが重なっている範囲だけの矩形を作る
		RECT  c = {
			ge->camera2D.x,
			ge->camera2D.y,
			ge->camera2D.x + ge->camera2D.w,
			ge->camera2D.y + ge->camera2D.h };
		RECT  m = {
			this->hitBase.x,
			this->hitBase.y,
			this->hitBase.x + this->hitBase.w,
			this->hitBase.y + this->hitBase.h };
		//２つの矩形の重なっている範囲だけの矩形を求める
		RECT  isr;
		isr.left = max(c.left, m.left);
		isr.top = max(c.top, m.top);
		isr.right = min(c.right, m.right);
		isr.bottom = min(c.bottom, m.bottom);

		//ループ範囲を決定
		int sx, sy, ex, ey;
		sx = isr.left / 256;
		sy = isr.top / 256;
		ex = (isr.right - 1) / 32;
		ey = (isr.bottom - 1) / 32;

		//画面内の範囲だけ描画
		for (int y = sy; y <= ey; ++y) {
			for (int x = sx; x <= ex; ++x) {
				ML::Box2D  draw(0, 0, 32, 32);
				draw.Offset(x * 32, y * 32);	//表示位置を調整
				
				//スクロール対応
				draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
				this->img->Draw(draw, this->chip[this->arr[y][x]]);
			}
		}
	}
	//-------------------------------------------------------------------
	//マップ読み込み
	bool  Object::Load(const  string&  fpath_) 
	{
		//ファイルを開く（読み込み）
		ifstream   fin(fpath_);
		if (!fin) { return  false; }//読み込み失敗


		//チップファイル名の読み込みと、画像のロード
		string   chipFileName, chipFilePath;
		fin >> chipFileName;
		chipFilePath = "./data/image/" + chipFileName;
		this->img = DG::Image::Create(chipFilePath);


		//マップ配列サイズの読み込み
		fin >> this->sizeX >> this->sizeY;
		this->hitBase = ML::Box2D(0, 0, this->sizeX * 32, this->sizeY * 32);

		//マップ配列データの読み込み
		for (int y = 0; y < this->sizeY; ++y) {
			for (int x = 0; x < this->sizeX; ++x) {
				fin >> this->arr[y][x];
			}
		}
		fin.close();

		return true;
	}
	//-------------------------------------------------------------------
	//あたり判定
	bool  Object::CheckHit(const  ML::Box2D&  hit_)
	{
		RECT  r = { hit_.x, hit_.y, hit_.x + hit_.w, hit_.y + hit_.h };
		//矩形がマップ外に出ていたら障害物に当たったことにする
		RECT  m = {
			this->hitBase.x,
			this->hitBase.y,
			this->hitBase.x + this->hitBase.w,
			this->hitBase.y + this->hitBase.h };
		if (r.left   < m.left) { r.left = m.left; }
		if (r.top    < m.top) { r.top = m.top; }
		if (r.right  > m.right) { r.right = m.right; }
		if (r.bottom > m.bottom) { r.bottom = m.bottom; }

		//ループ範囲調整
		int sx, sy, ex, ey;
		sx = r.left / 32;
		sy = r.top / 32;
		ex = (r.right - 1) / 32;
		ey = (r.bottom - 1) / 32;

		//範囲内の障害物を探す
		for (int y = sy; y <= ey; ++y) {
			for (int x = sx; x <= ex; ++x) {
				if ((1 <= this->arr[y][x] && 11 >= this->arr[y][x]) || 14 == this->arr[y][x]) {
					return true;
				}
			}
		}
		return false;
	}
	//-------------------------------------------------------------------
	//マップ外を見せないようにカメラを位置調整する
	void  Object::adjustCameraPos()
	{
		//カメラとマップの範囲を用意
		RECT  c = {
			ge->camera2D.x,
			ge->camera2D.y,
			ge->camera2D.x + ge->camera2D.w,
			ge->camera2D.y + ge->camera2D.h };
		RECT  m = {
			this->hitBase.x,
			this->hitBase.y,
			this->hitBase.x + this->hitBase.w,
			this->hitBase.y + this->hitBase.h };

		//カメラの位置を調整
		if (c.right  > m.right) { ge->camera2D.x = m.right - ge->camera2D.w; }
		if (c.bottom > m.bottom) { ge->camera2D.y = m.bottom - ge->camera2D.h; }
		if (c.left < m.left) { ge->camera2D.x = m.left; }
		if (c.top  < m.top) { ge->camera2D.y = m.top; }
		//マップがカメラより小さい場合
		if (this->hitBase.w < ge->camera2D.w) { ge->camera2D.x = m.left; }
		if (this->hitBase.h < ge->camera2D.h) { ge->camera2D.y = m.top; }
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