#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "config/Config.h"
#include "ui/CocosGUI.h"

// 表示する縦横のコマ数
#define MAX_BLOCK_X 8
#define MAX_BLOCK_Y 8

// コマを消すときのアクション時間
#define REMOVING_TIME 0.1f

// コマが移動するときのアクション時間
#define MOVING_TIME 0.2f

// ハイスコア保存用キー
#define KEY_HIGHSCORE "HighScore"

// リソース
#define PNG_BACKGROUND "img/background.png"
#define PNG_GAMEOVER "img/gameover.png"
#define PNG_RESET1 "img/reset1.png"
#define PNG_RESET2 "img/reset2.png"
#define MP3_REMOVE_BLOCK "sound/removeBlock.mp3"

#define FONT_RED "fonts/redFont.fnt"
#define FONT_BLUE "fonts/blueFont.fnt"
#define FONT_YELLOW "fonts/yellowFont.fnt"
#define FONT_GREEN "fonts/greenFont.fnt"
#define FONT_GRAY "fonts/grayFont.fnt"
#define FONT_WHITE "fonts/whiteFont.fnt"

class GameScene : public cocos2d::Scene
{
protected:

	enum kTag
	{
		kTagBackground = 1,	// 背景
		kTagRedLabel,
		kTagBlueLabel,
		kTagYellowLabel,
		kTagGreenLabel,
		kTagGrayLabel,
		kTagScoreLabel,
		kTagGameOver,
		kTagHighScoreLabel,
		kTagBaseBlock = 10000, // コマ
	};

	enum kZOrder
	{
		kZOrderBackground,
		kZOrderLabel,
		kZOrderBlock,
		kZOrderGameOver,

	};

	struct PositionIndex
	{
		PositionIndex(int x1, int y1)
		{
			x = x1;
			y = y1;
		}

		int x;
		int y;
	};

	// 背景表示
	cocos2d::Sprite* m_background;
	void showBackground();
	cocos2d::Size m_bgSize;

	// コマ表示
	float m_blockSize;// コマ画像の高さをコマのサイズとするので保存する変数
	std::map<kBlock, std::list<int>> m_blockTags;	// k=コマ種類、v=コマの固有タグ
	void initForVariables();
	void showBlock();
	cocos2d::Point getPosition(int posIndexX, int posIndexY);
	int getTag(int posIndexX, int posIndexY);

	// コマの消去
	cocos2d::EventListenerTouchOneByOne* m_listener;
	void getTouchBlockTag(cocos2d::Point touchPoint, int &tag, kBlock &blockType);
	std::list<int> getSameColorBlockTags(int baseTag, kBlock blockType);
	void removeBlock(std::list<int> blockTags, kBlock blockType);
	bool hasSameColorBlock(std::list<int> blockTagList, int searchBlockTag);
	void removingBlock(cocos2d::Node* block);

	// コマの移動
	std::vector<kBlock> m_blockTypes; // コマ種類に対してループ処理があるので配列を用意する
	PositionIndex getPositionIndex(int tag);
	void setNewPosition1(int tag, PositionIndex posIndex);
	void searchNewPosition1(std::list<int> blocks);
	void moveBlock();
	void movingBlocksAnimation1(std::list<int> blocks);

	bool m_animating;// アニメーション中にタップを受付ないようにするフラグ
	void movedBlocks(float dt);
	std::map<int, bool> getExistBlockColumn();
	void searchNewPosition2();
	void setNewPosition2(int tag, PositionIndex posIndex);
	void movingBlocksAnimation2(float dt);

	// ラベル表示
	void showLabel();

	// スコア
	int m_score;

	// ゲーム終了
	bool existsSameBlock();

	// ハイスコア
	void saveHighScore();
	void showHighScoreLabel();

	// リセット
	void menuResetCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);
	void showResetButton();

public:
		virtual bool init();
		// CCSceneという名前は非推奨になったのでSceneに変えた
		static cocos2d::Scene* scene();
		CREATE_FUNC(GameScene);

		virtual bool onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent);
		virtual void onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent);

};

#endif //__GAMESCENE_H__
