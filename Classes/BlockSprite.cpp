#include "BlockSprite.h"

BlockSprite::BlockSprite()
{
	initNextPos();
}

BlockSprite::~BlockSprite()
{

}

// コマのインスタンスを作って返す
BlockSprite* BlockSprite::createWithBlockType(kBlock blockType)
{
	// インスタンスと画像を生成
	BlockSprite *pRet = new BlockSprite();
	if( pRet && pRet->initWithBlockType(blockType) )
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		// インスタンス生成に失敗した
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

// コマ画像の初期化
bool BlockSprite::initWithBlockType(kBlock blockType)
{
	// 画像の読み込み
	if( !Sprite::initWithFile( getBlockImageFileName(blockType) ) )
	{
		// 読み込むべき画像が無くて失敗
		return false;
	}

	// 自分のコマ種別を覚えておく
	m_blockType = blockType;

	return true;
}

const char* BlockSprite::getBlockImageFileName(kBlock blockType)
{
	// コマ種別に対応したファイル名を返す
	switch(blockType)
	{
	case kBlock_Red:
		return "img/block/red.png";
	case kBlock_Blue:
		return "img/block/blue.png";
	case kBlock_Yellow:
		return "img/block/yellow.png";
	case kBlock_Green:
		return "img/block/green.png";
	case kBlock_Gray:
		return "img/block/gray.png";
	default:
		CCAssert(false, "invalid blockType");
		return "";
	}
}

// 移動先の初期化
void BlockSprite::initNextPos()
{
	m_nextPosX = -1;
	m_nextPosY = -1;
}

// 移動先のインデックスをセット
void BlockSprite::setNextPos(int nextPoxX, int nextPosY)
{
	m_nextPosX = nextPoxX;
	m_nextPosY = nextPosY;
}
