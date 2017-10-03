#include "scene/GameScene.h"

#include "SimpleAudioEngine.h"
#include "BlockSprite.h"
#include "sound/CCPlaySE.h"

USING_NS_CC;
using namespace CocosDenshion;
using namespace std;
using namespace ui;

Scene* GameScene::scene()
{
	Scene* scene = Scene::create();
	GameScene* layer = GameScene::create();
	scene->addChild(layer);

	return scene;
}

// 初期化
bool GameScene::init()
{
	if( !cocos2d::Scene::init() )
	{
		return false;
	}

	// タップイベントを取得する
	m_listener = EventListenerTouchOneByOne::create();
	m_listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	// ラムダ式でもOK
	/*m_listener->onTouchBegan = [](cocos2d::Touch* pTouch, cocos2d::Event* pEvent){
		log("test");
		return true;
	};*/
	m_listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_listener, this);// イベントリスナーを登録

	// 変数初期化
	initForVariables();

	// 背景表示
	showBackground();

	// コマ表示
	showBlock();

	// ラベル作成
	showLabel();
	showHighScoreLabel();

	// リセットボタン作成
	showResetButton();

	// 効果音の事前読み込み(初回読み込みをさせないと最初の音がずれる）
	SimpleAudioEngine::getInstance()->preloadEffect(MP3_REMOVE_BLOCK);

	return true;
}

// 背景表示
void GameScene::showBackground()
{
	//Size winSize = Director::sharedDirector()->getWinSize();
	Size winSize = Director::getInstance()->getWinSize();

	// 背景を生成
	m_background = Sprite::create(PNG_BACKGROUND);
	m_background->setPosition( Vec2(winSize.width / 2.0f, winSize.height / 2.0f) );
	addChild(m_background, kZOrderBackground, kTagBackground);

	m_bgSize = m_background->getContentSize();
}

// 変数初期化
void GameScene::initForVariables()
{
	// 乱数初期化
	// srand関数は rand関数で発生させる擬似乱数の発生系列を変更します。
	srand( (unsigned)time(NULL) );

	// コマのインスタンスを作って画像の高さを取得し一辺の長さとする
	BlockSprite* pBlock = BlockSprite::createWithBlockType(kBlock_Red);
	m_blockSize = pBlock->getContentSize().height;

	// コマ種類の配列生成
	m_blockTypes.push_back(kBlock_Red);
	m_blockTypes.push_back(kBlock_Blue);
	m_blockTypes.push_back(kBlock_Yellow);
	m_blockTypes.push_back(kBlock_Green);
	m_blockTypes.push_back(kBlock_Gray);

	m_animating = false;

	m_score = 0;
}

// 位置取得
Point GameScene::getPosition(int posIndexX, int posIndexY)
{
	float offsetX = m_background->getContentSize().width * 0.168f;
	float offsetY = m_background->getContentSize().height * 0.029f;

	return Point( (posIndexX + 0.5) * m_blockSize + offsetX, (posIndexY + 0.5) * m_blockSize + offsetY );
}

// コマのタグ取得
int GameScene::getTag(int posIndexX, int posIndexY)
{
	return (kTagBaseBlock + posIndexX * 100 + posIndexY);
}

void GameScene::showBlock()
{
	// 8×8のコマを作成する
	for(int x=0; x < MAX_BLOCK_X; ++x)
	{
		for(int y=0; y < MAX_BLOCK_Y; ++y)
		{
			// ランダムでコマを作成
			kBlock blockType = (kBlock)(rand() % kBlock_Count);

			// 対応するコマ配列にタグを追加
			int tag = getTag(x, y);
			m_blockTags[blockType].push_back(tag);

			// コマを作成
			BlockSprite* pBlock = BlockSprite::createWithBlockType(blockType);
			pBlock->setPosition( getPosition(x, y) );
			m_background->addChild(pBlock, kZOrderBlock, tag);
		}
	}
}

// タップ開始
bool GameScene::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	// アニメーション中はタップ処理を受け付けない
	return !m_animating;
}

// タップ終了
void GameScene::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	// タップポイント取得
	Point touchPoint = m_background->convertTouchToNodeSpace(pTouch);

	// タップしたコマのTagとコマの種類を取得
	int tag = 0;
	kBlock blockType;
	getTouchBlockTag(touchPoint, tag, blockType);

	if( tag == 0 )
	{
		// タップしたところにコマが無かったので終了
		return;
	}

	// 隣接する同種類のコマを取得
	list<int> sameColorBlockTags = getSameColorBlockTags(tag, blockType);

	if( sameColorBlockTags.size() <= 1 )
	{
		// 2個以上つながってなかったので終了
		return;
	}

	// 得点加算：（消したコマ数 - 2) の2乗
	m_score += pow( (sameColorBlockTags.size() - 1), 2);

	// アニメーション中の状態にする
	m_animating = true;

	// コマを削除する
	removeBlock( sameColorBlockTags, blockType );

	// コマ削除後のアニメーション
	movingBlocksAnimation1(sameColorBlockTags);
}

// タップされたコマのタグを取得
void GameScene::getTouchBlockTag(cocos2d::Point touchPoint, int &tag, kBlock &blockType)
{
	// 全コマを走査
	for(int x=0; x < MAX_BLOCK_X; x++)
	{
		for(int y=0; y < MAX_BLOCK_Y; y++)
		{
			// コマのインスタンスを取得（コマが消えている場合はnullが返ってくるので注意）
			int currentTag = getTag(x, y);
			Node* node = m_background->getChildByTag(currentTag);

			// タップしたところがコマの位置に該当するかチェック
			if( node && node->getBoundingBox().containsPoint(touchPoint) )
			{
				// 該当したコマのタグと種別を返す
				tag = currentTag;
				blockType = ((BlockSprite*)node)->getBlockType();

				return;
			}
		}
	}
}

// コマ配列にあるか検索
bool GameScene::hasSameColorBlock(list<int> blockTagList, int searchBlockTag)
{
	list<int>::iterator it;
	for( it = blockTagList.begin(); it != blockTagList.end(); ++it)
	{
		if( *it == searchBlockTag )
		{
			return true;
		}
	}

	return false;
}

// タップされたコマと同色でかつ接しているコマの配列を返す
list<int> GameScene::getSameColorBlockTags(int baseTag, kBlock blockType)
{
	// 同色のコマを格納するリストの初期化
	list<int> sameColorBlockTags;
	sameColorBlockTags.push_back(baseTag);


	list<int>::iterator it = sameColorBlockTags.begin();
	// ※sameColorBlockTagsはwhileの中で増えていく
	while( it != sameColorBlockTags.end() )
	{
		int tags[] = {
				*it + 100, // タップされたコマの右のコマのタグ
				*it - 100, // タップされたコマの左のコマのタグ
				*it + 1, // タップされたコマの上のコマのタグ
				*it - 1, // タップされたコマの下のコマのタグ
		};

		// タップされたコマの上下左右のコマを確認する
		int tagsCount = sizeof(tags) / sizeof(tags[0]);
		for( int i = 0; i < tagsCount; i++ )
		{
			// すでにリストに追加済みか検索
			if( hasSameColorBlock(sameColorBlockTags, tags[i]) )
			{
				// あるなら次を検索
				continue;
			}

			// 隣のコマが同じ種類のコマか検索
			if( hasSameColorBlock(m_blockTags[blockType], tags[i]) )
			{
				// 同じ種類のコマならリストに追加
				sameColorBlockTags.push_back(tags[i]);
			}
		}

		// 隣のコマがあったらさらにその隣のコマと隣り合っているコマの検索を行っていく
		++it;
	}

	return sameColorBlockTags;
}

// リストのコマを削除
void GameScene::removeBlock(list<int> blockTags, kBlock blockType)
{
	bool first = true;

	list<int>::iterator it = blockTags.begin();
	while( it != blockTags.end() )
	{
		// 既存タグ配列から該当コマタグを削除
		m_blockTags[blockType].remove(*it);

		// 親の背景から対象となるコマのインスタンスを取得
		Node* block = m_background->getChildByTag(*it);
		if( block )
		{
			// コマが消えるアニメーションを生成(画像の大きさを0.1秒間に0に縮めることにより再現している）
			ScaleTo* scale = ScaleTo::create(REMOVING_TIME, 0);

			// コマを削除する関数を呼び出すアクションを生成
			CallFuncN* func = CallFuncN::create(CC_CALLBACK_1(GameScene::removingBlock, this));

			// コマが消えるアクション実行したあと、削除するアクションを順番に処理するアクションを作る（引数のNULLは最後をあらわすので必ず必要）
			FiniteTimeAction* sequence = Sequence::create(scale, func, NULL);

			FiniteTimeAction* action;
			// コマが消える音が重複して再生されないように最初の一回だけアクションを生成する
			if( first )
			{
				// コマが消えるサウンドアクションを生成
				CCPlaySE* playSe = CCPlaySE::create(MP3_REMOVE_BLOCK);

				// アクションをつなげる（sequenceアクションと消える音は同時に出したいのでSpawnクラスを使う）
				action = Spawn::create(sequence, playSe, NULL);

				first = false;
			}
			else
			{
				action = sequence;
			}

			// アクションをセットする
			block->runAction(action);

		}

		it++;
	}

	// ブロックを消す効果音再生
	SimpleAudioEngine::getInstance()->playEffect(MP3_REMOVE_BLOCK);
}

// コマの削除
void GameScene::removingBlock(Node* block)
{
	block->removeFromParentAndCleanup(true);
}

// タグからコマの位置インデックスを取得
GameScene::PositionIndex GameScene::getPositionIndex(int tag)
{
	int pos1_x = (tag - kTagBaseBlock) / 100;
	int pos1_y = (tag - kTagBaseBlock) % 100;

	return PositionIndex(pos1_x, pos1_y);
}

// 新しい位置を設定(第二引数の位置は、コマの現在の位置）
void GameScene::setNewPosition1(int tag, PositionIndex posIndex)
{
	BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
	// 設定されている次のY位置を取得
	int nextPosY = blockSprite->getNextPosY();
	if( nextPosY == -1 )
	{
		// まだ設定されてなければ、現在の位置を設定
		// 設定されているなら、それを使う
		nextPosY = posIndex.y;
	}

	// 移動先の位置をセット
	blockSprite->setNextPos(posIndex.x, --nextPosY);
}

// 消えたコマを埋めるように新しい位置をセット
void GameScene::searchNewPosition1(std::list<int> blocks)
{
	// 消えるコマ数分のループ
	list<int>::iterator it1 = blocks.begin();
	while( it1 != blocks.end() )
	{
		// 消えるコマの位置インデックスを取得
		int deleteBlockTag = *it1;
		PositionIndex deletePosIndex = getPositionIndex(deleteBlockTag);

		// コマ種類のループ
		vector<kBlock>::iterator it2 = m_blockTypes.begin();
		while( it2 != m_blockTypes.end() )
		{
			kBlock blockType = *it2;

			// 画面に存在する各種類のコマ数分ループ
			list<int>::iterator it3 = m_blockTags[blockType].begin();
			while( it3 != m_blockTags[blockType].end() )
			{
				int blockTag = *it3;
				PositionIndex posIndex = getPositionIndex(blockTag);

				if( (deletePosIndex.x == posIndex.x) && (deletePosIndex.y < posIndex.y) )
				{
					// 消えるコマの上に位置するコマに対して移動先の位置をセットする
					setNewPosition1(blockTag, posIndex);
				}

				it3++;
			}

			it2++;
		}

		it1++;
	}
}

// コマを移動する
void GameScene::moveBlock()
{
	// コマ種類のループ
	vector<kBlock>::iterator it1 = m_blockTypes.begin();
	while( it1 != m_blockTypes.end() )
	{
		kBlock blockType = *it1;

		// 各種類のコマ数分のループ
		list<int>::iterator it2 = m_blockTags[blockType].begin();
		while( it2 !=  m_blockTags[blockType].end() )
		{
			int tag = *it2;
			BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);

			// 次の移動場所を取得
			int nextPosX = blockSprite->getNextPosX();
			int nextPosY = blockSprite->getNextPosY();

			if( (nextPosX != -1) || (nextPosY != -1) )
			{
				// 新しいタグをセットする
				int newTag = getTag(nextPosX, nextPosY);
				blockSprite->initNextPos();
				blockSprite->setTag(newTag);

				// タグ一覧の値も更新する
				*it2 = newTag;

				// アニメーションをセットする
				MoveTo* move = MoveTo::create(MOVING_TIME, getPosition(nextPosX, nextPosY));
				blockSprite->runAction(move);
			}

			it2++;
		}

		it1++;
	}
}

// コマ削除後のコマが落ちるアニメーション
void GameScene::movingBlocksAnimation1(std::list<int> blocks)
{
	// コマの新しい位置をセットする
	searchNewPosition1(blocks);

	// 新しい位置がセットされたコマのアニメーション
	moveBlock();

	// 落ちるアニメーションが終わったら、列が消えた時の列移動アニメーションを実行
	scheduleOnce( schedule_selector(GameScene::movingBlocksAnimation2), MOVING_TIME );
}

// コマの移動完了
void GameScene::movedBlocks(float dt)
{
	// ラベル再表示
	showLabel();

	// アニメーション終了
	m_animating = false;

	// ゲーム終了のチェック
	if( !existsSameBlock() )
	{
		// ハイスコアの保存と表示更新
		saveHighScore();

		// ゲーム終了表示
		Sprite* gameOver = Sprite::create(PNG_GAMEOVER);
		gameOver->setPosition( Vec2(m_bgSize.width / 2.0f, m_bgSize.height * 0.8f) );
		m_background->addChild(gameOver, kZOrderGameOver, kTagGameOver);

		// ゲームオーバなのでタップを受け付けないようにする
		m_listener->setEnabled(false);
	}
}

// 新しい位置をセット
void GameScene::setNewPosition2(int tag, PositionIndex posIndex)
{
	// この関数では列をずらす

	BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
	// 設定されている次のX位置を取得
	int nextPosX = blockSprite->getNextPosX();
	if( nextPosX == -1 )
	{
		// まだ設定されてなければ、現在の位置を設定
		// 設定されているなら、それを使う
		nextPosX = posIndex.x;
	}

	// 移動先の位置をセット
	blockSprite->setNextPos(--nextPosX, posIndex.y);
}

// 存在する列を取得する
std::map<int, bool> GameScene::getExistBlockColumn()
{
	// 検索配列初期化
	map<int, bool> xBlock;
	for( int i = 0; i < MAX_BLOCK_X; i++ )
	{
		xBlock[i] = false;
	}

	// コマ種類のループ
	std::vector<kBlock>::iterator it1 = m_blockTypes.begin();
	while( it1 != m_blockTypes.end() )
	{
		kBlock blockType = *it1;

		list<int>::iterator it2 = m_blockTags[blockType].begin();
		while( it2 != m_blockTags[blockType].end() )
		{
			int tag = *it2;

			// 存在するコマのx位置を記録
			xBlock[getPositionIndex(tag).x] = true;

			it2++;
		}

		it1++;
	}

	return xBlock;
}

// 消えたコマ列を埋めるように新しい位置をセット
void GameScene::searchNewPosition2()
{
	// 存在する列を取得する
	std::map<int, bool> xBlock = getExistBlockColumn();

	// コマが存在する列の最初
	bool first = true;
	// コマが存在しないx位置を埋めるので、右端から見て行く
	// （一気に複数行消えても大丈夫なように）
	for( int i = MAX_BLOCK_X -1; i >= 0; i-- )
	{
		if( xBlock[i] )
		{
			// コマが存在する列を見つけた
			first = false;
			// コマが存在する列なので次へ
			continue;
		}

		// コマが存在しない

		if( first )
		{
			// まだコマがある列を見つけていない
			continue;
		}

		// すでにコマがある列を見つけているのでその列を左にずらす

		// 全コマを検索する
		std::vector<kBlock>::iterator it1 = m_blockTypes.begin();
		while( it1 != m_blockTypes.end() )
		{
			kBlock blockType = *it1;

			std::list<int>::iterator it2 = m_blockTags[blockType].begin();
			while( it2 != m_blockTags[blockType].end() )
			{
				int tag = *it2;

				// 現在の位置情報を取得
				PositionIndex posIndex = getPositionIndex(tag);

				if( i < posIndex.x )
				{
					// 消えた列の右側にあるすべてのコマを左にひとつずらす
					setNewPosition2(tag, posIndex);
				}

				it2++;
			}

			it1++;
		}
	}
}

// 列移動のアニメーション
void GameScene::movingBlocksAnimation2(float dt)
{
	// 列移動の位置を設定
	searchNewPosition2();

	// 列移動アニメーション
	moveBlock();

	// 移動時間が終わったらタップ抑制フラグを戻す
	scheduleOnce(schedule_selector(GameScene::movedBlocks), MOVING_TIME);
}

// ラベル表示
void GameScene::showLabel()
{
	// 残数表示
	int tagsForLabel[] = {
		kTagRedLabel,
		kTagBlueLabel,
		kTagYellowLabel,
		kTagGreenLabel,
		kTagGrayLabel
	};

	const char* fontNames[] = {
		FONT_RED,
		FONT_BLUE,
		FONT_YELLOW,
		FONT_GREEN,
		FONT_GRAY
	};

	float heightRate[] = {
			0.61f,
			0.51f,
			0.41f,
			0.31f,
			0.21f
	};

	// コマ種類のループ
	std::vector<kBlock>::iterator it1 = m_blockTypes.begin();
	while( it1 != m_blockTypes.end() )
	{
		kBlock blockType = *it1;

		// コマ残数表示
		int count = m_blockTags[blockType].size();
		const char* countStr = ccsf("%02d", count);

		Label* label = (Label*)m_background->getChildByTag(tagsForLabel[blockType]);
		if( !label )
		{
			// コマ残数生成
			label = Label::createWithBMFont(fontNames[blockType], countStr);
			label->setPosition(Vec2(m_bgSize.width * 0.78f, m_bgSize.height * heightRate[blockType]));
			m_background->addChild(label, kZOrderLabel, tagsForLabel[blockType]);
		}
		else
		{
			label->setString(countStr);
		}

		it1++;
	}

	// スコア表示
	const char* scoreStr = ccsf("%d", m_score);
	Label* scoreLabel = (Label*)m_background->getChildByTag(kTagScoreLabel);

	if( !scoreLabel )
	{
		// コマ残数生成
		scoreLabel = Label::createWithBMFont(FONT_WHITE, scoreStr);
		scoreLabel->setPosition( Vec2(m_bgSize.width * 0.78, m_bgSize.height * 0.75) );
		m_background->addChild(scoreLabel, kZOrderLabel, kTagScoreLabel);
	}
	else
	{
		scoreLabel->setString(scoreStr);
	}
}

// 全コマに対して隣り合うコマがあるかチェック
bool GameScene::existsSameBlock()
{
	std::vector<kBlock>::iterator it1 = m_blockTypes.begin();
	while( it1 != m_blockTypes.end() )
	{
		kBlock blockType = *it1;

		// 各種類のコマ数分ループ
		std::list<int>::iterator it2 = m_blockTags[blockType].begin();
		while( it2 != m_blockTags[blockType].end() )
		{
			int tag = *it2;

			if( getSameColorBlockTags(tag, blockType).size() > 1 )
			{
				// 隣り合う同じコマがある場合はtrueを返す
				return true;
			}

			it2++;
		}

		it1++;
	}

	// 隣り合うコマが見つからなかったのでfalseを返す
	return false;
}

// ハイスコアラベル表示
void GameScene::showHighScoreLabel()
{
	// ハイスコア表示
	int highScore = UserDefault::getInstance()->getIntegerForKey(KEY_HIGHSCORE, 0);

	const char* highScoreStr = ccsf("%d", highScore);
	Label* highScoreLabel = (Label*)m_background->getChildByTag(kTagHighScoreLabel);
	if( !highScoreLabel )
	{
		// ハイスコア生成
		highScoreLabel = Label::createWithBMFont(FONT_WHITE, highScoreStr);
		highScoreLabel->setPosition(Vec2(m_bgSize.width * 0.78f, m_bgSize.height * 0.87f));
		m_background->addChild(highScoreLabel, kZOrderLabel, kTagHighScoreLabel);
	}
	else
	{
		highScoreLabel->setString(highScoreStr);
	}
}

// ハイスコア記録・表示
void GameScene::saveHighScore()
{
	UserDefault* userDefault = UserDefault::getInstance();

	// ハイスコアを取得する
	int oldHighScore = userDefault->getIntegerForKey(KEY_HIGHSCORE, 0);
	if( oldHighScore < m_score )
	{
		// ハイスコアを保持する
		userDefault->setIntegerForKey(KEY_HIGHSCORE, m_score);
		userDefault->flush();

		// ハイスコアの表示を更新する
		showHighScoreLabel();
	}
}

// リセットボタンタップ時の処理
void GameScene::menuResetCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if( type != Widget::TouchEventType::ENDED )
	{
		return;
	}

	// 新しいシーンを作って置き換える
	/*auto scene = GameScene::create();

	TransitionFade* fade = TransitionFade::create(0.5f, scene, Color3B::WHITE);

	Director::getInstance()->replaceScene(fade);*/

	Director::getInstance()->replaceScene(GameScene::create());
}

// リセットボタンの作成
void GameScene::showResetButton()
{
	// リセットボタンの生成
	Button* resetButton = Button::create();
	resetButton->setTouchEnabled(true);
	//resetButton->loadTextures(PNG_RESET1, PNG_RESET2, "");
	//resetButton->loadTextures(PNG_RESET2, PNG_RESET1, "");
	resetButton->loadTextures(PNG_RESET2, PNG_GAMEOVER, "");
	resetButton->setPosition(Vec2(m_bgSize.width * 0.78f, m_bgSize.height * 0.1f));
	resetButton->addTouchEventListener(CC_CALLBACK_2(GameScene::menuResetCallback, this));
	m_background->addChild(resetButton);
}
