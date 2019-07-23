#pragma warning(disable:4996)
#pragma once
//-----------------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-----------------------------------------------------------------------------
#include "GameEngine_Ver3_81.h"

class BChara : public BTask
{
	//�ύX�s����������������������������������������������������
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//�ύX������������������������������������������������������
	//�L�����N�^���ʃ����o�ϐ�
	ML::Vec2    pos;		//�L�����N�^�ʒu
	ML::Box2D   hitBase;	//�����蔻��͈�
	ML::Vec2	moveVec;	//�ړ��x�N�g��
	int			moveCnt;	//�s���J�E���^
							//���E�̌����i2D�����_�Q�[����p�j
	enum Angle_LR { Left, Right };
	Angle_LR	angle_LR;
	WP			target;
	//�L�����N�^�̍s����ԃt���O
	enum Motion
	{
		Unnon = -1,	//	����(�g���܂���j
		Stand,		//	��~
		Walk,		//	���s
		Attack,		//	�U��
		WAA,		//  �����Ȃ���U��
		Jump,		//	�W�����v
		Fall,		//	����
		Sit,		//	����
		Landing,	//	���n
		Turn,		//�@�����ύX
		Lose,		//  ������E���V
		Dead,		//  ����
		SitAttack	//  ����Ȃ���U��
	};
	Motion			motion;			//	���݂̍s���������t���O
	int				animCnt;		//�A�j���[�V�����J�E���^
	float			jumpPow;		//	�W�����v����
	float			maxFallSpeed;	//	�����ő呬�x
	float			gravity;		//	�t���[���P�ʂ̉��Z��
	float			maxSpeed;		//	���E�����ւ̈ړ��̉��Z��
	float			addSpeed;		//	���E�����ւ̈ړ��̉��Z��
	float			decSpeed;		//	���E�����ւ̈ړ��̌�����
	int				hp;				//�q�b�g�|�C���g
	int				unHitTime;		//���G����





	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����鎖����
	BChara()
		: pos(0, 0)
		, hitBase(0, 0, 0, 0)
		, moveVec(0, 0)
		, moveCnt(0)
		, angle_LR(Right)
		, motion(Stand)
		, jumpPow(0)
		, maxFallSpeed(0)
		, gravity(0)
		, maxSpeed(0)
		, addSpeed(0)
		, decSpeed(0)
		, hp(1)
		, unHitTime(0)
	{
	}
	virtual  ~BChara() {}

	//�L�����N�^���ʃ��\�b�h
	//�߂荞�܂Ȃ��ړ�����
	virtual  void  CheckMove(ML::Vec2&  est_);
	//�����ڐG����
	virtual  bool  CheckFoot();
	//����ڐG����
	virtual  bool  CheckHead();
	//���ʐڐG����i�T�C�h�r���[�Q�[����p�j
	virtual  bool  CheckFront_LR();
	//���[�V�������X�V�i�ύX�Ȃ��̏ꍇ	false)
	bool  UpdateMotion(Motion  nm_);
	

	//	�A�j���[�V�������\����
	struct DrawInfo {
		ML::Box2D		draw, src;
		ML::Color		color;
	};
	//�U�����
	struct AttackInfo {
		int power;		//�U���̈З�
		int hit;		//�������x
		int element;	//�U���̑���
		//���̑��K�v�ɉ�����
	};
	//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�Ȃ��́j
	virtual  void  Received(BChara*  from_, AttackInfo  at_);
	//�ڐG����
	virtual  bool  CheckHit(const  ML::Box2D& hit_);
};
