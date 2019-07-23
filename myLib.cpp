#include "myLib.h"
#include "DG2014/DG2014_Ver3_6.h"

namespace ML
{
	//値を所定値に近づける
	void  ValueControl_ToValue(float&  v_,float  to_,  float  aja_)
	{
		if(v_  ==  to_){  return;}
		else{
			if(v_  >=  to_){	v_  =  max(v_ - aja_, to_);}
			else{				v_  =  min(v_ - aja_, to_);}
		}
		
	}
	//ベクトルの水平成分の長さを規定値に近づける
	void  VectorControl_XZ_ToValue(Vec3&  v_,float  to_,  float  aja_)
	{
		Vec3  xz(v_.x,  0,  v_.z);
		float  len  =  xz.Length( );

		if(len  ==  0){
			xz  =  ML::Vec3(0,0,0);
		}
		else{
			float  nLen  =  len;
			ValueControl_ToValue(nLen,  to_,  aja_);
			float  vr  =  nLen  /  len;	//倍率を求める
			xz  *=  vr;	//現在の長さから調整後の長さに変更する
		}
		v_.x  =  xz.x;
		v_.z  =  xz.z;
	}
	//ベクトルの長さが規定値を超えている場合、規定値まで短くする
	void  VectorControl_XZ_Cut(Vec3&  v_,float  to_)
	{
		Vec3  xz(v_.x,  0,  v_.z);
		float  len  =  xz.Length( );
		if(len  >  to_){
			float  vr  =  to_  /  len;	//倍率を求める
			xz  *=  vr;	//現在の長さから調整後の長さに変更する
			v_.x  =  xz.x;
			v_.z  =  xz.z;
		}
	}

	
	Vec2::Vec2( ):D3DXVECTOR2(0,0){	}
	Vec2::Vec2(const float  x_,const float  y_):D3DXVECTOR2(x_,y_){}
	Vec2::Vec2(const D3DXVECTOR2&  v_):D3DXVECTOR2(v_){}
	Vec2  Vec2::operator  = (const D3DXVECTOR2&  v_){ return  (*this)  =  Vec2(v_); }
	Vec2  Vec2::Normalize()const{ return  Vec2((*this) / this->Length()); }
	float  Vec2::Length()const{ return  D3DXVec2Length(this); }

	Vec3::Vec3( ):D3DXVECTOR3(0,0,0){	}
	Vec3::Vec3(const float  x_,const float  y_,const float  z_):D3DXVECTOR3(x_,y_,z_){}
	Vec3::Vec3(const D3DXVECTOR3&  v_):D3DXVECTOR3(v_){}
	Vec3  Vec3::operator  = (const D3DXVECTOR3&  v_){ return  (*this)  =  Vec3(v_); }
	class  Vec4  Vec3::Vec4(  ){	return  class  Vec4(this->x, this->y, this->z, 1);}
	Vec3  Vec3::Normalize()const{ return  (*this) / this->Length(); }
	float  Vec3::Length(  )const{		return  D3DXVec3Length(this);		}	

	Vec4::Vec4( ):D3DXVECTOR4(0,0,0,0){	}
	Vec4::Vec4(const float  x_,const float  y_,const float  z_,const float  w_):D3DXVECTOR4(x_,y_,z_,w_){}
	Vec4::Vec4(const D3DXVECTOR4&  v_):D3DXVECTOR4(v_){}
	Vec4  Vec4::operator  = (const D3DXVECTOR4&  v_){ return  (*this)  =  Vec4(v_); }
	class  Vec3  Vec4::Vec3(  ){	return  class  Vec3(this->x/this->w, this->y/this->w, this->z/this->w);}







	float Gravity(const  float  byM_)
	{
		//return  (9.8f * byM_) / 60.0f / 60.0f;
		const float  gra = 9.8f / 60 /60;
		return  gra * byM_;
	}

	float ToRadian(const  float  degree_)
	{
		return  degree_ * PI / 180.0f;
	}
	float ToDegree(const  float	radian_)
	{
		return  radian_ * 180.0f / PI;
	}

	int	CheckStrings(TCHAR* s_[], int n_, const string& c_)
	{
		int c;
		for(c=0; c < n_; ++c){
			if(c_ == s_[c]){return c;}
		}
		return -1;
	}
	map<string, bool>  msgboxlog;
	void  MsgBox(const string&  ms){
		if (msgboxlog[ms] == false){
			MessageBox(nullptr, ms.c_str(), nullptr, MB_OK);
			msgboxlog[ms] = true;
		}
	}


	//クォータニオン----------------------------------------------------------------
	void QT::RotAx(const  ML::Vec3&  v_,  float  r_){	D3DXQuaternionRotationAxis(this, &v_, r_);}
	void QT::RotX(float  rX_){					RotAx(ML::Vec3(1,0,0), rX_);}
	void QT::RotY(float  rY_){					RotAx(ML::Vec3(0,1,0), rY_);}
	void QT::RotZ(float  rZ_){					RotAx(ML::Vec3(0,0,1), rZ_);}
	QT::QT( ){									RotY(0);}
	QT::QT(float  rY_){							RotY(rY_);}
	QT::QT(const  ML::Vec3&  v_,  float  r_){	RotAx(v_, r_);}
	QT  QT::operator * ( const D3DXQUATERNION&  m_) const
	{
	    QT  rtv;
	    D3DXQuaternionMultiply(&rtv, this, &m_);
	    return rtv;
	}
	QT& QT::operator *= ( const D3DXQUATERNION&  m_ )
	{
	    D3DXQuaternionMultiply(this, this, &m_);
		return *this;
	}
	void QT::RotXYZ(const  ML::Vec3  rXYZ_)
	{
		QT  x, y, z;
		x.RotX(rXYZ_.x);
		y.RotY(rXYZ_.y);
		z.RotZ(rXYZ_.z);
		(*this)  =  x * y * z;
	}
	//行列--------------------------------------------------------------------------
	Mat4x4::Mat4x4( ){	}
	Mat4x4::Mat4x4(const D3DXMATRIX&  v_):D3DXMATRIX(v_){	}

	Mat4x4  Mat4x4::operator  = (const D3DXMATRIX&  v_)
	{
		return  (*this)  =  Mat4x4(v_); 
	}
	//単位行列
	void  Mat4x4::Identity( )
	{
		D3DXMatrixIdentity(this);
	}
	//ビュー行列
	void  Mat4x4::LookAtLH(const  Vec3& pos_,  const  Vec3&  target_,  const  Vec3&  up_ )
	{
		D3DXMatrixLookAtLH(	this, &pos_, &target_, &up_);
	}
	//射影行列
	void  Mat4x4::PerspectiveFovLH(float  fov_,  float  aspect_,  float  nearPlane_,  float  forePlane_ )
	{
		D3DXMatrixPerspectiveFovLH(	this, fov_, aspect_, nearPlane_, forePlane_);
	}
	//固定軸回転行列
	void  Mat4x4::RotationX(float  r_)
	{
		D3DXMatrixRotationX(this, r_);
	}
	void  Mat4x4::RotationY(float  r_)
	{
		D3DXMatrixRotationY(this, r_);
	}
	void  Mat4x4::RotationZ(float  r_)
	{ 
		D3DXMatrixRotationZ(this, r_);
	}
	//任意軸回転行列
	void  Mat4x4::RotationAxis(const  Vec3&  v_, float  r_)
	{
		D3DXMatrixRotationAxis(this, &v_, r_); 
	}
	//クオータニオンで回転行列
	void  Mat4x4::RotationQuaternion(const  QT&  r_)
	{
		D3DXMatrixRotationQuaternion (this, &r_);
	}
	//平行移動行列
	void  Mat4x4::Translation(const  Vec3&  p_)
	{
		D3DXMatrixTranslation (this, p_.x, p_.y, p_.z);
	}
	//スケーリング
	void  Mat4x4::Scaling(const  Vec3&  s_)
	{
		D3DXMatrixScaling (this, s_.x, s_.y, s_.z);
	}
	void  Mat4x4::Scaling(float  s_)
	{
		D3DXMatrixScaling (this, s_, s_, s_);
	}


	//逆行列生成
	Mat4x4  Mat4x4::Inverse( )
	{
		Mat4x4  w;
		D3DXMatrixInverse(&w, NULL, this);
		return  w;
	}
	//転置行列生成
	Mat4x4  Mat4x4::Transpose( ) const
	{
		Mat4x4  w;
		D3DXMatrixTranspose(&w, this);
		return w;
	}
	//座標変換
	Vec3  Mat4x4::TransformCoord(const Vec3&  p_)
	{
		Vec3  w;
		D3DXVec3TransformCoord(&w, &p_, this);
		return  w;
	}
	Vec3  Mat4x4::TransformNormal(const Vec3&  v_)
	{
		Vec3  w;
		D3DXVec3TransformNormal(&w, &v_, this);
		return  w;
	}

}

namespace ML
{
	namespace Collsion	//COLLISION
	{
		//レイとポリゴンの接触判定
		bool HitRayTriangle(const  ML::Vec3&  p_, const  ML::Vec3&  d_,
			const  ML::Vec3&  v0_, const  ML::Vec3&  v1_, const  ML::Vec3&  v2_,
			float&  dist_)
		{
			{
				float  len = d_.Length();
				const  FXMVECTOR  p = XMLoadFloat3(&XMFLOAT3(p_));
				const  FXMVECTOR  d = XMLoadFloat3(&XMFLOAT3(d_ / len));
				const  FXMVECTOR  v0 = XMLoadFloat3(&XMFLOAT3(v0_));
				const  FXMVECTOR  v1 = XMLoadFloat3(&XMFLOAT3(v1_));
				const  FXMVECTOR  v2 = XMLoadFloat3(&XMFLOAT3(v2_));

				bool  rtv = TRUE == XNA::IntersectRayTriangle(p, d, v0, v1, v2, &dist_);
				if (rtv){
					dist_ /= len;
				}
				return  rtv;
			}
			//{//↑と同一結果になるコード（こちらは自力で実装）
			//	BOOL IntersectRayTriangle( FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0, CXMVECTOR V1, CXMVECTOR V2,
			//					   FLOAT* pDist );


			//	dist_  = 0;
			//	//面法線
			//	ML::Vec3	t0 = v1_ - v0_;
			//	ML::Vec3	t1 = v2_ - v0_;
			//	ML::Vec3	nm;
			//	D3DXVec3Cross(&nm,  &t0,  &t1);
			//	nm  =  ML::Normalize(nm);
			//	//線分判定(ポリゴン平面上の座標を求める）
			//	ML::Vec3	xp  =  v0_  -  p_;
			//	float   xpn  =  D3DXVec3Dot(&xp,  &nm);
			//	float   vn   =  D3DXVec3Dot(&d_,  &nm);
			//	if(-0.00001f <=  vn){ return  false;}//交差しない（平行・逆向き）
			//	float  t = xpn/vn;
			//	if(t<0){ return  false;}//始点がポリゴンの裏側にある
			//	//ポリゴン内（３辺の内積が0以上ならポリゴン内部）
			//	ML::Vec3  tp  =  p_  +  t *  d_;
			//	{
			//		ML::Vec3  d0  =  tp  -  v1_;
			//		ML::Vec3  d1  =  v0_  -  v1_;
			//		ML::Vec3  c;
			//		D3DXVec3Cross(&c,  &d0,  &d1);
			//		if(D3DXVec3Dot(&c,  &nm)  <  0){	return  false;}
			//	}		
			//	{
			//		ML::Vec3  d0  =  tp  -  v2_;
			//		ML::Vec3  d1  =  v1_  -  v2_;
			//		ML::Vec3  c;
			//		D3DXVec3Cross(&c,  &d0,  &d1);
			//		if(D3DXVec3Dot(&c,  &nm)  <  0){	return  false;}
			//	}		
			//	{
			//		ML::Vec3  d0  =  tp  -  v0_;
			//		ML::Vec3  d1  =  v2_  -  v0_;
			//		ML::Vec3  c;
			//		D3DXVec3Cross(&c,  &d0,  &d1);
			//		if(D3DXVec3Dot(&c,  &nm)  <  0){	return  false;}
			//	}
			//	dist_  =  t;
			//	return  true;
			//}
		}
		//レイとバーテックスバッファの接触判定（バーテックスバッファの構造に注意）
		//頂点が３個毎に１ポリゴンを構成しているデータでなければ機能しない
		bool HitRayTriangleA(const  ML::Vec3&  p_, const  ML::Vec3&  d_,
			shared_ptr<DG::VertexBuffer>		vb_, const  UINT pos_,
			float&  dist_)
		{
			BYTE  *p = (BYTE*)vb_->memory;
			p += pos_;
			UINT  st = vb_->stride;
			bool   hit = false;
			for (UINT n = 0; n < vb_->num; n += 3){
				ML::Vec3	*p0, *p1, *p2;
				p0 = (ML::Vec3*)&p[0];
				p1 = (ML::Vec3*)&p[st];
				p2 = (ML::Vec3*)&p[st * 2];
				float  distW = 0;

				bool  rtv = HitRayTriangle(p_, d_, *p0, *p1, *p2, distW);
				if (rtv == true){
					if (hit == false){
						dist_ = distW;
					}
					else if (dist_ > distW){
						dist_ = distW;
					}
					hit = true;
				}
				p += st * 3;
			}
			return  hit;
		}
		bool HitRayTriangleQ(const  ML::Vec3&  p_, const  ML::Vec3&  d_,
			shared_ptr<DG::VertexBuffer>	vb_, const  UINT pos_,
			float&  dist_)
		{
			BYTE  *p = (BYTE*)vb_->memory;
			p += pos_;
			UINT  st = vb_->stride;
			bool   hit = false;
			for (UINT n = 0; n < vb_->num; n += 3){
				ML::Vec3	*p0, *p1, *p2;
				p0 = (ML::Vec3*)&p[0];
				p1 = (ML::Vec3*)&p[st];
				p2 = (ML::Vec3*)&p[st * 2];
				float  distW = 0;

				bool  rtv = HitRayTriangle(p_, d_, *p0, *p1, *p2, distW);
				if (rtv == true){
					if (hit == false){
						dist_ = distW;
					}
					else if (dist_ > distW){
						dist_ = distW;
					}
					hit = true;
					break;
				}
				p += st * 3;
			}
			return  hit;


		}
		bool HitRayTriangle(const  ML::Vec3&  p_, const  ML::Vec3&  d_, bool  qmode_,
			shared_ptr<DG::VertexBuffer>		vb_, const  UINT pos_,
			float&  dist_)
		{
			if (qmode_){ return  HitRayTriangleQ(p_, d_, vb_, pos_, dist_); }
			else{ return  HitRayTriangleA(p_, d_, vb_, pos_, dist_); }

		}
		template<class  T>
		bool HitRayTriangleIBVBQ(const  ML::Vec3&  p_, const  ML::Vec3&  d_,
			shared_ptr< DG::VertexBuffer>		vb_, const  UINT pos_,
			shared_ptr<DG::IndexBuffer>		ib_,
			float&  dist_)
		{
			T  *idx = (T*)ib_->memory;

			BYTE  *p = (BYTE*)vb_->memory;
			p += pos_;
			UINT  st = vb_->stride;
			bool   hit = false;
			for (UINT n = 0; n < ib_->num; n += 3){
				ML::Vec3	*p0, *p1, *p2;
				p0 = (ML::Vec3*)&p[idx[0] * st];
				p1 = (ML::Vec3*)&p[idx[1] * st];
				p2 = (ML::Vec3*)&p[idx[2] * st];
				float  distW = 0;

				bool  rtv = HitRayTriangle(p_, d_, *p0, *p1, *p2, distW);
				if (rtv == true){
					if (hit == false){
						dist_ = distW;
					}
					else if (dist_ > distW){
						dist_ = distW;
					}
					hit = true;
					break;
				}
				idx += 3;
			}
			return  hit;
		}
		template<class  T>
		bool HitRayTriangleIBVBA(const  ML::Vec3&  p_, const  ML::Vec3&  d_,
			shared_ptr< DG::VertexBuffer>		vb_, const  UINT pos_,
			shared_ptr<DG::IndexBuffer>		ib_,
			float&  dist_)
		{
			T  *idx = (T*)ib_->memory;

			BYTE  *p = (BYTE*)vb_->memory;
			p += pos_;
			UINT  st = vb_->stride;
			bool   hit = false;
			for (UINT n = 0; n < ib_->num; n += 3){
				ML::Vec3	*p0, *p1, *p2;
				p0 = (ML::Vec3*)&p[idx[0] * st];
				p1 = (ML::Vec3*)&p[idx[1] * st];
				p2 = (ML::Vec3*)&p[idx[2] * st];
				float  distW = 0;

				bool  rtv = HitRayTriangle(p_, d_, *p0, *p1, *p2, distW);
				if (rtv == true){
					if (hit == false){
						dist_ = distW;
					}
					else if (dist_ > distW){
						dist_ = distW;
					}
					hit = true;
				}
				idx += 3;
			}
			return  hit;
		}
		bool HitRayTriangle(const  ML::Vec3&  p_, const  ML::Vec3&  d_, bool  qmode_,
			shared_ptr< DG::VertexBuffer>		vb_, const  UINT pos_,
			shared_ptr<DG::IndexBuffer>		ib_,
			float&  dist_)
		{
			if (ib_->stride == 2){
				if (qmode_){ return  HitRayTriangleIBVBQ<WORD>(p_, d_, vb_, pos_, ib_, dist_); }
				else{ return  HitRayTriangleIBVBA<WORD>(p_, d_, vb_, pos_, ib_, dist_); }
			}
			else if (ib_->stride == 4){
				if (qmode_){ return  HitRayTriangleIBVBQ<DWORD>(p_, d_, vb_, pos_, ib_, dist_); }
				else{ return  HitRayTriangleIBVBA<DWORD>(p_, d_, vb_, pos_, ib_, dist_); }
			}
			return false;
		}
		//------------------------------------------------------------------------------------------
		//軸平行な箱（ＡＡＢＢ）と点の位置関係から適切な排出方向（６面方向）を求める処理
		ML::Vec3  CheckBox6(
			const  ML::Vec3&  bsh_,		//ボックスのサイズ（半分）
			const  ML::Vec3&  pos_)		//ボックスの中心点から見た点の座標
		{
			float  XZ = (+bsh_.x)  *  pos_.z - (+bsh_.z)  *  pos_.x;		//X+ Z+ に対する外積
			float  xZ = (-bsh_.x)  *  pos_.z - (+bsh_.z)  *  pos_.x;		//X- Z+ に対する外積

			float  XY = (+bsh_.x)  *  pos_.y - (+bsh_.y)  *  pos_.x;		//X+ Y+ に対する外積
			float  xY = (-bsh_.x)  *  pos_.y - (+bsh_.y)  *  pos_.x;		//X- Y+ に対する外積

			float  YZ = (+bsh_.y)  *  pos_.z - (+bsh_.z)  *  pos_.y;		//Y+ Z+ に対する外積
			float  yZ = (-bsh_.y)  *  pos_.z - (+bsh_.z)  *  pos_.y;		//Y- Z+ に対する外積

			//排出方向がＺ＋の場合（Ｚ＋面に接触）
			if (XZ >= 0 && xZ <= 0 && YZ >= 0 && yZ <= 0){ return  ML::Vec3(0, 0, +1); }
			//排出方向がＺ－の場合（Ｚ－面に接触）
			else if (XZ <= 0 && xZ >= 0 && YZ <= 0 && yZ >= 0){ return  ML::Vec3(0, 0, -1); }
			//排出方向がＸ＋の場合（Ｘ＋面に接触）
			else if (XZ <= 0 && xZ <= 0 && XY <= 0 && xY <= 0){ return  ML::Vec3(+1, 0, 0); }
			//排出方向がＸ－の場合（Ｘ－面に接触）
			else if (XZ >= 0 && xZ >= 0 && XY >= 0 && xY >= 0){ return  ML::Vec3(-1, 0, 0); }
			//排出方向がＹ＋の場合（Ｙ＋面に接触）
			else if (XY >= 0 && xY <= 0 && YZ <= 0 && yZ <= 0){ return  ML::Vec3(0, +1, 0); }
			//排出方向がＹ－の場合（Ｙ－面に接触）
			//else if (XY  <=  0  &&  xY >= 0  &&  YZ  >=  0  &&  yZ  >=  0){
			else{/*必然的に無条件でコレになる(する）*/					   return  ML::Vec3(0, -1, 0); }
		}
	//図形クラスの基底クラス--------------------------------------------------------------------------
		//接触判定
		bool Shape::Hit(const Shape::SP  b_, ExtParam&  ex_)
		{
			const Shape*  p = b_.get();
			switch (b_->kind){
			default:			return  false;	break;
			case  eAABB:		return  Hit(*(static_cast<const  class  AABB*>(p)),ex_);	break;	//AABBとの接触
			case  eSphere:		return  Hit(*(static_cast<const  class  Sphere*>(p)),ex_);	break;	//球体との接触
			case  eOBB:			return  Hit(*(static_cast<const  class  OBB*>(p)),ex_);		break;	//OBBとの接触
				//★図形を増やしたら追加★
			}
		}
	//AABB--------------------------------------------------------------------------
		//生成
		AABB::SP  AABB::Create(const ML::Vec3&  p_, const ML::Vec3& s_)
		{
			shared_ptr<AABB>  ob = shared_ptr<AABB>(new  AABB());
			if (ob){
				if (ob->Initialize(p_, s_)){
					return  ob;
				}
			}
			return nullptr;
		}
		//初期化
		bool  AABB::Initialize(const ML::Vec3&  p_, const ML::Vec3& s_)
		{
			this->pos = p_;
			this->size_h = s_;
			this->kind = Shape::eAABB;
			return  true;
		}
		//コピーを生成して返す
		Shape::SP   AABB::Clone()
		{
			return  AABB::Create(this->pos, this->size_h);
		}

		//AABBとの接触
		bool AABB::Hit(const class AABB& b_, ExtParam&  ex_)
		{
			XNA::AxisAlignedBox m = { pos, size_h };
			XNA::AxisAlignedBox n = { b_.pos, b_.size_h };
			bool rtv = TRUE == XNA::IntersectAxisAlignedBoxAxisAlignedBox(&m, &n);
			if (rtv){
				//排出方向のベクトルを生成する
				//★仮処理★
				//ex_.pushOut = b_.pos - this->pos;
				//AABBとAABB場合、AABBと点のアルゴリズムと同じ処理で排出方向を求められる
				//ただし、排出を行う側のAABBを2つの合計にして行う
				ex_.pushOut = CheckBox6(size_h + b_.size_h, b_.pos - pos);
			}
			return rtv;
		}
		//球体との接触
		bool AABB::Hit(const class Sphere& b_, ExtParam&  ex_)
		{
			XNA::Sphere		 m = { b_.pos, b_.rad };
			XNA::AxisAlignedBox n = { pos, size_h };
			bool rtv = TRUE == XNA::IntersectSphereAxisAlignedBox(&m, &n);
			if (rtv){
				//排出方向のベクトルを生成する
				//★仮処理★
				//ex_.pushOut = b_.pos - this->pos;
				//AABBから球体を排出する場合、AABBと点のアルゴリズムと同じ処理で排出方向を求められる
				ex_.pushOut = CheckBox6(size_h, b_.pos - pos);
			}
			return rtv;
		}
		//OBBとの接触
		bool AABB::Hit(const class OBB& b_, ExtParam&  ex_)
		{
			XNA::AxisAlignedBox m = { pos, size_h };
			XNA::OrientedBox n = { b_.pos, b_.size_h, b_.angle };
			bool rtv = TRUE == XNA::IntersectAxisAlignedBoxOrientedBox(&m, &n);
			if (rtv){
				//排出方向のベクトルを生成する
				//★仮処理★
				//ex_.pushOut = b_.pos - this->pos;
				//AABBからOBBを排出する場合、精度の低い点排出と同じ方法をとる
				ex_.pushOut = CheckBox6(size_h, b_.pos - pos);
			}
			return rtv;
		}
		//Rayとの接触
		bool AABB::HitRay(	const ML::Vec3&  p_,
							const ML::Vec3&  d_,
							float&  dist_)
		{
			float  length = d_.Length( );
			const  FXMVECTOR  p = XMLoadFloat3(&XMFLOAT3(p_));
			const  FXMVECTOR  d = XMLoadFloat3(&XMFLOAT3(d_ / length));
			XNA::AxisAlignedBox n = { pos, size_h };
			bool  rtv = TRUE == XNA::IntersectRayAxisAlignedBox(p, d, &n, &dist_);
			//レイの長さに合わせて距離（割合）を調整
			if (rtv){ dist_ /= length; }
			return  rtv;
		}
	//球体--------------------------------------------------------------------------
		//生成
		Sphere::SP  Sphere::Create(const  ML::Vec3&  p_, float  r_)
		{
			shared_ptr<Sphere>  ob = shared_ptr<Sphere>(new  Sphere());
			if (ob){
				if (ob->Initialize(p_, r_)){
					return  ob;
				}
			}
			return nullptr;
		}
		//初期化
		bool  Sphere::Initialize(const  ML::Vec3&  p_, float  r_)
		{
			this->pos = p_;
			this->rad = r_;
			this->kind = Shape::eSphere;
			return  true;
		}
		//コピーを生成して返す
		Shape::SP   Sphere::Clone()
		{
			return  Sphere::Create(this->pos, this->rad);
		}

		//AABBとの接触
		bool Sphere::Hit(const class AABB& b_, ExtParam&  ex_)
		{
			XNA::Sphere		 m = { pos, rad };
			XNA::AxisAlignedBox n = { b_.pos, b_.size_h };
			bool rtv = TRUE == XNA::IntersectSphereAxisAlignedBox(&m, &n);
			if (rtv){
				//排出方向のベクトルを生成する
				//★仮処理★
				//ex_.pushOut = b_.pos - this->pos;
				//球体とAABB場合、AABBから球体を排出するベクトルの逆を使用する
				ex_.pushOut = -CheckBox6(b_.size_h, pos - b_.pos);

			}
			return rtv;
		}
		//球体との接触
		bool Sphere::Hit(const class Sphere& b_, ExtParam&  ex_)
		{
			XNA::Sphere		 m = { pos, rad };
			XNA::Sphere		 n = { b_.pos, b_.rad };
			bool rtv = TRUE == XNA::IntersectSphereSphere(&m, &n);
			if (rtv){
				//排出方向のベクトルを生成する
				//押し出すスフィアの中心方向に向かうベクトル
				ex_.pushOut = b_.pos - this->pos;
			}
			return rtv;
		}
		//OBBとの接触
		bool Sphere::Hit(const class OBB& b_, ExtParam&  ex_)
		{
			XNA::Sphere		 m = { pos, rad };
			XNA::OrientedBox n = { b_.pos, b_.size_h, b_.angle };
			bool rtv = TRUE == XNA::IntersectSphereOrientedBox(&m, &n);
			if (rtv){
				//排出方向のベクトルを生成する
				//★仮処理★
				//ex_.pushOut = b_.pos - this->pos;
				//ＯＢＢのもつ回転情報を使ってＯＢＢを中心としてスフィアの中心点を逆回転させる
				Mat4x4  inv_mat, mat;
				mat.RotationQuaternion(b_.angle);
				inv_mat = mat.Inverse();
				//相対座標を求める
				ML::Vec3  irSpherePos = inv_mat.TransformNormal(this->pos - b_.pos);
				//AABBからスフィアを排出する方向を求め、その逆ベクトルを得る
				ex_.pushOut = -CheckBox6(b_.size_h, irSpherePos);
				//そのベクトルをＯＢＢの回転に合わせて回転させる
				ex_.pushOut = mat.TransformNormal(ex_.pushOut);
			}
			return rtv;
		}
		//Rayとの接触
		bool Sphere::HitRay(const ML::Vec3&  p_,
			const ML::Vec3&  d_,
			float&  dist_)
		{
			float  length = d_.Length();
			const  FXMVECTOR  p = XMLoadFloat3(&XMFLOAT3(p_));
			const  FXMVECTOR  d = XMLoadFloat3(&XMFLOAT3(d_ / length));
			XNA::Sphere		 n = { pos, rad };
			bool  rtv = TRUE == XNA::IntersectRaySphere(p, d, &n, &dist_);
			//レイの長さに合わせて距離（割合）を調整
			if (rtv){ dist_ /= length; }
			return  rtv;
		}
	//OBB--------------------------------------------------------------------------
		//生成
		OBB::SP  OBB::Create(const ML::Vec3&  p_, const ML::Vec3& s_, const ML::QT& a_)
		{
			shared_ptr<OBB>  ob = shared_ptr<OBB>(new  OBB());
			if (ob){
				if (ob->Initialize(p_, s_, a_)){
					return  ob;
				}
			}
			return nullptr;
		}
		//初期化
		bool  OBB::Initialize(const ML::Vec3&  p_, const ML::Vec3& s_, const ML::QT& a_)
		{
			this->pos = p_;
			this->size_h = s_;
			this->angle = a_;
			this->kind = Shape::eOBB;
			return  true;
		}
		//コピーを生成して返す
		Shape::SP   OBB::Clone()
		{
			return  OBB::Create(this->pos, this->size_h, this->angle);
		}
		//AABBとの接触
		bool OBB::Hit(const class AABB& b_, ExtParam&  ex_)
		{
			XNA::OrientedBox m = { pos, size_h, angle };
			XNA::AxisAlignedBox n = { b_.pos, b_.size_h };
			bool rtv = TRUE == XNA::IntersectAxisAlignedBoxOrientedBox(&n, &m);
			if (rtv){
				//排出方向のベクトルを生成する
				//★仮処理★
				//ex_.pushOut = b_.pos - this->pos;
				//ＯＢＢのもつ回転情報を使ってＯＢＢを中心としてAABBの中心点を逆回転させる
				Mat4x4  inv_mat, mat;
				mat.RotationQuaternion(this->angle);
				inv_mat = mat.Inverse();
				//相対座標を求める
				ML::Vec3  irBoxPos = inv_mat.TransformNormal(b_.pos - this->pos);
				//AABBからOBBを排出する方向を求める
				ex_.pushOut = CheckBox6(this->size_h, irBoxPos);
				//そのベクトルをＯＢＢの回転に合わせて回転させる
				ex_.pushOut = mat.TransformNormal(ex_.pushOut);
			}
			return rtv;
		}
		//球体との接触
		bool OBB::Hit(const class Sphere& b_, ExtParam&  ex_)
		{
			XNA::Sphere		 m = { b_.pos, b_.rad };
			XNA::OrientedBox n = { pos, size_h, angle };
			bool rtv = TRUE == XNA::IntersectSphereOrientedBox(&m, &n);
			if (rtv){
				//排出方向のベクトルを生成する
				//★仮処理★
				//ex_.pushOut = b_.pos - this->pos;
				//ＯＢＢのもつ回転情報を使ってＯＢＢを中心としてスフィアを逆回転させる
				Mat4x4  inv_mat, mat;
				mat.RotationQuaternion(this->angle);
				inv_mat = mat.Inverse();
				//相対座標を求める
				ML::Vec3  irSpherePos = inv_mat.TransformNormal(b_.pos - this->pos);
				//AABBからスフィアを排出する方向を求める
				ex_.pushOut = CheckBox6(this->size_h, irSpherePos);
				//そのベクトルをＯＢＢの回転に合わせて回転させる
				ex_.pushOut = mat.TransformNormal(ex_.pushOut);
			}
			return rtv;
		}
		//OBBとの接触
		bool OBB::Hit(const class OBB& b_, ExtParam&  ex_)
		{
			XNA::OrientedBox m = { pos, size_h, angle };
			XNA::OrientedBox n = { b_.pos, b_.size_h, b_.angle };
			bool rtv = TRUE == XNA::IntersectOrientedBoxOrientedBox(&m, &n);
			if (rtv){
				//排出方向のベクトルを生成する
				//★仮処理★
				//ex_.pushOut = b_.pos - this->pos;
				//ＯＢＢ(this)のもつ回転情報を使ってＯＢＢ(this)を中心としてＯＢＢ(b_)を逆回転させる
				Mat4x4  inv_mat, mat;
				mat.RotationQuaternion(this->angle);
				inv_mat = mat.Inverse();
				ML::Vec3  irSpherePos = inv_mat.TransformNormal(b_.pos - this->pos);
				//AABBからＯＢＢ(b_)を排出する方向を求める
				ex_.pushOut = CheckBox6(this->size_h, irSpherePos);
				//そのベクトルをＯＢＢ(this)の回転に合わせて回転させる
				ex_.pushOut = mat.TransformNormal(ex_.pushOut);
			}
			return rtv;
		}
		//Rayとの接触
		bool OBB::HitRay(const ML::Vec3&  p_,
			const ML::Vec3&  d_,
			float&  dist_)
		{
			float  length = d_.Length();
			const  FXMVECTOR  p = XMLoadFloat3(&XMFLOAT3(p_));
			const  FXMVECTOR  d = XMLoadFloat3(&XMFLOAT3(d_ / length));
			XNA::OrientedBox n = { pos, size_h, angle };
			bool  rtv = TRUE == XNA::IntersectRayOrientedBox(p, d, &n, &dist_);
			//レイの長さに合わせて距離（割合）を調整
			if (rtv){ dist_ /= length; }
			return  rtv;
		}
	//複合図形クラス----------------------------------------------------
		bool ShapeContainer::Initialize()
		{
			this->pos = ML::Vec3(0, 0, 0);
			this->angle = ML::QT(0);
			return  true;
		}
		//複合図形クラスを生成する（ただし、中身が無い）
		ShapeContainer::SP  ShapeContainer::Create()
		{
			shared_ptr<ShapeContainer>  ob = shared_ptr<ShapeContainer>(new  ShapeContainer());
			if (ob){
				if (ob->Initialize()){
					return  ob;
				}
			}
			return nullptr;
		}
		//コピーを生成して返す
		ShapeContainer::SP   ShapeContainer::Clone()
		{
			shared_ptr<ShapeContainer>  ob = shared_ptr<ShapeContainer>(new  ShapeContainer());
			ob->pos = this->pos;
			ob->angle = this->angle;
			//子のクローン
			for (auto  it = this->child.begin();
				it != this->child.end();
				it++){
				ob->child.push_back((*it)->Clone());
			}
			return  ob;
		}
		//移動させる
		void  ShapeContainer::Offset(const  ML::Vec3  ofs_)
		{
			this->pos += ofs_;
			//子を移動
			for (auto  it = this->child.begin();
				it != this->child.end();
				it++){
				(*it)->Offset(ofs_);
			}
		}
		//回転させる
		void  ShapeContainer::Rotation(const  ML::QT  rot_)
		{
			this->angle *= rot_;
			ML::Mat4x4  m;
			m.RotationQuaternion(rot_);
			//子を回転により移動させる
			for (auto  it = this->child.begin();
				it != this->child.end();
				it++){
				//親の座標を軸に子の座標を回転させたものを再設定する
				ML::Vec3  nPos = (*it)->pos - this->pos;
				nPos = m.TransformNormal(nPos);
				(*it)->pos = nPos + this->pos;
				(*it)->angle *= rot_;
			}
		}
		//接触判定(単一図形）
		bool ShapeContainer::Hit(const Shape::SP  b_, struct  Shape::ExtParam&  ep_)
		{
			//自身の子全てと判定させる
			for (auto itA = this->child.begin(); itA != this->child.end(); itA++){
				if ((*itA)->Hit(b_, ep_)){
					return true;
				}
			}
			return false;
		}
		//接触判定（コンテナ同士）
		bool ShapeContainer::Hit(const ShapeContainer::SP  b_, struct  Shape::ExtParam&  ep_)
		{
			//自身の子全てと判定させる
			for (auto itA = this->child.begin(); itA != this->child.end(); itA++){
				//Bの子全てと判定させる
				for (auto itB = b_->child.begin(); itB != b_->child.end(); itB++){
					if ((*itA)->Hit((*itB), ep_)){
						return true;
					}
				}
			}
			return false;
		}

		//Rayとの接触
		bool ShapeContainer::HitRay(const ML::Vec3&  p_,
			const ML::Vec3&  d_,
			float&  dist_)
		{
			//子と判定させる(こちらはすべてを対象にする）
			bool  flag = false;
			for (auto  it = this->child.begin();
				it != this->child.end();
				it++){
				float  dist;
				if ((*it)->HitRay(p_, d_, dist))
				{
					if (flag == false){
						flag = true;
						dist_ = dist;
					}
					else if (dist_ > dist){
						dist_ = dist;	//もっとも近い物を探す
					}
				}
			}
			return flag;
		}
	//複合図形クラス(カプセル型)----------------------------------------------------
		//薬のカプセル型の図形を設定する(全長と球の半径を指定する）
		Capsule::SP  Capsule::Create(const ML::Vec3&  p_, float  tb_, float  r_)
		{
			shared_ptr<Capsule>  ob = shared_ptr<Capsule>(new  Capsule());
			if (ob){
				if (ob->Initialize(p_, tb_, r_)){
					return  ob;
				}
			}
			return nullptr;
		}
		bool  Capsule::Initialize(const ML::Vec3&  p_, float  tb_, float  r_)
		{
			//スフィアの数は、直径を基準として生成する、カプセルの全長が120cm、スフィアの直径が40cm
			//である場合、カプセル数は３個になる。（130cmなら４個、110cmなら３個）
			int scnt = 0;
			for (float len = tb_; len > 0; len -= r_ * 2){ scnt++; }
			//１個当たりの間隔を求める
			float  cut = 0;
			if (scnt >= 2){
				cut = (tb_ - (r_ * 2)) / (scnt - 1);
			}
			//下端のスフィアから生成していく
			ML::Vec3  spos(0, 0, 0);
			spos.y -= (tb_ * 0.5f) - r_;
			for (int c = 0; c < scnt; c++){
				this->child.push_back(Sphere::Create(spos, r_));
				spos.y += cut;
			}
			//最後に位置調整
			this->Offset(p_);
			return  true;
		}
	}
}