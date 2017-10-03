#include "CCPlaySE.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;


// ※とりあえず「にゃんがめ」のプロジェクトに作ったけど、これcocos2dxのプロジェクトに入れたほうがいいかも
CCPlaySE::CCPlaySE(std::string sound)
{
	// メンバ変数にサウンド名を保持
	m_sound = sound;
}

CCPlaySE* CCPlaySE::create(std::string sound)
{
	// 自分を生成
	CCPlaySE* pRet = new CCPlaySE(sound);
	if( pRet )
	{
		// 必要がなくなったら自動的にメモリが解放されるように設定
		pRet->autorelease();
	}

	return pRet;
}

// アクション時に呼ばれる
void CCPlaySE::update(float time)
{
	// 引数で渡されたtimeの時間は使用しないのでCC_UNUSED_PARAMで私は使いませんと宣言する
	CC_UNUSED_PARAM(time);

	// 効果音を再生する
	SimpleAudioEngine::getInstance()->playEffect(m_sound.c_str());

}

// reverseは一般的にはupdateの逆の処理を実装するが今回は音を再生するだけなので同じ効果音が再生するようにしている
FiniteTimeAction* CCPlaySE::reverse()
{
	return  (FiniteTimeAction*)(CCPlaySE::create(m_sound));
}
