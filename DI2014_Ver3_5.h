#pragma warning(disable:4996)
#pragma once
#include <windows.h>
#define DIRECTINPUT_VERSION	0x0800
#pragma comment(lib,"dxguid")
#pragma comment(lib,"dInput8")
#include <dInput.h>
#include "myLib.h"
#include <string>
#include <vector>

//┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//┃入力制御系機能群							　　┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
namespace DI
{
	namespace  KS {
		enum Type {	//キー状態	
			Err,
			On,		//押している間中
			Off,	//離している間中
			Down,	//押した直後（１フレームのみ反応）
			Up,		//離した直後（１フレームのみ反応）
		};
	}
	struct  ButtonInfo {
		bool  on = false;
		bool  off = true;
		bool  down = false;
		bool  up = false;
		//ボタンをONに書き換える
		void  SetOn() {   on = 1; off = 0; down = 0; up = 0; }
		//ボタンをOFFに書き換える
		void  SetOff() {  on = 0; off = 1; down = 0; up = 0; }
		//ボタンをDOWNに書き換える
		void  SetDown() { on = 1; off = 0; down = 1; up = 0; }
		//ボタンをUPに書き換える
		void  SetUp() {   on = 0; off = 1; down = 0; up = 1; }
	};
	struct  StickInfo {
		float                angle;		//アナログスティックの角度（RT0度　DW90度のラジアン）
		float                volume;	//アナログスティックの強弱0.0f～1.0f
		ML::Vec2             axis;		//軸毎の量-1.0f～+1.0f
		ButtonInfo			 L;
		ButtonInfo			 R;
		ButtonInfo			 U;
		ButtonInfo			 D;
		StickInfo() {
			this->angle = 0;
			this->volume = 0;
			this->axis = ML::Vec2(0,0);
			this->L.SetOff();
			this->R.SetOff();
			this->U.SetOff();
			this->D.SetOff();
		}
	};
	namespace  But {
		enum Type {	//プログラム上で使用できるキーの種類
			//○,
			B1,
			//×
			B2,
			//△
			B3,
			//□
			B4,
			L1, R1, L2, R2,
			ST, SE, L3, R3,
			//←
			HL,		//ハットスイッチ
			//→
			HR,
			//↑
			HU,
			//↓
			HD,
			S0, S1, S2, S3, S4, S5, S6, S7, S8, S9,
			KMax		//　ボタン総数
		};
	}


	namespace GPB	//ゲームパッドのキーを表す
	{
		enum Type {
			B01, B02, B03, B04, B05, B06, B07, B08,
			B09, B10, B11, B12, B13, B14, B15, B16,
			HSL, HSR, HSU, HSD,		//ハットスイッチ 
			KMax				//　ボタン総数
		};
	}

	struct  AnalogAxisKB {
		DWORD           vKeyL;	//キーボードのバーチャルキー
		DWORD           vKeyR;	//キーボードのバーチャルキー
		DWORD           vKeyU;	//キーボードのバーチャルキー
		DWORD           vKeyD;	//キーボードのバーチャルキー
	};
	//キー設定キーボード用
	struct  KD_KB {
		DWORD           vKey;	//キーボードのバーチャルキー
		But::Type       pKey;	//ボタンの種類（ＰＧ上での名称）
	};
	typedef  vector<KD_KB>	KeyDatas_KB;

	//キー設定ゲームパッド用
	struct  KD_GP {
		GPB::Type		gKey;	//ゲームパッドのボタン
		But::Type       pKey;	//ボタンの種類（ＰＧ上での名称）
	};
	typedef  vector<KD_GP>	KeyDatas_GP;

	//例外として投げられるエラー情報
	namespace Err {
		enum Type
		{
			NOTHING,		//エラーなし
			CREATE,			//生成失敗
			NOTDEVICE,		//デバイスが取得的ない
			UNNONTYPE,		//不明なデバイス種類
			ENUMDEVICE,		//デバイスの列挙に失敗
			CREATEDEVICE,	//デバイスの生成失敗
			DATAFORMAT,		//データフォーマットの設定失敗
			COOPLEVEL,		//協調レベル設定失敗
			GPAXIS,			//
			KBPROPERTY,
		};
	}
		//仮想コントローラ構造体
		struct  VGamePad {
			StickInfo  LStick;
			StickInfo  RStick;
			ButtonInfo  B1;
			ButtonInfo  B2;
			ButtonInfo  B3;
			ButtonInfo  B4;
			ButtonInfo  L1;
			ButtonInfo  L2;
			ButtonInfo  L3;
			ButtonInfo  R1;
			ButtonInfo  R2;
			ButtonInfo  R3;
			ButtonInfo  ST;
			ButtonInfo  SE;
			ButtonInfo  HL;
			ButtonInfo  HR;
			ButtonInfo  HU;
			ButtonInfo  HD;
			ButtonInfo  S0;
			ButtonInfo  S1;
			ButtonInfo  S2;
			ButtonInfo  S3;
			ButtonInfo  S4;
			ButtonInfo  S5;
			ButtonInfo  S6;
			ButtonInfo  S7;
			ButtonInfo  S8;
			ButtonInfo  S9;
		};


	class  GamePad
	{
	public:
		typedef  shared_ptr<GamePad>  SP;
		typedef  weak_ptr<GamePad>  WP;

	private:
		enum  DeviceKind { KeyBoard, GPad, UnNon };	//デバイスの種類
		struct sKeyParam {
			DWORD             dKey;		//実際のボタンまたはキーボードのバーチャルキー
			But::Type		  pKey;		//ボタンの種類（ＰＧ上での名称）
			KS::Type		  state;	//キーの状態
		};
		IDirectInputDevice8*  device;
		DeviceKind            dk;
		vector<sKeyParam>     buttons;
		vector<DWORD>         kbAxisL;	//キーボード専用方向キー
		vector<DWORD>         kbAxisR;	//キーボード専用方向キー
		vector<DWORD>         kbPov4;	//キーボード専用POVキー
		StickInfo		      ls, rs;
		SP  link;
	private:
		GamePad();
		bool  Initialize(
			DeviceKind    dk_,
			DWORD         id_,
			AnalogAxisKB* axisL_,
			AnalogAxisKB* axisR_,
			KeyDatas_KB		buttonKB_,
			KeyDatas_GP		buttonGP_);
		Err::Type  CreateDevice(DWORD  id_);

		void  CheckKB();
		void  CheckGP();
		void  CheckUnNon();

		//	ボタンの状態を確認
		KS::Type  CheckBT(But::Type  kk_);
		//	アナログスティックの入力を得る
		StickInfo  StickL();
		StickInfo  StickR();
		//
		static  SP  Create(
			DeviceKind		dk_,
			DWORD			id_,
			AnalogAxisKB*	axisL_,
			AnalogAxisKB*	axisR_,
			KeyDatas_KB		buttonKB_,
			KeyDatas_GP		buttonGP_);
	public:
		~GamePad();
		//	デバイスを生成する
		static  SP  CreateKB(
			AnalogAxisKB	axisL_,
			AnalogAxisKB	axisR_,
			KeyDatas_KB		buttonKB_);
		static  SP  CreateGP(
			DWORD			id_,
			KeyDatas_GP		buttonGP_);
		//	更新
		void  UpDate();
		//２つのデバイスを統合する
		//安全性を考えるなら、クラスメソッドのLinkを使う事
		SP  Link(SP  l) {
			if (this == l.get()) { return 0; }
			else { SP w = link; link = l; return link; }
		}
		//２つのデバイスを統合する
		//片方のデバイスが生成できていない場合の対応あり
		static  GamePad::SP  Link(SP  a, SP  b);

		//ゲームパッドのボタンの状態を取得する
		VGamePad  GetState();
	};
	//--------------------------------
	namespace MB	//マウスのボタンを表す
	{
		enum Type {  //マウスボタンの種類 8個まで認識可能
			LB, RB, CB, RESERVED1,
			RESERVED2, RESERVED3, RESERVED4, RESERVED5,
			kMax
		};
	}
	//仮想マウス構造体
	struct  VMouse {
		POINT         pos;
		POINT         pos_Screen;
		int			  wheel;
		ButtonInfo	  LB;
		ButtonInfo	  RB;
		ButtonInfo	  CB;
		ButtonInfo	  B1;
		ButtonInfo	  B2;
		ButtonInfo	  B3;
		ButtonInfo	  B4;
		ButtonInfo	  B5;
	};
	class Mouse {
	private:
		//    IDirectInputDevice8*	device;
		//--------------------------------
		//マウス
		LPDIRECTINPUTDEVICE8 mouse;
		DIMOUSESTATE2 mouseState;
		POINT		  pos_Screen;	//スクリーン座標
		POINT		  pos;			//クライアント座標
		long          mouseWheel;
		KS::Type      mouseButton[MB::kMax];
		Err::Type     init(float, float);
		float         scaleX, scaleY;
	public:
	private:
		KS::Type     CheckBT(MB::Type mb_);
		Mouse();
	public:
		~Mouse();
		typedef shared_ptr<Mouse> SP;
		typedef weak_ptr<Mouse> WP;
		//生成
		static SP   Create(float scaleX_ = 1.f, float scaleY_ = 1.f);
		//	更新
		void UpDate();
		//
		VMouse  GetState();
	};
	class  Obj
	{
	public:
		typedef  weak_ptr<Obj>  WP;
		typedef  shared_ptr<Obj>  SP;
	private:
		IDirectInput8*  di;
		HWND  wnd;
		static  Obj::WP  winst;		//	１つしか存在できないインスタンスのウィークポインタ
		Obj();
		bool  Initialize(HWND wnd_);

		static  vector<GamePad::WP>  gpads;
		static  vector<Mouse::WP>    mouses;

	public:
		~Obj();
		//	DIライブラリを生成する
		static  Obj::SP  Create(HWND  wnd_);
		static  Obj::SP  GetInst();
		HWND  Wnd() { return  wnd; }
		IDirectInput8&  DInput() { return  *di; }
		//毎フレーム呼び出し
		void UpDate();
		void Set(GamePad::SP s_);
		void Set(Mouse::SP s_);
	};

	//---------------------------------------------------------------------------
	//ゲームパッドオブジェクト
	//登録
	void  Managed_GamePadSet(const string name_, DI::GamePad::SP sp_);
	//名前で検索
	DI::GamePad::SP  Managed_GamePadGet(const string name_);
	//単体破棄
	void  Managed_GamePadClear(const string name_);
	//すべて破棄
	void  Managed_GamePadAllClear();
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	//ゲームパッドマネージャー
	class  GamePadManager {
	private:
		static  map<string, DI::GamePad::SP>	arr;
		static  bool notObject;
		GamePadManager() { notObject = false; }
	public:
		//登録
		static  void  Set(const string name_, DI::GamePad::SP sp_);
		//名前で検索
		static  DI::GamePad::SP  Get(const string name_);
		//単体破棄
		static  void  Clear(const string name_);
		//すべて破棄
		static  void  AllClear();
	};
	//---------------------------------------------------------------------------
	//マウスマネージャー
	class  MouseManager {
	private:
		static  map<string, DI::Mouse::SP>	arr;
		static  bool notObject;
		MouseManager() { notObject = false; }
	public:
		//登録
		static  void  Set(const string name_, DI::Mouse::SP sp_);
		//名前で検索
		static  DI::Mouse::SP  Get(const string name_);
		//単体破棄
		static  void  Clear(const string name_);
		//すべて破棄
		static  void  AllClear();
	};
}