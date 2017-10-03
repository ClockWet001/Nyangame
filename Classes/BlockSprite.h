#ifndef __BLOCKSPRITE_H__
#define __BLOCKSPRITE_H__

#include "cocos2d.h"
#include "config/Config.h"

class BlockSprite : public cocos2d::Sprite
{
protected:
	const char* getBlockImageFileName(kBlock blockType);

public:
	// 変数とゲッターの宣言をするマクロ
	CC_SYNTHESIZE_READONLY(kBlock, m_blockType, BlockType);// 自身のコマ種別
	CC_SYNTHESIZE_READONLY(int, m_nextPosX, NextPosX);// 移動先の座標X
	CC_SYNTHESIZE_READONLY(int, m_nextPosY, NextPosY);// 移動先の座標Y

	BlockSprite();
	virtual ~BlockSprite();

	virtual bool initWithBlockType(kBlock blockType);
	static BlockSprite* createWithBlockType(kBlock blockType);

	void initNextPos();
	void setNextPos(int nextPoxX, int nextPosY);
};


#endif // __BLOCKSPRITE_H__
