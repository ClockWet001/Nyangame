#ifndef __CCPLAYSE_H__
#define __CCPLAYSE_H__

#include "cocos2d.h"

class CCPlaySE : public cocos2d::ActionInstant
{
public:
	CCPlaySE(std::string sound);
	virtual ~CCPlaySE(){}

	// update関数に処理を記述する
	virtual void update(float time);
	virtual cocos2d::FiniteTimeAction* reverse(void);

public:
	// 引数に効果音のファイル名を指定するcreate関数
	static CCPlaySE* create(std::string sound);

protected:
	// ファイル名を保持するメンバ変数
	std::string m_sound;

};

#endif // __CCPLAYSE_H__
