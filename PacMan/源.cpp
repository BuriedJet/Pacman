﻿/*
* Edited By Jet, Moriarty, weaZen
* 2016/05/01 19:55
* Naive AI Guzuta
*/

/*
* Pacman 样例程序
* 作者：zhouhy
* 时间：2016/3/22 15:32:51
* 最后更新：2016/4/22 16:18
* 【更新内容】
* 修复了文件不存在时不能通过控制台输入的Bug……
* 修改的部位：包含了fstream库、ReadInput的函数体中前几行发生了变化，不使用freopen了。
*
* 【命名惯例】
*  r/R/y/Y：Row，行，纵坐标
*  c/C/x/X：Column，列，横坐标
*  数组的下标都是[y][x]或[r][c]的顺序
*  玩家编号0123
*
* 【坐标系】
*   0 1 2 3 4 5 6 7 8
* 0 +----------------> x
* 1 |
* 2 |
* 3 |
* 4 |
* 5 |
* 6 |
* 7 |
* 8 |
*   v y
*
* 【提示】你可以使用
* #ifndef _BOTZONE_ONLINE
* 这样的预编译指令来区分在线评测和本地评测
*
* 【提示】一般的文本编辑器都会支持将代码块折叠起来
* 如果你觉得自带代码太过冗长，可以考虑将整个namespace折叠
*/

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <stack>
#include <stdexcept>
#include <vector>
#include <climits>
#include "jsoncpp/json.h"

#define FIELD_MAX_HEIGHT 20
#define FIELD_MAX_WIDTH 20
#define MAX_GENERATOR_COUNT 4 // 每个象限1
#define MAX_PLAYER_COUNT 4
#define MAX_TURN 100
#define TIME_LIMIT 0.975
#define QUEUE_MAX 121
#define MAX_INT 0x3fffffff
#define DEFAULT_DEPTH 7
#define MAX_DEPTH 16

//#define DEBUG

// 你也可以选用 using namespace std; 但是会污染命名空间
using std::cin;
using std::cout;
using std::endl;
using std::swap;
using std::string;
using std::getline;
using std::to_string;
using std::runtime_error;

string presetString;

//把枚举扩展收起来
namespace EnumExt
{
	// 让枚举也可以用这些运算了（不加会编译错误）
	template<typename T>
	inline T operator |=(T &a, const T &b)
	{
		return a = static_cast<T>(static_cast<int>(a) | static_cast<int>(b));
	}
	template<typename T>
	inline T operator |(const T &a, const T &b)
	{
		return static_cast<T>(static_cast<int>(a) | static_cast<int>(b));
	}
	template<typename T>
	inline T operator &=(T &a, const T &b)
	{
		return a = static_cast<T>(static_cast<int>(a) & static_cast<int>(b));
	}
	template<typename T>
	inline T operator &(const T &a, const T &b)
	{
		return static_cast<T>(static_cast<int>(a) & static_cast<int>(b));
	}
	template<typename T>
	inline T operator ++(T &a)
	{
		return a = static_cast<T>(static_cast<int>(a) + 1);
	}
	template<typename T>
	inline T operator ~(const T &a)
	{
		return static_cast<T>(~static_cast<int>(a));
	}
}

// 平台提供的吃豆人相关逻辑处理程序
namespace Pacman
{
	using namespace EnumExt;

	const time_t seed = time(0);
	const int dx[] = { 0, 1, 0, -1, 1, 1, -1, -1 }, dy[] = { -1, 0, 1, 0, -1, 1, 1, -1 };
	const string dirStr[] = { "stay" ,"up","right","down","left","ur","dr","dl","ul" };

	// 枚举定义；使用枚举虽然会浪费空间（sizeof(GridContentType) == 4），但是计算机处理32位的数字效率更高

	// 每个格子可能变化的内容，会采用“或”逻辑进行组合
	enum GridContentType
	{
		empty = 0, // 其实不会用到
		player1 = 1, // 1号玩家
		player2 = 2, // 2号玩家
		player3 = 4, // 3号玩家
		player4 = 8, // 4号玩家
		playerMask = 1 | 2 | 4 | 8, // 用于检查有没有玩家等
		smallFruit = 16, // 小豆子
		largeFruit = 32 // 大豆子
	};

	// 用玩家ID换取格子上玩家的二进制位
	GridContentType playerID2Mask[] = { player1, player2, player3, player4 };
	string playerID2str[] = { "0", "1", "2", "3" };

	// 每个格子固定的东西，会采用“或”逻辑进行组合
	enum GridStaticType
	{
		emptyWall = 0, // 其实不会用到
		wallNorth = 1, // 北墙（纵坐标减少的方向）
		wallEast = 2, // 东墙（横坐标增加的方向）
		wallSouth = 4, // 南墙（纵坐标增加的方向）
		wallWest = 8, // 西墙（横坐标减少的方向）
		generator = 16 // 豆子产生器
	};

	// 用移动方向换取这个方向上阻挡着的墙的二进制位
	GridStaticType direction2OpposingWall[] = { wallNorth, wallEast, wallSouth, wallWest };

	// 方向，可以代入dx、dy数组，同时也可以作为玩家的动作
	enum Direction
	{
		stay = -1,
		up = 0,
		right = 1,
		down = 2,
		left = 3,
		// 下面的这几个只是为了产生器程序方便，不会实际用到
		ur = 4, // 右上
		dr = 5, // 右下
		dl = 6, // 左下
		ul = 7 // 左上
	};

	// 场地上带有坐标的物件
	struct FieldProp
	{
		int row, col;
		FieldProp(int i = 0, int j = 0) : row(i), col(j) {}
		inline bool operator==(const FieldProp &a)
		{
			return (row == a.row && col == a.col);
		}
		inline bool operator!=(const FieldProp &a)
		{
			return (row != a.row || col != a.col);
		}
	};

	struct PathInfoType : FieldProp
	{
		bool isImpasse;
		bool isExit;
		int fleeLength;//到死路出口的距离
		PathInfoType * pExit;
		PathInfoType(int y = 0, int x = 0) : FieldProp(y, x), isImpasse(false), isExit(false), fleeLength(0), pExit(NULL) {}
	};

	// 场地上的玩家
	struct Player : FieldProp
	{
		int strength;
		int powerUpLeft;
		bool dead;
	};

	// 回合新产生的豆子的坐标
	struct NewFruits
	{
		FieldProp newFruits[MAX_GENERATOR_COUNT * 8];
		int newFruitCount;
	} newFruits[MAX_TURN];
	int newFruitsCount = 0;

	// 状态转移记录结构
	struct TurnStateTransfer
	{
		enum StatusChange // 可组合
		{
			none = 0,
			ateSmall = 1,
			ateLarge = 2,
			powerUpCancel = 4,
			die = 8,
			error = 16
		};

		// 玩家选定的动作
		Direction actions[MAX_PLAYER_COUNT];

		// 此回合该玩家的状态变化
		StatusChange change[MAX_PLAYER_COUNT];

		// 此回合该玩家的力量变化
		int strengthDelta[MAX_PLAYER_COUNT];
	};

	// 游戏主要逻辑处理类，包括输入输出、回合演算、状态转移，全局唯一
	class GameField
	{
	private:
		// 为了方便，大多数属性都不是private的

		// 记录每回合的变化（栈）
		TurnStateTransfer backtrack[MAX_TURN];

		// 这个对象是否已经创建
		static bool constructed;

	public:
		// 场地的长和宽
		int height, width;
		int generatorCount;
		int GENERATOR_INTERVAL, LARGE_FRUIT_DURATION, LARGE_FRUIT_ENHANCEMENT;

		//道路信息
		PathInfoType pathInfo[FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH];

		// 场地格子固定的内容
		GridStaticType fieldStatic[FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH];

		// 场地格子会变化的内容
		GridContentType fieldContent[FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH];
		int generatorTurnLeft; // 多少回合后产生豆子
		int aliveCount; // 有多少玩家存活
		int smallFruitCount;
		FieldProp generators[MAX_GENERATOR_COUNT]; // 有哪些豆子产生器
		Player players[MAX_PLAYER_COUNT]; // 有哪些玩家

		int turnID;
		// 玩家选定的动作
		Direction actions[MAX_PLAYER_COUNT];

		// weaZen：省得每次查一遍
		bool hasNext;

		// 恢复到上次场地状态。可以一路恢复到最开始。
		// 恢复失败（没有状态可恢复）返回false
		bool PopState()
		{
			if (turnID <= 0)
				return false;

			const TurnStateTransfer &bt = backtrack[--turnID];
			int i, _;

			// 倒着来恢复状态

			for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
			{
				Player &_p = players[_];
				GridContentType &content = fieldContent[_p.row][_p.col];
				TurnStateTransfer::StatusChange change = bt.change[_];

				if (!_p.dead)
				{
					// 5. 大豆回合恢复
					if (_p.powerUpLeft || change & TurnStateTransfer::powerUpCancel)
						_p.powerUpLeft++;

					// 4. 吐出豆子
					if (change & TurnStateTransfer::ateSmall)
					{
						content |= smallFruit;
						smallFruitCount++;
					}
					else if (change & TurnStateTransfer::ateLarge)
					{
						content |= largeFruit;
						_p.powerUpLeft -= LARGE_FRUIT_DURATION;
					}
				}

				// 2. 魂兮归来
				if (change & TurnStateTransfer::die)
				{
					_p.dead = false;
					aliveCount++;
					content |= playerID2Mask[_];
				}

				// 1. 移形换影
				if (!_p.dead && bt.actions[_] != stay)
				{
					fieldContent[_p.row][_p.col] &= ~playerID2Mask[_];
					_p.row = (_p.row - dy[bt.actions[_]] + height) % height;
					_p.col = (_p.col - dx[bt.actions[_]] + width) % width;
					fieldContent[_p.row][_p.col] |= playerID2Mask[_];
				}

				// 0. 救赎不合法的灵魂
				if (change & TurnStateTransfer::error)
				{
					_p.dead = false;
					aliveCount++;
					content |= playerID2Mask[_];
				}

				// *. 恢复力量
				if (!_p.dead)
					_p.strength -= bt.strengthDelta[_];
			}

			// 3. 收回豆子
			if (generatorTurnLeft == GENERATOR_INTERVAL)
			{
				generatorTurnLeft = 1;
				NewFruits &fruits = newFruits[--newFruitsCount];
				for (i = 0; i < fruits.newFruitCount; i++)
				{
					fieldContent[fruits.newFruits[i].row][fruits.newFruits[i].col] &= ~smallFruit;
					smallFruitCount--;
				}
			}
			else
				generatorTurnLeft++;

			hasNext = true;
			return true;
		}

		// Jet:把PopState包装了一下 方便一些
		void RollBack(int turnCount = -1)
		{
			if (turnCount < 0)
			{
				while (PopState());
				return;
			}

			for (int i = 0; i < turnCount; i++)
				if (!PopState())
					break;
		}

		// 判断指定玩家向指定方向移动是不是合法的（没有撞墙且没有踩到豆子产生器）
		inline bool ActionValid(int playerID, Direction &dir) const
		{
			if (dir == stay)
				return true;
			const Player &p = players[playerID];
			const GridStaticType &s = fieldStatic[p.row][p.col];
			return dir >= -1 && dir < 4 && !(s & direction2OpposingWall[dir]);
		}

		// 在向actions写入玩家动作后，演算下一回合局面，并记录之前所有的场地状态，可供日后恢复。
		// 是终局的话就返回false
		bool NextTurn()
		{
			int _, i, j;

			TurnStateTransfer &bt = backtrack[turnID];
			memset(&bt, 0, sizeof(bt));

			// 0. 杀死不合法输入
			for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
			{
				Player &p = players[_];
				if (!p.dead)
				{
					Direction &action = actions[_];
					if (action == stay)
						continue;

					if (!ActionValid(_, action))
					{
						bt.strengthDelta[_] += -p.strength;
						bt.change[_] = TurnStateTransfer::error;
						fieldContent[p.row][p.col] &= ~playerID2Mask[_];
						p.strength = 0;
						p.dead = true;
						aliveCount--;
					}
					else
					{
						// 遇到比自己强♂壮的玩家是不能前进的
						GridContentType target = fieldContent
							[(p.row + dy[action] + height) % height]
						[(p.col + dx[action] + width) % width];
						if (target & playerMask)
							for (i = 0; i < MAX_PLAYER_COUNT; i++)
								if (target & playerID2Mask[i] && players[i].strength > p.strength)
									action = stay;
					}
				}
			}

			// 1. 位置变化
			for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
			{
				Player &_p = players[_];
				if (_p.dead)
					continue;

				bt.actions[_] = actions[_];

				if (actions[_] == stay)
					continue;

				// 移动
				fieldContent[_p.row][_p.col] &= ~playerID2Mask[_];
				_p.row = (_p.row + dy[actions[_]] + height) % height;
				_p.col = (_p.col + dx[actions[_]] + width) % width;
				fieldContent[_p.row][_p.col] |= playerID2Mask[_];
			}

			// 2. 玩家互殴
			for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
			{
				Player &_p = players[_];
				if (_p.dead)
					continue;

				// 判断是否有玩家在一起
				int player, containedCount = 0;
				int containedPlayers[MAX_PLAYER_COUNT];
				for (player = 0; player < MAX_PLAYER_COUNT; player++)
					if (fieldContent[_p.row][_p.col] & playerID2Mask[player])
						containedPlayers[containedCount++] = player;

				if (containedCount > 1)
				{
					// NAIVE
					for (i = 0; i < containedCount; i++)
						for (j = 0; j < containedCount - i - 1; j++)
							if (players[containedPlayers[j]].strength < players[containedPlayers[j + 1]].strength)
								swap(containedPlayers[j], containedPlayers[j + 1]);

					int begin;
					for (begin = 1; begin < containedCount; begin++)
						if (players[containedPlayers[begin - 1]].strength > players[containedPlayers[begin]].strength)
							break;

					// 这些玩家将会被杀死
					int lootedStrength = 0;
					for (i = begin; i < containedCount; i++)
					{
						int id = containedPlayers[i];
						Player &p = players[id];

						// 从格子上移走
						fieldContent[p.row][p.col] &= ~playerID2Mask[id];
						p.dead = true;
						int drop = p.strength / 2;
						bt.strengthDelta[id] += -drop;
						bt.change[id] |= TurnStateTransfer::die;
						lootedStrength += drop;
						p.strength -= drop;
						aliveCount--;
					}

					// 分配给其他玩家
					int inc = lootedStrength / begin;
					for (i = 0; i < begin; i++)
					{
						int id = containedPlayers[i];
						Player &p = players[id];
						bt.strengthDelta[id] += inc;
						p.strength += inc;
					}
				}
			}

			// 3. 产生豆子
			if (--generatorTurnLeft == 0)
			{
				generatorTurnLeft = GENERATOR_INTERVAL;
				NewFruits &fruits = newFruits[newFruitsCount++];
				fruits.newFruitCount = 0;
				for (i = 0; i < generatorCount; i++)
					for (Direction d = up; d < 8; ++d)
					{
						// 取余，穿过场地边界
						int r = (generators[i].row + dy[d] + height) % height, c = (generators[i].col + dx[d] + width) % width;
						if (fieldStatic[r][c] & generator || fieldContent[r][c] & (smallFruit | largeFruit))
							continue;
						fieldContent[r][c] |= smallFruit;
						fruits.newFruits[fruits.newFruitCount].row = r;
						fruits.newFruits[fruits.newFruitCount++].col = c;
						smallFruitCount++;
					}
			}

			// 4. 吃掉豆子
			for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
			{
				Player &_p = players[_];
				if (_p.dead)
					continue;

				GridContentType &content = fieldContent[_p.row][_p.col];

				// 只有在格子上只有自己的时候才能吃掉豆子
				if (content & playerMask & ~playerID2Mask[_])
					continue;

				if (content & smallFruit)
				{
					content &= ~smallFruit;
					_p.strength++;
					bt.strengthDelta[_]++;
					smallFruitCount--;
					bt.change[_] |= TurnStateTransfer::ateSmall;
				}
				else if (content & largeFruit)
				{
					content &= ~largeFruit;
					if (_p.powerUpLeft == 0)
					{
						_p.strength += LARGE_FRUIT_ENHANCEMENT;
						bt.strengthDelta[_] += LARGE_FRUIT_ENHANCEMENT;
					}
					_p.powerUpLeft += LARGE_FRUIT_DURATION;
					bt.change[_] |= TurnStateTransfer::ateLarge;
				}
			}

			// 5. 大豆回合减少
			for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
			{
				Player &_p = players[_];
				if (_p.dead)
					continue;

				if (_p.powerUpLeft > 0 && --_p.powerUpLeft == 0)
				{
					_p.strength -= LARGE_FRUIT_ENHANCEMENT;
					bt.change[_] |= TurnStateTransfer::powerUpCancel;
					bt.strengthDelta[_] += -LARGE_FRUIT_ENHANCEMENT;
				}
			}

			++turnID;

			// 是否只剩一人？
			if (aliveCount <= 1)
			{
				for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
					if (!players[_].dead)
					{
						bt.strengthDelta[_] += smallFruitCount;
						players[_].strength += smallFruitCount;
					}
				return hasNext = false;
			}

			// 是否回合超限？
			if (turnID >= 100)
				return hasNext = false;

			return hasNext = true;
		}

		// 获取row, col位置的水果估值
		char GetFruitValue(int row, int col)const
		{
			char v = 0;
			if (fieldContent[row][col] & smallFruit)
				v += 1;
			if (fieldContent[row][col] & largeFruit)
				v += 6;
			return v;
		}

		//weaZen: 地图分析
		void MapAnalyze()
		{
			FieldProp deadSpot[40];
			int degree[FIELD_MAX_HEIGHT][FIELD_MAX_HEIGHT];
			int dCount = 0;
			PathInfoType * ptmpExit;
			ptmpExit = NULL;

			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					pathInfo[y][x].row = y;
					pathInfo[y][x].col = x;
					int degreeCount = 0;
					for (Pacman::Direction dir = Pacman::Direction::up; dir < 4; ++dir)
						if (!(fieldStatic[y][x] & Pacman::direction2OpposingWall[dir]))
							++degreeCount;
					degree[y][x] = degreeCount;
					if (degreeCount == 1)
					{
						pathInfo[y][x].isImpasse = true;
						deadSpot[dCount].row = y;
						deadSpot[dCount++].col = x;
					}
				}
			}
			for (int i = 0; i < dCount; ++i)
			{
				FieldProp startPos = deadSpot[i];
				FieldProp queue[QUEUE_MAX];
				queue[0] = startPos;
				int nowFlag = 0, endFlag = 0;
				while (nowFlag <= endFlag)
				{
					const Pacman::GridStaticType &curGrid = fieldStatic[queue[nowFlag].row][queue[nowFlag].col];
					for (Pacman::Direction dir = Pacman::Direction::up; dir < 4; ++dir)
					{
						if (!(curGrid & Pacman::direction2OpposingWall[dir]))
						{
							Pacman::FieldProp newPos = queue[nowFlag];
							newPos.row = (newPos.row + Pacman::dy[dir] + height) % height;
							newPos.col = (newPos.col + Pacman::dx[dir] + width) % width;
							--degree[newPos.row][newPos.col];
							if (degree[newPos.row][newPos.col] == 1 && !pathInfo[newPos.row][newPos.col].isImpasse)
							{
								pathInfo[newPos.row][newPos.col].isExit = false;
								pathInfo[newPos.row][newPos.col].isImpasse = true;
								queue[++endFlag] = newPos;
							}
							if (degree[newPos.row][newPos.col] > 1)
							{
								pathInfo[newPos.row][newPos.col].isExit = true;
								pathInfo[newPos.row][newPos.col].pExit = ptmpExit = &pathInfo[newPos.row][newPos.col];
								pathInfo[newPos.row][newPos.col].fleeLength = 0;
							}
						}
					}
					++nowFlag;
				}
				for (int i = 0; i <= endFlag; ++i)
				{
					pathInfo[queue[i].row][queue[i].col].fleeLength = endFlag - i + 1;
					pathInfo[queue[i].row][queue[i].col].pExit = ptmpExit;
				}

			}
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					if (pathInfo[y][x].isImpasse)
					{
						ptmpExit = pathInfo[y][x].pExit;
						while (ptmpExit != ptmpExit->pExit)
						{
							pathInfo[y][x].pExit = ptmpExit->pExit;
							pathInfo[y][x].fleeLength += ptmpExit->fleeLength;
							ptmpExit = pathInfo[y][x].pExit;
						}
					}
				}
			}
#ifdef DEBUG
			int tmp = 0;
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					if (pathInfo[y][x].isImpasse)
					{
						cout << ++tmp << ' ' << y << ' ' << x << ' ' << pathInfo[y][x].fleeLength << endl;
						//cout << pathInfo[y][x].pExit->row << ' ' << pathInfo[y][x].pExit->col << endl;
					}
				}
			}
			cout << "***********************" << endl;
			tmp = 0;
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					if (pathInfo[y][x].isExit)
						cout << ++tmp << ' ' << y << ' ' << x << ' ' << pathInfo[y][x].fleeLength << endl;
				}
			}

#endif // DEBUG
		}

		// 读取并解析程序输入，本地调试或提交平台使用都可以。
		// 如果在本地调试，程序会先试着读取参数中指定的文件作为输入文件，失败后再选择等待用户直接输入。
		// 本地调试时可以接受多行以便操作，Windows下可以用 Ctrl-Z 或一个【空行+回车】表示输入结束，但是在线评测只需接受单行即可。
		// localFileName 可以为NULL
		// obtainedData 会输出自己上回合存储供本回合使用的数据
		// obtainedGlobalData 会输出自己的 Bot 上以前存储的数据
		// 返回值是自己的 playerID
		int ReadInput(const char *localFileName, string &obtainedData, string &obtainedGlobalData)
		{
			string str, chunk;
			if (!presetString.empty())
				str = presetString;
			else
			{
#ifdef _BOTZONE_ONLINE
				std::ios::sync_with_stdio(false); //ω\\)
				getline(cin, str);
#else
				if (localFileName)
				{
					std::ifstream fin(localFileName);
					if (fin)
						while (getline(fin, chunk) && chunk != "")
							str += chunk;
					else
						while (getline(cin, chunk) && chunk != "")
							str += chunk;
				}
				else
					while (getline(cin, chunk) && chunk != "")
						str += chunk;
#endif
			}
			Json::Reader reader;
			Json::Value input;
			reader.parse(str, input);

			int len = input["requests"].size();

			// 读取场地静态状况
			Json::Value field = input["requests"][(Json::Value::UInt) 0],
				staticField = field["static"], // 墙面和产生器
				contentField = field["content"]; // 豆子和玩家
			height = field["height"].asInt();
			width = field["width"].asInt();
			LARGE_FRUIT_DURATION = field["LARGE_FRUIT_DURATION"].asInt();
			LARGE_FRUIT_ENHANCEMENT = field["LARGE_FRUIT_ENHANCEMENT"].asInt();
			generatorTurnLeft = GENERATOR_INTERVAL = field["GENERATOR_INTERVAL"].asInt();

			PrepareInitialField(staticField, contentField);

			MapAnalyze();

			// 根据历史恢复局面
			for (int i = 1; i < len; i++)
			{
				Json::Value req = input["requests"][i];
				for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
					if (!players[_].dead)
						actions[_] = (Direction)req[playerID2str[_]]["action"].asInt();
				NextTurn();
			}

			obtainedData = input["data"].asString();
			obtainedGlobalData = input["globaldata"].asString();

			return field["id"].asInt();
		}

		// 根据 static 和 content 数组准备场地的初始状况
		void PrepareInitialField(const Json::Value &staticField, const Json::Value &contentField)
		{
			int r, c, gid = 0;
			generatorCount = 0;
			aliveCount = 0;
			smallFruitCount = 0;
			generatorTurnLeft = GENERATOR_INTERVAL;
			for (r = 0; r < height; r++)
				for (c = 0; c < width; c++)
				{
					GridContentType &content = fieldContent[r][c] = (GridContentType)contentField[r][c].asInt();
					GridStaticType &s = fieldStatic[r][c] = (GridStaticType)staticField[r][c].asInt();
					if (s & generator)
					{
						generators[gid].row = r;
						generators[gid++].col = c;
						generatorCount++;
					}
					if (content & smallFruit)
						smallFruitCount++;
					for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
						if (content & playerID2Mask[_])
						{
							Player &p = players[_];
							p.col = c;
							p.row = r;
							p.powerUpLeft = 0;
							p.strength = 1;
							p.dead = false;
							aliveCount++;
						}
				}
		}

		// 完成决策，输出结果。
		// action 表示本回合的移动方向，stay 为不移动
		// tauntText 表示想要叫嚣的言语，可以是任意字符串，除了显示在屏幕上不会有任何作用，留空表示不叫嚣
		// data 表示自己想存储供下一回合使用的数据，留空表示删除
		// globalData 表示自己想存储供以后使用的数据（替换），这个数据可以跨对局使用，会一直绑定在这个 Bot 上，留空表示删除
		void WriteOutput(Direction action, string tauntText = "", string data = "",
			string globalData = "", string debugData = "") const
		{
			debugData += ' ' + to_string(seed);

			Json::Value ret;
			ret["response"]["action"] = action;
			ret["response"]["tauntText"] = tauntText;
			ret["data"] = data;
			ret["globaldata"] = globalData;
			ret["debug"] = debugData;

#ifdef _BOTZONE_ONLINE
			Json::FastWriter writer; // 在线评测的话能用就行……
#else
			Json::StyledWriter writer; // 本地调试这样好看 > <
#endif
			cout << writer.write(ret) << endl;
		}

		// 用于显示当前游戏状态，调试用。
		// 提交到平台后会被优化掉。
		inline void DebugPrint() const
		{
#ifndef _BOTZONE_ONLINE
			printf("回合号【%d】存活人数【%d】| 图例 产生器[G] 有玩家[0/1/2/3] 多个玩家[*] 大豆[o] 小豆[.]\n", turnID, aliveCount);
			for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
			{
				const Player &p = players[_];
				printf("[玩家%d(%d, %d)|力量%d|加成剩余回合%d|%s]\n",
					_, p.row, p.col, p.strength, p.powerUpLeft, p.dead ? "死亡" : "存活");
			}
			putchar(' ');
			putchar(' ');
			for (int c = 0; c < width; c++)
				printf("  %d ", c);
			putchar('\n');
			for (int r = 0; r < height; r++)
			{
				putchar(' ');
				putchar(' ');
				for (int c = 0; c < width; c++)
				{
					putchar(' ');
					printf((fieldStatic[r][c] & wallNorth) ? "---" : "   ");
				}
				printf("\n%d ", r);
				for (int c = 0; c < width; c++)
				{
					putchar((fieldStatic[r][c] & wallWest) ? '|' : ' ');
					putchar(' ');
					int hasPlayer = -1;
					for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
						if (fieldContent[r][c] & playerID2Mask[_])
							if (hasPlayer == -1)
								hasPlayer = _;
							else
								hasPlayer = 4;
					if (hasPlayer == 4)
						putchar('*');
					else if (hasPlayer != -1)
						putchar('0' + hasPlayer);
					else if (fieldStatic[r][c] & generator)
						putchar('G');
					else if (fieldContent[r][c] & playerMask)
						putchar('*');
					else if (fieldContent[r][c] & smallFruit)
						putchar('.');
					else if (fieldContent[r][c] & largeFruit)
						putchar('o');
					else
						putchar(' ');
					putchar(' ');
				}
				putchar((fieldStatic[r][width - 1] & wallEast) ? '|' : ' ');
				putchar('\n');
			}
			putchar(' ');
			putchar(' ');
			for (int c = 0; c < width; c++)
			{
				putchar(' ');
				printf((fieldStatic[height - 1][c] & wallSouth) ? "---" : "   ");
			}
			putchar('\n');
#endif
		}

		Json::Value SerializeCurrentTurnChange()
		{
			Json::Value result;
			TurnStateTransfer &bt = backtrack[turnID - 1];
			for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
			{
				result["actions"][_] = bt.actions[_];
				result["strengthDelta"][_] = bt.strengthDelta[_];
				result["change"][_] = bt.change[_];
			}
			return result;
		}

		// 初始化游戏管理器
		GameField()
		{
			if (constructed)
				throw runtime_error("请不要再创建 GameField 对象了，整个程序中只应该有一个对象");
			constructed = true;

			turnID = 0;
		}

		//GameField(const GameField &b) : GameField() { }
	};

	bool GameField::constructed = false;
}

// 一些辅助程序
namespace Helpers
{
	using namespace EnumExt;

    int distance[FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH][FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH]{};
    int randomPlayCount = 0;
    clock_t startTime = clock();
    string debugData;
    bool timeOutFlag = false;
    std::vector<string> jiangXuan = {
        /*	u8"赶紧续一秒 +1s",
        u8"人吶就都不知道",
        u8"自己不可以预料",
        u8"一个人的命运啊",
        u8"当然要靠自我的奋斗",
        u8"也要考虑历史的行程",
        u8"作为一个上海市委书记",
        u8"怎么就被调到北京去了",
        u8"我说另请高明吧",
        u8"我实在也不是谦虚",
        u8"中央研究都决定了",
        u8"你来当总书记",
        u8"当时我就念了两居诗",
        u8"苟利国家生死以",
        u8"岂因祸福避趋之",
        u8"我主要的就是三件事情",
        u8"军队一律不得经商",
        u8"这对军队的命运很重要",
        u8"就做了一点微小的工作",
        u8"很惭愧，但是Excited",
        u8"吼啊",
        u8"董先生吼不吼啊",
        u8"按照基本法，选举法",
        u8"最近欧盟出了一份报告",
        u8"内定，硬点的感觉",
        u8"不要见得风就是雨",
        u8"自己也要学会判断",
        u8"无中生有的东西",
        u8"你也有责任，对吧",
        u8"无可奉告！",
        u8"你们又不高兴"
        u8"不是我要钦点他",
        u8"你问我支持不支持",
        u8"我明确地告诉你这一点",
        u8"你们新闻界还要学习",
        u8"西方的那一套理论",
        u8"你们毕竟还too young",
        u8"我是身经百战啦",
        u8"我是见得多啦",
        u8"西方国家我都去过",
        u8"不知道高到哪里去啦",
        u8"我跟他谈笑风生",
        u8"提高自己的知识水平",
        u8"识得唔识得啊？",
        u8"我也替你们着急啊",
        u8"你们有一个好",
        u8"跑得比西方记者还快",
        u8"问来问去的问题啊",
        u8"都too simple",
        u8"sometimes naive",
        u8"我今天是作为一个长者",
        u8"我不是新闻工作者",
        u8"但是我见得太多了",
        u8"告诉你们人生的经验",
        u8"闷声大发财",
        u8"什么都不说是坠好的",
        u8"看到你们这样的热情啊",
        u8"一句话不说又不好",
        u8"报道偏差你们要负责",
        u8"我没有说要硬点",
        u8"没有任何这个意思",
        u8"要要，要要",
        u8"我们的决定权也很重要",
        u8"到那时候我们会表态",
        u8"不要想喜欢弄个大新闻",
        u8"就把我批判一番",
        u8"你们啊，naive",
        u8"我今天算是得罪了你们",
        u8"I'm angry!", 暂时不可用*/
        "Congratulations! +1s",
        "Congratulations! +2s",
        "Congratulations! +5s",
        "A man, He not knows",
        "He can't predict",
        "Fen Dou + History",
        "I'm not being modest",
        "I'm a Shanghai mayor",
        "U ask other GAO MING",
        "ZHONGYANG decided!",
        "3 minor things",
        "Army No Bussiness!",
        "Micro work so CanKui!"
        "This thing, Excited!"
        "HO AH",
        "HO BU HO AH?",
        "ZHONGYANG supports!"
        "Read The EU's report",
        "Never heard of",
        "No Feng Is Rain",
        "Judge By Yourself!",
        "You too have ZEREN!",
        "NeiDing, QingDian",
        "WU KE FENG GAO！",
        "We talk, Wind blows",
        "Level of Knowledge",
        "I ZHAOJI FOR U!",
        "U have One Ge good!",
        "You Need Xue One Ge",
        "Hum, Western Theory",
        "You ，Naive！"
        "I SHEN ME YE no say.",
        "This is ZUI HAO DE！",
        "No Any This Meaning",
        "If U Have to ask me",
        "I Will Biao Tai",
        "wanna make BIG NEWS?",
        "sound outta sin(x)?",
        "I'll sing BJ opera",
        "Two Ju Cantonese",
        "I Drive Red Ferrari",
        "YoYo! YoYo!",
        "I'm healthier than u",
        "U Young People",
        "No Future For U"
    };


	inline double TimeThrough()
	{
		return double(clock() - Helpers::startTime) / CLOCKS_PER_SEC;
	}

    inline bool TimeOut()
    {
		if (timeOutFlag || TimeThrough() > TIME_LIMIT) {
			Helpers::debugData += " TIMEOUT!!! ";
			return timeOutFlag = true;
		}
		return false;
	}

	inline int RandBetween(int a, int b)
	{
		if (a > b)
			swap(a, b);
		return rand() % (b - a) + a;
	}

	inline int DeltaATK(const Pacman::GameField &gamefield, int id1, int id2)
	{
		return gamefield.players[id1].strength - gamefield.players[id2].strength;
	}

	string MoHa()
	{
		return jiangXuan[RandBetween(0, jiangXuan.size())];
	}


	// Jet:可以试试哪个效率比较高
	typedef std::vector<Pacman::FieldProp> Path;
	//typedef std::vector<Pacman::Direction> Path;

	// Jet: 用cc的改的
	int Distance(const Pacman::GameField &gameField, Pacman::FieldProp startPos, Pacman::FieldProp endPos)
	{
		if (distance[startPos.row][startPos.col][endPos.row][endPos.col])
			return distance[startPos.row][startPos.col][endPos.row][endPos.col];
		if (startPos == endPos)
			return 0;

		//初始化广搜数组
		int** step = new int*[gameField.height];
		for (int i = 0; i < gameField.height; i++)
		{
			step[i] = new int[gameField.width];
			for (int j = 0; j < gameField.width; j++)
				step[i][j] = MAX_INT;
		}
		step[startPos.row][startPos.col] = 0;

		//初始化广搜队列
		Pacman::FieldProp queue[QUEUE_MAX];
		queue[0] = startPos;
		int nowFlag = 0, endFlag = 0;
		bool hasFound = false;

		while (nowFlag <= endFlag && !hasFound)
		{
			const Pacman::GridStaticType &curGrid = gameField.fieldStatic[queue[nowFlag].row][queue[nowFlag].col];
			for (Pacman::Direction dir = Pacman::Direction::up; dir < 4; ++dir)
			{
				if (!(curGrid & Pacman::direction2OpposingWall[dir]))
				{
					Pacman::FieldProp newPos = queue[nowFlag];
					newPos.row = (newPos.row + Pacman::dy[dir] + gameField.height) % gameField.height;
					newPos.col = (newPos.col + Pacman::dx[dir] + gameField.width) % gameField.width;
					if (step[newPos.row][newPos.col] > step[queue[nowFlag].row][queue[nowFlag].col] + 1) //新的点是好的
					{
						step[newPos.row][newPos.col] = step[queue[nowFlag].row][queue[nowFlag].col] + 1;
						queue[++endFlag] = newPos;
						if (newPos == endPos)
						{
							hasFound = true;
							break;
						}
					}
				}
			}
			++nowFlag;
		}

		int ret = step[endPos.row][endPos.col];
		for (int i = 0; i < gameField.height; i++)
			delete[]step[i];
		delete[]step;

		return distance[startPos.row][startPos.col][endPos.row][endPos.col] = ret;
	}

	// Moriartycc: 牢记位运算优先级
	int Distance(const Pacman::GameField &gameField, int alphaID, int betaID)
	{
		return Distance(gameField, gameField.players[alphaID], gameField.players[betaID]);
	}

	//weaZen: 返回distance<<3 + dir + 1以便决策
	// Jet: 改写了个模版
	template <typename __Pred>
	int GetTo(Pacman::GameField &gameField, int myID, __Pred pr)
	{
		Pacman::FieldProp startPos = gameField.players[myID];
		if (pr(gameField, startPos))
			return 0;

		//初始化广搜数组
		Pacman::Direction** dirInfo = new Pacman::Direction*[gameField.height];
		for (int i = 0; i < gameField.height; i++)
		{
			dirInfo[i] = new Pacman::Direction[gameField.width];
			for (int j = 0; j < gameField.width; j++)
				dirInfo[i][j] = Pacman::Direction::stay;
		}
		int** step = new int*[gameField.height];
		for (int i = 0; i < gameField.height; i++)
		{
			step[i] = new int[gameField.width];
			for (int j = 0; j < gameField.width; j++)
				step[i][j] = MAX_INT;
		}
		step[startPos.row][startPos.col] = 0;


		//初始化广搜队列
		Pacman::FieldProp queue[QUEUE_MAX];
		queue[0] = startPos;
		int nowFlag = 0, endFlag = 0;
		int dis = 0;
		bool hasEaten = false;

		while (nowFlag <= endFlag && !hasEaten)
		{
			const Pacman::GridStaticType &curGrid = gameField.fieldStatic[queue[nowFlag].row][queue[nowFlag].col];
			for (Pacman::Direction dir = Pacman::Direction::up; dir < 4; ++dir)
			{
				if (!(curGrid & Pacman::direction2OpposingWall[dir]))
				{
					Pacman::FieldProp newPos = queue[nowFlag];
					newPos.row = (newPos.row + Pacman::dy[dir] + gameField.height) % gameField.height;
					newPos.col = (newPos.col + Pacman::dx[dir] + gameField.width) % gameField.width;
					if (dirInfo[newPos.row][newPos.col] == -1 && newPos != startPos)
					{
						if (step[newPos.row][newPos.col] > step[queue[nowFlag].row][queue[nowFlag].col] + 1)
							step[newPos.row][newPos.col] = step[queue[nowFlag].row][queue[nowFlag].col] + 1;
						dirInfo[newPos.row][newPos.col] = dir;
						queue[++endFlag] = newPos;
						if (pr(gameField, newPos))
						{
							hasEaten = true;
							dis = step[queue[endFlag].row][queue[endFlag].col];
							break;
						}
					}
				}
			}
			++nowFlag;
		}
		if (!hasEaten)
			return Pacman::Direction::ur + 1;

		//cout << '*' << queue[endFlag].row << ' ' << queue[endFlag].col << endl;

		//回溯
		Pacman::Direction dir;
		Pacman::FieldProp curPos = queue[endFlag];
		while (curPos != startPos)
		{
			dir = dirInfo[curPos.row][curPos.col];
			curPos.row = (curPos.row - Pacman::dy[dir] + gameField.height) % gameField.height;
			curPos.col = (curPos.col - Pacman::dx[dir] + gameField.width) % gameField.width;
		}
		for (int i = 0; i < gameField.height; i++)
			delete[]dirInfo[i];
		delete[]dirInfo;

		return (dis << 3) + dir + 1;
	}

	//weaZen:照着cc的广搜写了个寻找方向 target是GridContentType里的组合 可以试一下吃人了//ω\\)
	int GetToTarget(Pacman::GameField &gameField, int myID, int target)
	{
		if (target == 0) return Pacman::Direction::ur + 1;
		return GetTo(gameField, myID,
			[target](const Pacman::GameField& gameField, const Pacman::FieldProp& pos)
		{ return gameField.fieldContent[pos.row][pos.col] & target; });
	}

	//weaZen: 把判断是不是在生成器旁边的函数单独拿出来 搜索有用
	// Jet: 没豆子吃的时候去生成器旁边等着
	bool isBesideGenerator(const Pacman::GameField& gameField, const Pacman::FieldProp& pos)
	{
		for (int i = 0; i < gameField.generatorCount; i++)
			for (Pacman::Direction d = Pacman::Direction::up; d < 8; ++d)
			{
				int r = (gameField.generators[i].row + Pacman::dy[d] + gameField.height) % gameField.height,
					c = (gameField.generators[i].col + Pacman::dx[d] + gameField.width) % gameField.width;
				if (pos.row == r && pos.col == c)
					return true;
			}
		return false;
	}

	int GetToNearbyGenerator(Pacman::GameField &gameField, int myID)
	{
		return GetTo(gameField, myID, isBesideGenerator);
	}

	// Jet: 算直线距离
	double DirectDistance(Pacman::FieldProp startPos, Pacman::FieldProp endPos)
	{
		return sqrt((startPos.row - endPos.row) * (startPos.row - endPos.row) + (startPos.col - endPos.col) * (startPos.col - endPos.col));
	}

	// weaZen:简单的危险判断
	bool DangerJudge(const Pacman::GameField &gameField, int myID, Pacman::Direction myDir = Pacman::stay)
	{
		const Pacman::Player & me = gameField.players[myID];
		Pacman::FieldProp myPos;
		if (myDir != Pacman::Direction::stay)
		{
			myPos.row = (me.row + Pacman::dy[myDir] + gameField.height) % gameField.height;
			myPos.col = (me.col + Pacman::dx[myDir] + gameField.width) % gameField.width;
		}
		else
			myPos = me;
		for (int _ = 0; _ < 4; ++_)
		{
			if (DeltaATK(gameField, myID, _) < 0)
			{
				if (Helpers::Distance(gameField, myPos, gameField.players[_]) <= 1)
					return true;
			}
		}
		return false;
	}

	//weaZen:随便找个不被吃的方向(如果可以)
	//Jet:需要返回随机值的情况大多可以返回AI::RandomAI()代替
	Pacman::Direction SimpleRandom(Pacman::GameField &gameField, int myID)
	{
		Pacman::Direction dir;
		int vCount = 0;
		Pacman::Direction valid[5];
		for (Pacman::Direction d = Pacman::stay; d < 4; ++d)
			if (gameField.ActionValid(myID, d) && !DangerJudge(gameField, myID, d))
				valid[vCount++] = d;
		if (vCount == 0) return Pacman::Direction::stay;
		dir = valid[RandBetween(0, vCount)];
		return dir;
	}



	char RandomPlay(Pacman::GameField &gameField, int myID, bool noStay)
	{
		randomPlayCount++;
		int count = 0, myAct = -1;
		while (true)
		{
			// 对每个玩家生成随机的合法动作
			for (int i = 0; i < MAX_PLAYER_COUNT; i++)
			{
				if (gameField.players[i].dead)
					continue;
				Pacman::Direction valid[5];
				int vCount = 0;
				for (Pacman::Direction d = Pacman::Direction(-1 + noStay); d < 4; ++d)
					if (gameField.ActionValid(i, d))
						valid[vCount++] = d;
				gameField.actions[i] = valid[RandBetween(0, vCount)];
			}

			if (count == 0)
				myAct = gameField.actions[myID];

			// 演算一步局面变化
			// NextTurn返回true表示游戏没有结束
			bool hasNext = gameField.NextTurn();
			count++;

			if (!hasNext)
				break;
		}

		// 计算分数
		int rank2player[] = { 0, 1, 2, 3 };
		for (int j = 0; j < MAX_PLAYER_COUNT; j++)
			for (int k = 0; k < MAX_PLAYER_COUNT - j - 1; k++)
				if (gameField.players[rank2player[k]].strength > gameField.players[rank2player[k + 1]].strength)
					swap(rank2player[k], rank2player[k + 1]);

		int actionScore;
		int scorebase = 0;
		if (rank2player[0] == myID)
			actionScore = 0;
		else
			for (int j = 1; j < MAX_PLAYER_COUNT; j++)
			{
				if (gameField.players[rank2player[j - 1]].strength < gameField.players[rank2player[j]].strength)
					scorebase = j + 1;
				if (rank2player[j] == myID)
				{
					actionScore = scorebase;
					break;
				}
			}

		// 恢复游戏状态到本回合初
		gameField.RollBack(count);

		return ((myAct + 1) << 2) + actionScore;
	}
}

namespace AI
{
    using namespace EnumExt;
    typedef std::pair<Pacman::Direction, float> Solution;

	Pacman::Direction MCTS_AI(Pacman::GameField &gameField, int myID, bool noStay = false)
	{
		int actionScore[5]{};
		char tmp;
		while (!Helpers::TimeOut())
		{
			tmp = Helpers::RandomPlay(gameField, myID, noStay);
			actionScore[tmp >> 2] += (tmp & 3);
		}

		int maxD = 0, d;
		for (d = 0; d < 5; d++)
			if (actionScore[d] > actionScore[maxD])
				maxD = d;

		return Pacman::Direction(maxD - 1);
	}

	//weaZen：先吃到豆再说，尽量避免被人吃掉
	//weaZen： 现在他只会把在逃不出死路的玩家和附近可能会走进死路的玩家加入攻击范围
	Pacman::Direction NaiveAI(Pacman::GameField &gameField, int myID)
	{
		int fruitDirInfo, playerDirInfo;
		int fruitTarget = (Pacman::GridContentType::smallFruit | Pacman::GridContentType::largeFruit);
		int playerTarget = 0;
		int fruitInfo, playerInfo;
		Pacman::Direction dir;

		for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
		{
			Pacman::Player & rival = gameField.players[_];
			if (rival.dead || _ == myID)
				continue;
			if (Helpers::DeltaATK(gameField, myID, _) > 0)
			{
				if (gameField.pathInfo[rival.row][rival.col].isImpasse && gameField.pathInfo[rival.row][rival.col].fleeLength + 2 >= Helpers::Distance(gameField, gameField.players[myID], *gameField.pathInfo[rival.row][rival.col].pExit))
					playerTarget |= Pacman::playerID2Mask[_];
				if (gameField.pathInfo[rival.row][rival.col].isExit && Helpers::Distance(gameField, myID, _) <= 2)
					playerTarget |= Pacman::playerID2Mask[_];
			}
		}
		fruitInfo = Helpers::GetToTarget(gameField, myID, fruitTarget);
		playerInfo = Helpers::GetToTarget(gameField, myID, playerTarget);
#ifdef DEBUG
		cout << '#' << myID << ' ' << (fruitInfo >> 3) << ' ' << Pacman::dirStr[fruitInfo & 7] << ' ' << (playerInfo >> 3) << ' ' << Pacman::dirStr[playerInfo & 7] << endl;
#endif // DEBUG
		fruitDirInfo = fruitInfo & 7;
		playerDirInfo = playerInfo & 7;

		switch ((fruitDirInfo == 5) * 10 + (playerDirInfo == 5))
		{
		case (11):
			dir = Pacman::Direction((Helpers::GetToNearbyGenerator(gameField, myID) & 7) - 1);
			break;
		case (10):
			dir = Pacman::Direction(playerDirInfo - 1);
			break;
		case (1):
			dir = Pacman::Direction(fruitDirInfo - 1);
			break;
		case (0):
			if ((playerInfo >> 3) <= 2 + (fruitInfo >> 3))
				dir = Pacman::Direction(playerDirInfo - 1);
			else
				dir = Pacman::Direction(fruitDirInfo - 1);
			break;
		}

		if (dir != Pacman::Direction::stay && !Helpers::DangerJudge(gameField, myID, dir))
			return dir;
		//为了能够搜索减少耗时直接随机
		return Helpers::SimpleRandom(gameField, myID);
		//return MCTS_AI(gameField, myID);
		//return MCTS_AI(gameField, myID, true);
	}

	// Jet :这是一个考虑豆子分布情况进行估计的估值函数
	float GreedyEval(const Pacman::GameField &gameField, int myID)
	{
		float minGeneratorDis = 100.0f;
		float generatorDisSum = 0.0f;
		int strengthSum = 0;
		if (gameField.players[myID].dead)
			return -1000000.0f;
		float e = 0.0f;
		float tmp;
		for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
		{
			strengthSum += gameField.players[i].strength;
		}
		e = gameField.players[myID].strength / (float)strengthSum * 100.0f;
		if (!gameField.hasNext)
			return e;

		for (int i = 0; i < gameField.generatorCount; i++)
		{
			tmp = float(Helpers::DirectDistance(gameField.generators[i], gameField.players[myID]));
			generatorDisSum += tmp;
			if (minGeneratorDis > tmp)
				minGeneratorDis = tmp;
		}
		if (minGeneratorDis > gameField.generatorTurnLeft)
			e -= minGeneratorDis - gameField.generatorTurnLeft;

		//else e -= 0.5 * generatorDisSum / gameField.generatorCount;

		//这里暂时不太完善
		//      for (int i = 0; i < MAX_PLAYER_COUNT; i++)
		//      {
		//          if (i == myID)
		//              continue;

		//          float dD = Helpers::Distance(gameField, myID, i) + 0.5;//防止除以零
		//          if (dD >= 5)
		//              continue;

		//          int dA = Helpers::DeltaATK(gameField, myID, i);
		//          if (dA >= 3)
		//              e += float(dA) / dD;
		//          else if (dA >= 1)
		//              e += 1.0f / dD;
		//          else if (dA <= -1)
		//              e -= 1.0f * dA * dA / dD;
		//      }


		for (int i = 0; i < gameField.height; i++)
			for (int j = 0; j < gameField.width; j++)
				if ((tmp = gameField.GetFruitValue(i, j)) != 0)
					e -= tmp * Helpers::Distance(gameField, Pacman::FieldProp(i, j), gameField.players[myID]) / 100.0f;

		//e -= 2.0f * Helpers::DangerJudge(gameField, myID);
		//e += gameField.players[myID].strength;
		return e;
	}

	// weaZen:简单的搜索，调用返回最高估值 若上一步造成力量变化则不给出lastDir
	float SimpleSearch(Pacman::GameField &gameField, int myID, int depth, Pacman::Direction lastDir = Pacman::Direction::stay)
	{
		float max = -1000000.0f;
		float tmp;
		int strength = gameField.players[myID].strength;
        //cout << depth << ' ';
        if (Helpers::TimeOut() || depth == 0 || gameField.players[myID].dead || !gameField.hasNext)
            return GreedyEval(gameField, myID);
        for (Pacman::Direction dir = Pacman::stay; dir <= Pacman::left; ++dir)
        {
            if (!gameField.ActionValid(myID, dir) || Helpers::DangerJudge(gameField, myID, dir)) 
                continue;
			//基于以下两点猜测减少搜索量
			//1.没有力量增加却往反方向跑是无意义的
			//2.不在生成器周围却不动是无意义的
			if (lastDir != Pacman::Direction::stay && Pacman::dy[dir] + Pacman::dy[lastDir] == 0 && Pacman::dx[dir] + Pacman::dx[lastDir] == 0)
				continue;
			if (dir == Pacman::Direction::stay && !Helpers::isBesideGenerator(gameField, gameField.players[myID]))
				continue;

			for (int i = 0; i < MAX_PLAYER_COUNT; i++)
			{
				if (i == myID)
					continue;
				if (gameField.players[i].dead)
					continue;
				gameField.actions[i] = NaiveAI(gameField, i);
				//cout << '*' << i << ' ' << Pacman::dirStr[gameField.actions[i] + 1] << endl;
			}
			gameField.actions[myID] = dir;
			gameField.NextTurn();
			if (gameField.players[myID].strength - strength == 0)
				tmp = SimpleSearch(gameField, myID, depth - 1, dir);
			else tmp = SimpleSearch(gameField, myID, depth - 1);
            tmp += GreedyEval(gameField, myID); //这是为了把来回走的淘汰掉
            max = std::max(max, tmp);
            gameField.RollBack(1);

			// 超时处理
			if (Helpers::TimeOut())
				return max;
		}

		return max;
	}

	// Jet :这是一个考虑豆子分布情况进行估计的AI
	Solution GreedySearchAI(Pacman::GameField &gameField, int myID, int depth = DEFAULT_DEPTH)
	{
		float *evals = new float[5];
		float max = -1000000.0f;
		Pacman::Direction naiveDir = NaiveAI(gameField, myID);
		if (gameField.turnID == MAX_TURN - 1)
			return std::make_pair(naiveDir, 0.0f);
		for (Pacman::Direction dir = Pacman::stay; dir <= Pacman::left; ++dir)
		{
			if (!gameField.ActionValid(myID, dir))
			{
				evals[dir + 1] = -9999999.0f;
				continue;
			}
			if (Helpers::DangerJudge(gameField, myID, dir))
			{
				evals[dir + 1] = -1000000.0f;
				continue;
			}
			for (int i = 0; i < MAX_PLAYER_COUNT; i++)
			{
				if (i == myID || gameField.players[i].dead)
					continue;
				gameField.actions[i] = NaiveAI(gameField, i);
#ifdef DEBUG
				cout << '*' << i << ' ' << Pacman::dirStr[gameField.actions[i] + 1] << endl;
#endif // DEBUG
			}
			gameField.actions[myID] = dir;
			gameField.NextTurn();
			evals[dir + 1] = AI::SimpleSearch(gameField, myID, depth);
			//不知道为什么特别容易不动 只好先这样了
			if (dir == Pacman::Direction::stay && !Helpers::isBesideGenerator(gameField, gameField.players[myID]))
				evals[dir + 1] /= 2;
			max = std::max(max, evals[dir + 1]);
			gameField.RollBack(1);
		}

		evals[NaiveAI(gameField, myID) + 1] += 1.0f;

        int maxD = 0;
        for (int d = 0; d < 5; d++)
        {
            Helpers::debugData += '*' + Pacman::dirStr[d] + ' ' + to_string(evals[d]) + ' ';
            if (evals[d] >= evals[maxD])
                maxD = d;
        }
        if (evals[naiveDir + 1] == max)
            return std::make_pair(naiveDir, max);
        max = evals[maxD];
        delete[] evals;
        return std::make_pair(Pacman::Direction(maxD - 1), max);
    }

    Pacman::Direction IterativeGreedySearch(Pacman::GameField &gameField, int myID)
    {
        typedef std::pair<Pacman::Direction, float> Solution;
        std::vector<Solution> solutions;

        for (int depth = DEFAULT_DEPTH; !Helpers::TimeOut() && depth < MAX_DEPTH; depth++)
        {
            solutions.push_back(GreedySearchAI(gameField, myID, depth));
            Helpers::debugData += " depth" + to_string(depth) + ' ' + to_string(solutions.back().second) + ' ';
        }
        if (solutions.size() >= 2 && Helpers::TimeOut())
            solutions.pop_back();

        return solutions.back().first;
    }

	//Pacman::Direction JetAI(Pacman::GameField &gameField, int myID)
	//{
	//    throw new std::exception("Not Implemented");
	//}
}

int main()
{
    auto AI = AI::IterativeGreedySearch;
    auto TAUNT = Helpers::MoHa;

	Pacman::GameField mainGameField;
	string data, globalData; // 这是回合之间可以传递的信息
							 // 如果在本地调试，有input.txt则会读取文件内容作为输入
							 // 如果在平台上，则不会去检查有无input.txt
	int myID = mainGameField.ReadInput("input.txt", data, globalData); // 输入，并获得自己ID
	srand(unsigned(Pacman::seed + myID));

	// 输出当前游戏局面状态以供本地调试。注意提交到平台上会自动优化掉，不必担心。
	mainGameField.DebugPrint();

#ifndef _BOTZONE_ONLINE
	Helpers::startTime = clock();
#endif


	// 中央决定一定要叫嚣
	Pacman::Direction choice = AI(mainGameField, myID);

#ifndef _BOTZONE_ONLINE
    Helpers::debugData.clear();
#endif

	Helpers::debugData += "Time used " + to_string(Helpers::TimeThrough());
	mainGameField.WriteOutput(choice, TAUNT(), data, globalData, Helpers::debugData);

	//cout << Helpers::Distance(mainGameField, 1, 1);

#ifndef _BOTZONE_ONLINE
	system("pause");
#endif
	return 0;
}
