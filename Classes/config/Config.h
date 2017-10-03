#ifndef __CONFIG_H__
#define __CONFIG_H__

// コマの種類
enum kBlock
{
	kBlock_Red,
	kBlock_Blue,
	kBlock_Yellow,
	kBlock_Green,
	kBlock_Gray,

	kBlock_Count,
};

#define ccsf(...) String::createWithFormat(__VA_ARGS__)->getCString()

#endif // __CONFIG_H__
