/*
* Edited By Jet, Moriarty, weaZen
* 2016/05/05 00:31
* AI XXXXX
*
* Pacman ��������
* ���ߣ�zhouhy
* ʱ�䣺2016/3/22 15:32:51
* �����£�2016/4/22 16:18
*/

#include <fstream>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <string>
#include <stack>
#include <stdexcept>
#include <vector>
#include "jsoncpp/json.h"

#define FIELD_MAX_HEIGHT 20
#define FIELD_MAX_WIDTH 20
#define MAX_GENERATOR_COUNT 4 // ÿ������1
#define MAX_PLAYER_COUNT 4
#define MAX_TURN 100
#define TIME_LIMIT 0.98
#define QUEUE_MAX 121
#define MAX_INT 0x3fffffff
#define DEFAULT_DEPTH 6
#define MAX_DEPTH 15
#define DEATH_EVAL -1000000
#define INVALID_EVAL -9999999

#ifndef _BOTZONE_ONLINE
//#define DEBUG
#define PROFILING
#endif

// ��Ҳ����ѡ�� using namespace std; ���ǻ���Ⱦ�����ռ�
using std::cin;
using std::cout;
using std::endl;
using std::swap;
using std::string;
using std::getline;
using std::to_string;
using std::runtime_error;

// ���ڵ���
namespace Debug
{
    auto printInfo = false;
	string presetString;
#ifdef DEBUG
	//presetString = R"*({"requests":[{"GENERATOR_INTERVAL":20,"LARGE_FRUIT_DURATION":10,"LARGE_FRUIT_ENHANCEMENT":10,"content":[[0,16,16,32,0,16,0,32,16,16,0],[0,0,1,0,0,0,0,0,2,0,0],[16,0,0,0,16,0,16,0,0,0,16],[0,0,16,0,0,0,0,0,16,0,0],[16,16,0,0,0,0,0,0,0,16,16],[16,16,0,0,0,0,0,0,0,16,16],[0,0,16,0,0,0,0,0,16,0,0],[16,0,0,0,16,0,16,0,0,0,16],[0,0,4,0,0,0,0,0,8,0,0],[0,16,16,32,0,16,0,32,16,16,0]],"height":10,"id":3,"seed":1462548363,"static":[[2,10,13,5,5,1,5,5,7,10,8],[8,6,9,5,1,4,1,5,3,12,2],[4,5,6,9,6,31,12,3,12,5,4],[1,7,9,6,13,1,7,12,3,13,1],[8,5,0,1,5,4,5,1,0,5,2],[8,5,0,4,5,1,5,4,0,5,2],[4,7,12,3,13,4,7,9,6,13,4],[1,5,3,12,3,31,9,6,9,5,1],[8,3,12,5,4,1,4,5,6,9,2],[2,10,13,5,5,4,5,5,7,10,8]],"width":11},{"0":{"action":1},"1":{"action":2},"2":{"action":1},"3":{"action":0}},{"0":{"action":1},"1":{"action":1},"2":{"action":1},"3":{"action":1}},{"0":{"action":2},"1":{"action":1},"2":{"action":0},"3":{"action":1}},{"0":{"action":3},"1":{"action":1},"2":{"action":3},"3":{"action":1}},{"0":{"action":2},"1":{"action":3},"2":{"action":0},"3":{"action":3}},{"0":{"action":3},"1":{"action":1},"2":{"action":3},"3":{"action":1}},{"0":{"action":2},"1":{"action":3},"2":{"action":0},"3":{"action":3}},{"0":{"action":3},"1":{"action":1},"2":{"action":3},"3":{"action":1}},{"0":{"action":3},"1":{"action":3},"2":{"action":3},"3":{"action":3}},{"0":{"action":0},"1":{"action":1},"2":{"action":2},"3":{"action":1}},{"0":{"action":3},"1":{"action":3},"2":{"action":3},"3":{"action":3}},{"0":{"action":2},"1":{"action":1},"2":{"action":0},"3":{"action":1}},{"0":{"action":3},"1":{"action":3},"2":{"action":3},"3":{"action":3}},{"0":{"action":3},"1":{"action":1},"2":{"action":3},"3":{"action":1}},{"0":{"action":0},"1":{"action":3},"2":{"action":2},"3":{"action":3}},{"0":{"action":3},"1":{"action":1},"2":{"action":3},"3":{"action":1}},{"0":{"action":0},"1":{"action":3},"2":{"action":2},"3":{"action":3}},{"0":{"action":3},"1":{"action":1},"2":{"action":3},"3":{"action":1}},{"0":{"action":0},"1":{"action":3},"2":{"action":2},"3":{"action":3}},{"0":{"action":3},"1":{"action":1},"2":{"action":3},"3":{"action":1}},{"0":{"action":3},"1":{"action":3},"2":{"action":3},"3":{"action":3}},{"0":{"action":2},"1":{"action":1},"2":{"action":1},"3":{"action":1}},{"0":{"action":0},"1":{"action":3},"2":{"action":2},"3":{"action":3}},{"0":{"action":1},"1":{"action":1},"2":{"action":-1},"3":{"action":1}},{"0":{"action":1},"1":{"action":3},"2":{"action":3},"3":{"action":3}},{"0":{"action":3},"1":{"action":1},"2":{"action":3},"3":{"action":1}},{"0":{"action":0},"1":{"action":3},"2":{"action":-1},"3":{"action":3}},{"0":{"action":1},"1":{"action":1},"2":{"action":-1},"3":{"action":1}},{"0":{"action":1},"1":{"action":3},"2":{"action":1},"3":{"action":3}},{"0":{"action":1},"1":{"action":1},"2":{"action":-1},"3":{"action":1}},{"0":{"action":3},"1":{"action":3},"2":{"action":1},"3":{"action":3}}],"responses":[{"action":0,"tauntText":""},{"action":1,"tauntText":""},{"action":1,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""},{"action":1,"tauntText":""},{"action":3,"tauntText":""}]})*";
#endif

    Json::Value debugData;
    auto timeOutFlag = false;
    auto startTime = clock();

    inline double TimeThrough(clock_t stt = startTime)
    {
        return double(clock() - stt) / CLOCKS_PER_SEC;
    }

    inline bool TimeOut()
    {
#ifdef DEBUG
        return false;
#endif
        if (timeOutFlag)
            return true;
        if (TimeThrough() > TIME_LIMIT)
        {
            debugData["profiling"]["TimeOut"] = true;
            return timeOutFlag = true;
        }
        return false;
    }
}

//��ö����չ������
namespace EnumExt
{
    // ��ö��Ҳ��������Щ�����ˣ����ӻ�������
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

// ƽ̨�ṩ�ĳԶ�������߼��������
namespace Pacman
{
    using namespace EnumExt;

    const auto seed = time(nullptr);
    const int dx[] = {0, 1, 0, -1, 1, 1, -1, -1}, dy[] = {-1, 0, 1, 0, -1, 1, 1, -1};
    const string dirStr[] = {"stay" ,"up","right","down","left","ur","dr","dl","ul"};

    // ö�ٶ��壻ʹ��ö����Ȼ���˷ѿռ䣨sizeof(GridContentType) == 4�������Ǽ��������32λ������Ч�ʸ���

    // ÿ�����ӿ��ܱ仯�����ݣ�����á����߼��������
    enum GridContentType
    {
        empty = 0, // ��ʵ�����õ�
        player1 = 1, // 1�����
        player2 = 2, // 2�����
        player3 = 4, // 3�����
        player4 = 8, // 4�����
        playerMask = 1 | 2 | 4 | 8, // ���ڼ����û����ҵ�
        smallFruit = 16, // С����
        largeFruit = 32 // ����
    };

    // �����ID��ȡ��������ҵĶ�����λ
    GridContentType playerID2Mask[] = {player1, player2, player3, player4};
    string playerID2str[] = {"0", "1", "2", "3"};

    // ÿ�����ӹ̶��Ķ���������á����߼��������
    enum GridStaticType
    {
        emptyWall = 0, // ��ʵ�����õ�
        wallNorth = 1, // ��ǽ����������ٵķ���
        wallEast = 2, // ��ǽ�����������ӵķ���
        wallSouth = 4, // ��ǽ�����������ӵķ���
        wallWest = 8, // ��ǽ����������ٵķ���
        generator = 16 // ���Ӳ�����
    };

    // ���ƶ�����ȡ����������赲�ŵ�ǽ�Ķ�����λ
    GridStaticType direction2OpposingWall[] = {wallNorth, wallEast, wallSouth, wallWest};

    // ���򣬿��Դ���dx��dy���飬ͬʱҲ������Ϊ��ҵĶ���
    enum Direction
    {
        stay = -1,
        up = 0,
        right = 1,
        down = 2,
        left = 3,
        // ������⼸��ֻ��Ϊ�˲��������򷽱㣬����ʵ���õ�
        ur = 4, // ����
        dr = 5, // ����
        dl = 6, // ����
        ul = 7 // ����
    };

    // �����ϴ�����������
    struct FieldProp
    {
        int row, col;
        FieldProp(int i = 0, int j = 0): row(i), col(j) {}
        bool operator==(const FieldProp &a)const { return (row == a.row && col == a.col); }
        bool operator!=(const FieldProp &a)const { return (row != a.row || col != a.col); }
    };

    struct PathInfoType: FieldProp
    {
        bool isImpasse;
        bool isExit;
        int fleeLength;//����·���ڵľ���
        PathInfoType * pExit;
        PathInfoType(int y = 0, int x = 0): FieldProp(y, x), isImpasse(false), isExit(false), fleeLength(0), pExit(nullptr) {}
    };

    // �����ϵ����
    struct Player: FieldProp
    {
        int strength;
        int powerUpLeft;
        bool dead;
    };

    // �غ��²����Ķ��ӵ�����
    struct NewFruits
    {
        FieldProp newFruits[MAX_GENERATOR_COUNT * 8];
        int newFruitCount;
    } newFruits[MAX_TURN];
    int newFruitsCount = 0;

    // ״̬ת�Ƽ�¼�ṹ
    struct TurnStateTransfer
    {
        enum StatusChange // �����
        {
            none = 0,
            ateSmall = 1,
            ateLarge = 2,
            powerUpCancel = 4,
            die = 8,
            error = 16
        };

        // ���ѡ���Ķ���
        Direction actions[MAX_PLAYER_COUNT];

        // �˻غϸ���ҵ�״̬�仯
        StatusChange change[MAX_PLAYER_COUNT];

        // �˻غϸ���ҵ������仯
        int strengthDelta[MAX_PLAYER_COUNT];
    };

    // ��Ϸ��Ҫ�߼������࣬��������������غ����㡢״̬ת�ƣ�ȫ��Ψһ
    class GameField
    {
        // Ϊ�˷��㣬��������Զ�����private��
        // ��¼ÿ�غϵı仯��ջ��
        TurnStateTransfer backtrack[MAX_TURN];

        // ��������Ƿ��Ѿ�����
        static bool constructed;

    public:
        // ���صĳ��Ϳ�
        int height, width;
        int generatorCount;
        int GENERATOR_INTERVAL, LARGE_FRUIT_DURATION, LARGE_FRUIT_ENHANCEMENT;

        //��·��Ϣ
        PathInfoType pathInfo[FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH];

        // ���ظ��ӹ̶�������
        GridStaticType fieldStatic[FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH];

        // ���ظ��ӻ�仯������
        GridContentType fieldContent[FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH];
        int generatorTurnLeft; // ���ٻغϺ��������
        int aliveCount; // �ж�����Ҵ��
        int smallFruitCount;
        FieldProp generators[MAX_GENERATOR_COUNT]; // ����Щ���Ӳ�����
        Player players[MAX_PLAYER_COUNT]; // ����Щ���
        int turnID;
        Direction actions[MAX_PLAYER_COUNT]; // ���ѡ���Ķ���
        bool hasNext; // weaZen��ʡ��ÿ�β�һ��

        // �ָ����ϴγ���״̬������һ·�ָ����ʼ��
        // �ָ�ʧ�ܣ�û��״̬�ɻָ�������false
        bool PopState()
        {
            hasNext = true;
            if (turnID <= 0)
                return false;

            const auto &bt = backtrack[--turnID];

            // �������ָ�״̬
            for (int playerID = 0; playerID < MAX_PLAYER_COUNT; playerID++)
            {
                auto &player = players[playerID];
                auto &content = fieldContent[player.row][player.col];
                auto change = bt.change[playerID];

                if (!player.dead)
                {
                    // 5. �󶹻غϻָ�
                    if (player.powerUpLeft || change & TurnStateTransfer::powerUpCancel)
                        player.powerUpLeft++;

                    // 4. �³�����
                    if (change & TurnStateTransfer::ateSmall)
                    {
                        content |= smallFruit;
                        smallFruitCount++;
                    }
                    else if (change & TurnStateTransfer::ateLarge)
                    {
                        content |= largeFruit;
                        player.powerUpLeft -= LARGE_FRUIT_DURATION;
                    }
                }

                // 2. �������
                if (change & TurnStateTransfer::die)
                {
                    player.dead = false;
                    aliveCount++;
                    content |= playerID2Mask[playerID];
                }

                // 1. ���λ�Ӱ
                if (!player.dead && bt.actions[playerID] != stay)
                {
                    fieldContent[player.row][player.col] &= ~playerID2Mask[playerID];
                    player.row = (player.row - dy[bt.actions[playerID]] + height) % height;
                    player.col = (player.col - dx[bt.actions[playerID]] + width) % width;
                    fieldContent[player.row][player.col] |= playerID2Mask[playerID];
                }

                // 0. ���겻�Ϸ������
                if (change & TurnStateTransfer::error)
                {
                    player.dead = false;
                    aliveCount++;
                    content |= playerID2Mask[playerID];
                }

                // *. �ָ�����
                if (!player.dead)
                    player.strength -= bt.strengthDelta[playerID];
            }

            // 3. �ջض���
            if (generatorTurnLeft == GENERATOR_INTERVAL)
            {
                generatorTurnLeft = 1;
                auto &fruits = newFruits[--newFruitsCount];
                for (int i = 0; i < fruits.newFruitCount; i++)
                {
                    fieldContent[fruits.newFruits[i].row][fruits.newFruits[i].col] &= ~smallFruit;
                    smallFruitCount--;
                }
            }
            else
                generatorTurnLeft++;

            return true;
        }

        // Jet:��PopState��װ��һ�� ����һЩ
        void RollBack(int turnCount = -1)
        {
#ifdef PROFILING
            auto startTime = clock();
#endif
            if (turnCount < 0)
            // ReSharper disable once CppPossiblyErroneousEmptyStatements
                while (PopState());
            else
                for (int i = 0; i < turnCount; i++)
                    if (!PopState())
                        break;
#ifdef PROFILING
            auto& d = Debug::debugData["profiling"]["RollBack()"];
            d = d.asDouble() + double(clock() - startTime) / CLOCKS_PER_SEC;
#endif
        }

        // �ж�ָ�������ָ�������ƶ��ǲ��ǺϷ��ģ�û��ײǽ��û�вȵ����Ӳ�������
        inline bool ActionValid(int playerID, const Direction &dir) const
        {
            if (dir == stay)
                return true;
            const auto &p = players[playerID];
            const auto &s = fieldStatic[p.row][p.col];
            return dir >= -1 && dir < 4 && !(s & direction2OpposingWall[dir]);
        }

        // ����actionsд����Ҷ�����������һ�غϾ��棬����¼֮ǰ���еĳ���״̬���ɹ��պ�ָ���
        // ���վֵĻ��ͷ���false
        bool NextTurn()
        {
#ifdef PROFILING
            auto startTime = clock();
#endif
            auto &bt = backtrack[turnID];
            memset(&bt, 0, sizeof bt);

            // 0. ɱ�����Ϸ�����
            for (int playerID = 0; playerID < MAX_PLAYER_COUNT; playerID++)
            {
                auto &player = players[playerID];
                if (!player.dead)
                {
                    auto &action = actions[playerID];
                    if (action == stay)
                        continue;

                    if (!ActionValid(playerID, action))
                    {
                        bt.strengthDelta[playerID] += -player.strength;
                        bt.change[playerID] = TurnStateTransfer::error;
                        fieldContent[player.row][player.col] &= ~playerID2Mask[playerID];
                        player.strength = 0;
                        player.dead = true;
                        aliveCount--;
                    }
                    else
                    {
                        // �������Լ�ǿ��׳������ǲ���ǰ����
                        const auto& target = fieldContent
                            [(player.row + dy[action] + height) % height]
                            [(player.col + dx[action] + width) % width];
                        if (target & playerMask)
                            for (int i = 0; i < MAX_PLAYER_COUNT; i++)
                                if (target & playerID2Mask[i] && players[i].strength > player.strength)
                                    action = stay;
                    }
                }
            }

            // 1. λ�ñ仯
            for (int playerID = 0; playerID < MAX_PLAYER_COUNT; playerID++)
            {
                auto &player = players[playerID];
                if (player.dead)
                    continue;

                bt.actions[playerID] = actions[playerID];

                if (actions[playerID] == stay)
                    continue;

                // �ƶ�
                fieldContent[player.row][player.col] &= ~playerID2Mask[playerID];
                player.row = (player.row + dy[actions[playerID]] + height) % height;
                player.col = (player.col + dx[actions[playerID]] + width) % width;
                fieldContent[player.row][player.col] |= playerID2Mask[playerID];
            }

            // 2. ��һ�Ź
            for (int playerID = 0; playerID < MAX_PLAYER_COUNT; playerID++)
            {
                auto &player = players[playerID];
                if (player.dead)
                    continue;

                // �ж��Ƿ��������һ��
                int containedCount = 0;
                int containedPlayers[MAX_PLAYER_COUNT];
                for (int i = 0; i < MAX_PLAYER_COUNT; i++)
                    if (fieldContent[player.row][player.col] & playerID2Mask[i])
                        containedPlayers[containedCount++] = i;

                if (containedCount > 1)
                {
                    // NAIVE
                    for (int i = 0; i < containedCount; i++)
                        for (int j = 0; j < containedCount - i - 1; j++)
                            if (players[containedPlayers[j]].strength < players[containedPlayers[j + 1]].strength)
                                swap(containedPlayers[j], containedPlayers[j + 1]);

                    int begin;
                    for (begin = 1; begin < containedCount; begin++)
                        if (players[containedPlayers[begin - 1]].strength > players[containedPlayers[begin]].strength)
                            break;

                    // ��Щ��ҽ��ᱻɱ��
                    int lootedStrength = 0;
                    for (int i = begin; i < containedCount; i++)
                    {
                        int id = containedPlayers[i];
                        Player &p = players[id];

                        // �Ӹ���������
                        fieldContent[p.row][p.col] &= ~playerID2Mask[id];
                        p.dead = true;
                        int drop = p.strength / 2;
                        bt.strengthDelta[id] += -drop;
                        bt.change[id] |= TurnStateTransfer::die;
                        lootedStrength += drop;
                        p.strength -= drop;
                        aliveCount--;
                    }

                    // ������������
                    auto inc = lootedStrength / begin;
                    for (int i = 0; i < begin; i++)
                    {
                        auto id = containedPlayers[i];
                        auto &p = players[id];
                        bt.strengthDelta[id] += inc;
                        p.strength += inc;
                    }
                }
            }

            // 3. ��������
            if (--generatorTurnLeft == 0)
            {
                generatorTurnLeft = GENERATOR_INTERVAL;
                auto &fruits = newFruits[newFruitsCount++];
                fruits.newFruitCount = 0;
                for (int i = 0; i < generatorCount; i++)
                    for (auto d = up; d < 8; ++d)
                    {
                        // ȡ�࣬�������ر߽�
                        int r = (generators[i].row + dy[d] + height) % height, c = (generators[i].col + dx[d] + width) % width;
                        if (fieldStatic[r][c] & generator || fieldContent[r][c] & (smallFruit | largeFruit))
                            continue;
                        fieldContent[r][c] |= smallFruit;
                        fruits.newFruits[fruits.newFruitCount].row = r;
                        fruits.newFruits[fruits.newFruitCount++].col = c;
                        smallFruitCount++;
                    }
            }

            // 4. �Ե�����
            for (int playerID = 0; playerID < MAX_PLAYER_COUNT; playerID++)
            {
                auto &player = players[playerID];
                if (player.dead)
                    continue;

                auto &content = fieldContent[player.row][player.col];

                // ֻ���ڸ�����ֻ���Լ���ʱ����ܳԵ�����
                if (content & playerMask & ~playerID2Mask[playerID])
                    continue;

                if (content & smallFruit)
                {
                    content &= ~smallFruit;
                    player.strength++;
                    bt.strengthDelta[playerID]++;
                    smallFruitCount--;
                    bt.change[playerID] |= TurnStateTransfer::ateSmall;
                }
                else if (content & largeFruit)
                {
                    content &= ~largeFruit;
                    if (player.powerUpLeft == 0)
                    {
                        player.strength += LARGE_FRUIT_ENHANCEMENT;
                        bt.strengthDelta[playerID] += LARGE_FRUIT_ENHANCEMENT;
                    }
                    player.powerUpLeft += LARGE_FRUIT_DURATION;
                    bt.change[playerID] |= TurnStateTransfer::ateLarge;
                }
            }

            // 5. �󶹻غϼ���
            for (int playerID = 0; playerID < MAX_PLAYER_COUNT; playerID++)
            {
                auto &player = players[playerID];
                if (player.dead)
                    continue;

                if (player.powerUpLeft > 0 && --player.powerUpLeft == 0)
                {
                    player.strength -= LARGE_FRUIT_ENHANCEMENT;
                    bt.change[playerID] |= TurnStateTransfer::powerUpCancel;
                    bt.strengthDelta[playerID] += -LARGE_FRUIT_ENHANCEMENT;
                }
            }

            ++turnID;
#ifdef PROFILING
            auto&& d = Debug::debugData["profiling"]["NextTurn()"];
            d = d.asDouble() + double(clock() - startTime) / CLOCKS_PER_SEC;
#endif
            // �Ƿ�ֻʣһ�ˣ�
            if (aliveCount <= 1)
            {
                for (int playerID = 0; playerID < MAX_PLAYER_COUNT; playerID++)
                    if (!players[playerID].dead)
                    {
                        bt.strengthDelta[playerID] += smallFruitCount;
                        players[playerID].strength += smallFruitCount;
                    }
                return hasNext = false;
            }

            // �Ƿ�غϳ��ޣ�
            if (turnID >= 100)
                return hasNext = false;
            return hasNext = true;
        }

        // ��ȡrow, colλ�õ�ˮ����ֵ
        inline char GetFruitValue(int row, int col)const
        {
            char v = 0;
            if (fieldContent[row][col] & smallFruit)
                v += 1;
            if (fieldContent[row][col] & largeFruit)
                v += isSmallMap() ? 7 : 3;
            return v;
        }

        inline bool isSmallMap()const
        {
            return height + width <= 16;
        }

        //weaZen: ��ͼ����
        void MapAnalyze()
        {
#ifdef PROFILING
            auto startTime = clock();
#endif

            FieldProp deadSpot[40];
            int degree[FIELD_MAX_HEIGHT][FIELD_MAX_HEIGHT];
            int dCount = 0;
            PathInfoType * ptmpExit;
            ptmpExit = nullptr;

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
                    ++nowFlag;
                }
                for (int j = 0; j <= endFlag; ++j)
                {
                    pathInfo[queue[j].row][queue[j].col].fleeLength = endFlag - j + 1;
                    pathInfo[queue[j].row][queue[j].col].pExit = ptmpExit;
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
#ifdef PROFILING
        auto&& d = Debug::debugData["profiling"]["MapAnalyze()"];
        d = d.asDouble() + double(clock() - startTime) / CLOCKS_PER_SEC;
#endif 
        }

        // ��ȡ�������������룬���ص��Ի��ύƽ̨ʹ�ö����ԡ�
        // ����ڱ��ص��ԣ�����������Ŷ�ȡ������ָ�����ļ���Ϊ�����ļ���ʧ�ܺ���ѡ��ȴ��û�ֱ�����롣
        // ���ص���ʱ���Խ��ܶ����Ա������Windows�¿����� Ctrl-Z ��һ��������+�س�����ʾ���������������������ֻ����ܵ��м��ɡ�
        // localFileName ����ΪNULL
        // obtainedData ������Լ��ϻغϴ洢�����غ�ʹ�õ�����
        // obtainedGlobalData ������Լ��� Bot ����ǰ�洢������
        // ����ֵ���Լ��� playerID
        int ReadInput(const char *localFileName, string &obtainedData, string &obtainedGlobalData)
        {
            string str, chunk;
            if (!Debug::presetString.empty())
                str = Debug::presetString;
            else
            {
#ifdef _BOTZONE_ONLINE
                std::ios::sync_with_stdio(false); //��\\)
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

            // ��ȡ���ؾ�̬״��
            Json::Value field = input["requests"][Json::Value::UInt(0)],
                staticField = field["static"], // ǽ��Ͳ�����
                contentField = field["content"]; // ���Ӻ����
            height = field["height"].asInt();
            width = field["width"].asInt();
            LARGE_FRUIT_DURATION = field["LARGE_FRUIT_DURATION"].asInt();
            LARGE_FRUIT_ENHANCEMENT = field["LARGE_FRUIT_ENHANCEMENT"].asInt();
            generatorTurnLeft = GENERATOR_INTERVAL = field["GENERATOR_INTERVAL"].asInt();

            PrepareInitialField(staticField, contentField);

            hasNext = true;

            MapAnalyze();

            // ������ʷ�ָ�����
            Json::Value req;
            for (int i = 1; i < len; i++)
            {
                req = input["requests"][i];
                for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
                    if (!players[_].dead)
                        actions[_] = Direction(req[playerID2str[_]]["action"].asInt());
                NextTurn();
            }

            obtainedData = input["data"].asString();
            obtainedGlobalData = input["globaldata"].asString();

            return field["id"].asInt();
        }

        // ���� static �� content ����׼�����صĳ�ʼ״��
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
                    GridContentType &content = fieldContent[r][c] = GridContentType(contentField[r][c].asInt());
                    GridStaticType &s = fieldStatic[r][c] = GridStaticType(staticField[r][c].asInt());
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

        // ��ɾ��ߣ���������
        // action ��ʾ���غϵ��ƶ�����stay Ϊ���ƶ�
        // tauntText ��ʾ��Ҫ��������������������ַ�����������ʾ����Ļ�ϲ������κ����ã����ձ�ʾ������
        // data ��ʾ�Լ���洢����һ�غ�ʹ�õ����ݣ����ձ�ʾɾ��
        // globalData ��ʾ�Լ���洢���Ժ�ʹ�õ����ݣ��滻����������ݿ��Կ�Ծ�ʹ�ã���һֱ������� Bot �ϣ����ձ�ʾɾ��
        // Jet: debugDataΪһ��Json����botzone�ϲ���ӡ�����ڱ��ص���
        void WriteOutput(Direction action, string& tauntText, Json::Value& data,
                        Json::Value& globalData, Json::Value& debugData) const
        {
            debugData["seed"] = to_string(seed);

            Json::Value ret;
            ret["response"]["action"] = action;
            ret["response"]["tauntText"] = tauntText;
            ret["data"] = data;
            ret["globaldata"] = globalData;
            if (Debug::printInfo)
                ret["debug"] = debugData;

#ifdef _BOTZONE_ONLINE
            Json::FastWriter writer; // ��������Ļ����þ��С���
#else
            Json::StyledWriter writer; // ���ص��������ÿ� > <
#endif
            cout << writer.write(ret) << endl;
        }

        // ������ʾ��ǰ��Ϸ״̬�������á�
        // �ύ��ƽ̨��ᱻ�Ż�����
        inline void DebugPrint() const
        {
#ifndef _BOTZONE_ONLINE
            printf("�غϺš�%d�����������%d��| ͼ�� ������[G] �����[0/1/2/3] ������[*] ��[o] С��[.]\n", turnID, aliveCount);
            for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
            {
                auto& p = players[_];
                printf("[���%d(%d, %d)|����%d|�ӳ�ʣ��غ�%d|%s]\n",
                       _, p.row, p.col, p.strength, p.powerUpLeft, p.dead ? "����" : "���");
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

        // ��ʼ����Ϸ������
        GameField()
        {
            if (constructed)
                throw runtime_error("�벻Ҫ�ٴ��� GameField �����ˣ�����������ֻӦ����һ������");
            constructed = true;

            turnID = 0;
        }

        //GameField(const GameField &b) : GameField() { }
    };

    bool GameField::constructed = false;
}

// һЩ��������
namespace Helpers
{
    using namespace EnumExt;

    class Solution: public std::pair<Pacman::Direction, int>
    {
    public:
        Solution() {}
        Solution(const std::pair<Pacman::Direction, int>& p): std::pair<Pacman::Direction, int>(p) {}
        bool operator < (const Solution& o)const { return second < o.second; }
    };

    int distance[FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH][FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH]{};
    int randomPlayCount = 0;
    std::vector<string> jiangXuan = {
        u8"�Ͻ���һ�� +1s",
        u8"�˅ȾͶ���֪��",
        u8"�Լ�������Ԥ��",
        u8"һ���˵����˰�",
        u8"��ȻҪ�����ҵķܶ�",
        u8"ҲҪ������ʷ���г�",
        u8"��Ϊһ���Ϻ���ί���",
        u8"��ô�ͱ���������ȥ��",
        u8"��˵���������",
        u8"��ʵ��Ҳ����ǫ��",
        u8"�����о���������",
        u8"�����������",
        u8"��ʱ�Ҿ���������ʫ",
        u8"��������������",
        u8"�����������֮",
        u8"����Ҫ�ľ�����������",
        u8"����һ�ɲ��þ���",
        u8"��Ծ��ӵ����˺���Ҫ",
        u8"������һ��΢С�Ĺ���",
        u8"�ܲ���������Excited",
        u8"��",
        u8"�������𲻺�",
        u8"���ջ�������ѡ�ٷ�",
        u8"���ŷ�˳���һ�ݱ���",
        u8"�ڶ���Ӳ��ĸо�",
        u8"��Ҫ���÷������",
        u8"�Լ�ҲҪѧ���ж�",
        u8"�������еĶ���",
        u8"��Ҳ�����Σ��԰�",
        u8"�޿ɷ�棡",
        u8"�����ֲ�����"
        u8"������Ҫ�յ���",
        u8"������֧�ֲ�֧��",
        u8"����ȷ�ظ�������һ��",
        u8"�������Ž绹Ҫѧϰ",
        u8"��������һ������",
        u8"���ǱϾ���too young",
        u8"��������ս��",
        u8"���Ǽ��ö���",
        u8"���������Ҷ�ȥ��",
        u8"��֪���ߵ�����ȥ��",
        u8"�Ҹ���̸Ц����",
        u8"����Լ���֪ʶˮƽ",
        u8"ʶ����ʶ�ð���",
        u8"��Ҳ�������ż���",
        u8"������һ����",
        u8"�ܵñ��������߻���",
        u8"������ȥ�����Ⱑ",
        u8"��too simple",
        u8"sometimes naive",
        u8"�ҽ�������Ϊһ������",
        u8"�Ҳ������Ź�����",
        u8"�����Ҽ���̫����",
        u8"�������������ľ���",
        u8"�����󷢲�",
        u8"ʲô����˵��׹�õ�",
        u8"�����������������鰡",
        u8"һ�仰��˵�ֲ���",
        u8"����ƫ������Ҫ����",
        u8"��û��˵ҪӲ��",
        u8"û���κ������˼",
        u8"ҪҪ��ҪҪ",
        u8"���ǵľ���ȨҲ����Ҫ",
        u8"����ʱ�����ǻ��̬",
        u8"��Ҫ��ϲ��Ū��������",
        u8"�Ͱ�������һ��",
        u8"���ǰ���naive",
        u8"�ҽ������ǵ���������",
        u8"I'm angry!",
        /*"Congratulations! +1s",
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
        "WU KE FENG GAO��",
        "We talk, Wind blows",
        "Level of Knowledge",
        "I ZHAOJI FOR U!",
        "U have One Ge good!",
        "You Need Xue One Ge",
        "Hum, Western Theory",
        "You ��Naive��"
        "I SHEN ME YE no say.",
        "This is ZUI HAO DE��",
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
        "No Future For U"*/
    };

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

    inline string MoHa()
    {
        return jiangXuan[RandBetween(0, jiangXuan.size())];
    }

    inline string KeepSilentMakeFortune()
    {
        return "";
    }

    // weaZen: ˫��
    // Jet: ��cc�ĸĵ�
    int Distance(const Pacman::GameField &gameField, Pacman::FieldProp startPos, Pacman::FieldProp endPos)
    {
#ifdef PROFILING
        auto startTime = clock();
#endif
        if (startPos == endPos)
            return distance[startPos.row][startPos.col][endPos.row][endPos.col] = 0;

        if (distance[startPos.row][startPos.col][endPos.row][endPos.col])
            return distance[startPos.row][startPos.col][endPos.row][endPos.col];
        

        //��ʼ����������
        int** step = new int*[gameField.height];
        for (int i = 0; i < gameField.height; i++)
        {
            step[i] = new int[gameField.width];
            for (int j = 0; j < gameField.width; j++)
                step[i][j] = 0;
        }
        step[startPos.row][startPos.col] = 1;
        step[endPos.row][endPos.col] = -1;

        //��ʼ�����Ѷ���
        Pacman::FieldProp queue[QUEUE_MAX];
        queue[0] = startPos;
        queue[1] = endPos;
        int nowFlag = 0, endFlag = 1;
        bool hasFound = false;
        int ret = 0;

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
                    if (step[queue[nowFlag].row][queue[nowFlag].col] > 0)
                    {
                        if (step[newPos.row][newPos.col] > step[queue[nowFlag].row][queue[nowFlag].col] + 1 || step[newPos.row][newPos.col] == 0) //�µĵ��Ǻõ�
                        {
                            step[newPos.row][newPos.col] = step[queue[nowFlag].row][queue[nowFlag].col] + 1;
                            distance[startPos.row][startPos.col][newPos.row][newPos.col] = step[newPos.row][newPos.col] - 1;
                            queue[++endFlag] = newPos;
                        }
                        if (step[newPos.row][newPos.col] < 0)
                        {
                            hasFound = true;
                            ret = step[queue[nowFlag].row][queue[nowFlag].col] - step[newPos.row][newPos.col] - 1;
                        }
                    }
                    if (step[queue[nowFlag].row][queue[nowFlag].col] < 0)
                    {
                        if (step[newPos.row][newPos.col] < step[queue[nowFlag].row][queue[nowFlag].col] - 1 || step[newPos.row][newPos.col] == 0) //�µĵ��Ǻõ�
                        {
                            step[newPos.row][newPos.col] = step[queue[nowFlag].row][queue[nowFlag].col] - 1;
                            distance[endPos.row][endPos.col][newPos.row][newPos.col] = -step[newPos.row][newPos.col] - 1;
                            queue[++endFlag] = newPos;
                        }
                        if (step[newPos.row][newPos.col] > 0)
                        {
                            hasFound = true;
                            ret = -step[queue[nowFlag].row][queue[nowFlag].col] + step[newPos.row][newPos.col] - 1;
                        }
                    }
                }
            }
            ++nowFlag;
        }

        for (int i = 0; i < gameField.height; i++)
            delete[]step[i];
        delete[]step;
#ifdef PROFILING
        auto&& d = Debug::debugData["profiling"]["Distance()"];
        d = d.asDouble() + double(clock() - startTime) / CLOCKS_PER_SEC;
#endif
        return distance[startPos.row][startPos.col][endPos.row][endPos.col] = ret;
    }

    // Moriartycc: �μ�λ�������ȼ�
    int Distance(const Pacman::GameField &gameField, int alphaID, int betaID)
    {
        return Distance(gameField, gameField.players[alphaID], gameField.players[betaID]);
    }

    // weaZen: ����distance<<3 + dir + 1�Ա����
    // Jet: ��д�˸�ģ��
    template <typename __Pred>
    char GetTo(Pacman::GameField &gameField, int myID, __Pred pr, char forbiddenDirs = '\0')
    {
#ifdef PROFILING
        clock_t startTime = clock();
#endif

        Pacman::FieldProp startPos = gameField.players[myID];
        if (pr(gameField, startPos) && !(forbiddenDirs & 1))
            return 0;

        //��ʼ����������
        Pacman::Direction** dirInfo = new Pacman::Direction*[gameField.height];
        for (int i = 0; i < gameField.height; i++)
        {
            dirInfo[i] = new Pacman::Direction[gameField.width];
            for (int j = 0; j < gameField.width; j++)
                dirInfo[i][j] = Pacman::Direction::stay;
        }
        char** step = new char*[gameField.height];
        for (int i = 0; i < gameField.height; i++)
        {
            step[i] = new char[gameField.width];
            for (int j = 0; j < gameField.width; j++)
                step[i][j] = 31;
        }
        step[startPos.row][startPos.col] = 0;

        //��ʼ�����Ѷ���
        Pacman::FieldProp queue[QUEUE_MAX];
        queue[0] = startPos;
        char nowFlag = 0, endFlag = 0;
        char dis = 0;
        bool hasEaten = false;

        //��ʼ���������
        Pacman::Direction randomDir[4];
        for (int i = 0; i < 4; ++i)
            randomDir[i] = Pacman::Direction(i);

		//��ֹ�ķ�����Ϊ�Ѿ�����
		for (int i = 0; i < 4; ++i)
		{
			if (forbiddenDirs & (1 << (i + 1)) && !(gameField.fieldStatic[startPos.row][startPos.col] & Pacman::direction2OpposingWall[i]))
				dirInfo[(startPos.row + Pacman::dy[i] + gameField.height) % gameField.height][(startPos.col + Pacman::dx[i] + gameField.width) % gameField.width] = Pacman::Direction::up;
		}

        while (nowFlag <= endFlag && !hasEaten)
        {
            const Pacman::GridStaticType &curGrid = gameField.fieldStatic[queue[nowFlag].row][queue[nowFlag].col];
            for (int i = 0; i < 4; ++i)
                swap(randomDir[RandBetween(0, 4)], randomDir[RandBetween(0, 4)]);
            Pacman::Direction dir;
            for (int i = 0; i < 4; ++i)
            {
                dir = randomDir[i];
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

        //����
        Pacman::Direction dir = Pacman::stay;
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
#ifdef PROFILING
        auto&& d = Debug::debugData["profiling"]["GetTo()"];
        d = d.asDouble() + double(clock() - startTime) / CLOCKS_PER_SEC;
#endif

        return (dis << 3) + dir + 1;
    }

    //weaZen:����cc�Ĺ���д�˸�Ѱ�ҷ��� target��GridContentType������ ������һ�³�����//��\\)
    char GetToTarget(Pacman::GameField &gameField, int myID, int target, char forbiddenDirs = '\0')
    {
        if (target == 0)
            return Pacman::Direction::ur + 1;
        return GetTo(gameField, myID,
                     [target](const Pacman::GameField& gameField, const Pacman::FieldProp& pos)
        { return gameField.fieldContent[pos.row][pos.col] & target; }, forbiddenDirs);
    }

    //weaZen: ���ж��ǲ������������Աߵĺ��������ó��� ��������
    // Jet: û���ӳԵ�ʱ��ȥ�������Աߵ���
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

    char GetToNearbyGenerator(Pacman::GameField &gameField, int myID, char forbiddenDirs = '\0')
    {
        return GetTo(gameField, myID, isBesideGenerator, forbiddenDirs);
    }

    // Jet: ������ֱ�߾���
    inline int ApprDirectDistance(Pacman::FieldProp startPos, Pacman::FieldProp endPos)
    {
        return (abs(startPos.row - endPos.row) + abs(startPos.col - endPos.col) + 1) / 2;
    }

    // weaZen:�򵥵�Σ���ж�
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
                if (Helpers::Distance(gameField, myPos, gameField.players[_]) <= 1)
                    return true;
        }
        return false;
    }

    //weaZen:����Ҹ������Եķ���(�������)
    //Jet:��Ҫ�������ֵ����������Է���AI::RandomAI()����
    Pacman::Direction SimpleRandom(Pacman::GameField &gameField, int myID, char forbiddenDirs = '\0')
    {
        Pacman::Direction dir;
        int vCount = 0;
        Pacman::Direction valid[5];
        for (Pacman::Direction d = Pacman::stay; d < 4; ++d)
            if (gameField.ActionValid(myID, d) && !(forbiddenDirs & (1 << (d + 1))))
                valid[vCount++] = d;
        if (vCount == 0) return Pacman::Direction::stay;
        dir = valid[RandBetween(0, vCount)];
        return dir;
    }

    Solution RandomPlay(Pacman::GameField &gameField, int myID, bool noStay)
    {
        randomPlayCount++;
        int count = 0, myAct = -1;
        while (true)
        {
            // ��ÿ�������������ĺϷ�����
            for (int i = 0; i < MAX_PLAYER_COUNT; i++)
            {
                if (gameField.players[i].dead)
                    continue;
                Pacman::Direction valid[5]{};
                int vCount = 0;
                for (Pacman::Direction d = Pacman::Direction(-1 + noStay); d < 4; ++d)
                    if (gameField.ActionValid(i, d))
                        valid[vCount++] = d;
                gameField.actions[i] = valid[RandBetween(0, vCount)];
            }

            if (count == 0)
                myAct = gameField.actions[myID];

            // ����һ������仯
            // NextTurn����true��ʾ��Ϸû�н���
            bool hasNext = gameField.NextTurn();
            count++;

            if (!hasNext)
                break;
        }

        // �������
        int actionScore,
            total = 0;
        for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
            total += gameField.players[_].strength;

        actionScore = (10000 * gameField.players[myID].strength / total) / 100;

        // �ָ���Ϸ״̬�����غϳ�
        gameField.RollBack(count);

        return std::make_pair(Pacman::Direction(myAct), actionScore);
    }

    inline string depth2String(int depth)
    {
        string str = to_string(depth);
        if (str.length() == 1)
            str.insert(str.begin(), '0');
        return "depth = " + str;
    }
}

namespace AI
{
    using namespace EnumExt;
    using Helpers::Solution;
    int tmpEvals[5];
    int averagedEvals[5];

    //bool operator <(const Solution& a, const Solution& b) { return a.second < b.second; }
    //bool operator >(const Solution& a, const Solution& b) { return a.second > b.second; }
    int SimpleSearch(Pacman::GameField &gameField, int myID, int depth, Pacman::Direction(*rivalAI)(Pacman::GameField &, int), Pacman::Direction lastDir = Pacman::Direction::stay, bool top = false, bool rivalFlag = false);

    std::vector<Solution> MCTS_AI(Pacman::GameField &gameField, int myID, bool noStay = false, double timeOut = 0)
    {
        clock_t startTime = clock();
        int actionScore[5]{};
        while (Debug::TimeThrough(startTime) < timeOut)
        {
            Solution sol = Helpers::RandomPlay(gameField, myID, noStay);
            actionScore[sol.first + 1] += sol.second;
        }

        std::vector<Solution> solutions;
        for (int d = 0; d < 5; d++)
            solutions.push_back(std::make_pair(Pacman::Direction(d - 1), actionScore[d]));
        sort(solutions.begin(), solutions.end());

        return solutions;
    }

    //weaZen�� Ŀ�����ȼ�������·�Ͽ����Ӳ���������AI > ��������·���ڵ���AI�������ڼе��б�׷������AI > ���� > ������
    Pacman::Direction NaiveAttackAI(Pacman::GameField &gameField, int myID)
    {
        char fruitDirInfo, playerDirInfo, tryPlayerDirInfo;
        char fruitTarget = (Pacman::GridContentType::smallFruit | Pacman::GridContentType::largeFruit);
        char playerTarget = 0;
        char tryPlayerTarget = 0;
        char fruitInfo, playerInfo, tryPlayerInfo;
        char forbiddenDirs = '\0';
        Pacman::Direction dir;

		for (int i = -1; i < 4; ++i)
		{
			if (!gameField.ActionValid(myID, Pacman::Direction(i)))
			{
				forbiddenDirs |= 1 << (i + 1);
				continue;
			}
			if (Helpers::DangerJudge(gameField, myID, Pacman::Direction(i)))
				forbiddenDirs |= 1 << (i + 1);
		}
		if (forbiddenDirs == 31)//����������������
			return Helpers::SimpleRandom(gameField, myID);


        for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
        {
            Pacman::Player & rival = gameField.players[_];
            if (rival.dead || _ == myID)
                continue;
            if (Helpers::DeltaATK(gameField, myID, _) > 0)
            {
				bool preyFlag = gameField.pathInfo[rival.row][rival.col].isImpasse
                    && gameField.pathInfo[rival.row][rival.col].fleeLength + 2 >= Helpers::Distance(gameField, gameField.players[myID], *gameField.pathInfo[rival.row][rival.col].pExit);
                bool tryPreyFlag = gameField.pathInfo[rival.row][rival.col].isExit
                    && Helpers::Distance(gameField, myID, _) <= 2
                    && Helpers::DeltaATK(gameField, myID, _) > 1;
				//�е��ﱻ׷������AI
				if (!preyFlag && !tryPreyFlag)
				{
					int dirCount = 4;
					for (int i = 0; i < 4; ++i)
					{
						if (gameField.fieldStatic[rival.row][rival.col] & Pacman::direction2OpposingWall[i])
						{
							--dirCount;
							continue;
						}
						Pacman::FieldProp checkPos;
						checkPos.row = (rival.row + Pacman::dy[i] + gameField.height) % gameField.height;
						checkPos.col = (rival.col + Pacman::dx[i] + gameField.width) % gameField.width;
						if (gameField.fieldContent[checkPos.row][checkPos.col] & Pacman::playerMask)
							for (int checkID = 0; checkID < 4; ++checkID)
							{
								if (gameField.fieldContent[checkPos.row][checkPos.col] & Pacman::playerID2Mask[checkID]
									&& gameField.players[checkID].strength > rival.strength)
									{
										--dirCount;
										break;
									}
							}
					}
					if (dirCount == 1 && Helpers::Distance(gameField, myID, _) <= 2)
						tryPreyFlag = true;
				}
                if (preyFlag)
                    playerTarget |= Pacman::playerID2Mask[_];
                if (tryPreyFlag)
                    tryPlayerTarget |= Pacman::playerID2Mask[_];
            }
        }
        fruitInfo = Helpers::GetToTarget(gameField, myID, fruitTarget, forbiddenDirs);
        playerInfo = Helpers::GetToTarget(gameField, myID, playerTarget, forbiddenDirs);
        tryPlayerInfo = Helpers::GetToTarget(gameField, myID, tryPlayerTarget, forbiddenDirs);
		//һ�����ʷ�����ǰ����
		if (fruitInfo == '\0' && Helpers::RandBetween(0, 2) == 0)
				fruitInfo = Helpers::GetToTarget(gameField, myID, fruitTarget, forbiddenDirs | 1);
#ifdef DEBUG
//		cout << '#' << myID << ' ' << (fruitInfo >> 3) << ' ' << Pacman::dirStr[fruitInfo & 7] << ' ' << (playerInfo >> 3) << ' ' << Pacman::dirStr[playerInfo & 7] << endl;
#endif // DEBUG
        fruitDirInfo = fruitInfo & 7;
        playerDirInfo = playerInfo & 7;
        tryPlayerDirInfo = tryPlayerInfo & 7;

        int info = (fruitDirInfo < 5) + ((tryPlayerDirInfo < 5) << 1) + ((playerDirInfo < 5) << 2);

        if (info >= 4)
            dir = Pacman::Direction(playerDirInfo - 1);
        else
            if (info >= 2)
                dir = Pacman::Direction(tryPlayerDirInfo - 1);
            else
                if (info >= 1)
                    dir = Pacman::Direction(fruitDirInfo - 1);
                else
                    dir = Pacman::Direction((Helpers::GetToNearbyGenerator(gameField, myID, forbiddenDirs) & 7) - 1);

		if (dir != Pacman::Direction::stay && dir != Pacman::Direction::ur)
			return dir;
		//Ϊ���ܹ��������ٺ�ʱֱ�����
		if (Helpers::RandBetween(0, 4) <= 2 && dir == Pacman::Direction::stay)
			return dir;
		else
			return Helpers::SimpleRandom(gameField, myID, forbiddenDirs);
	}


    //weaZen�� ��ر������ĸ߼�AI
    Pacman::Direction NaiveThinkAI(Pacman::GameField &gameField, int myID)
    {
        char fruitDirInfo, playerDirInfo, tryPlayerDirInfo;
        char fruitTarget = (Pacman::GridContentType::smallFruit | Pacman::GridContentType::largeFruit);
        char playerTarget = 0;
        char tryPlayerTarget = 0;
        char fruitInfo, playerInfo, tryPlayerInfo;
        char forbiddenDirs = '\0';
        Pacman::Direction dir;

		for (int i = -1; i < 4; ++i)
		{
			dir = Pacman::Direction(i);
			Pacman::FieldProp nextGrid;
			if (!gameField.ActionValid(myID, Pacman::Direction(i)))
			{
				forbiddenDirs |= 1 << (i + 1);
				continue;
			}
			if (i != -1)
			{
				nextGrid.row = (gameField.players[myID].row + Pacman::dy[dir] + gameField.height) % gameField.height;
				nextGrid.col = (gameField.players[myID].col + Pacman::dx[dir] + gameField.width) % gameField.width;
			}
			else
				nextGrid = gameField.players[myID];

			if (Helpers::DangerJudge(gameField, myID, Pacman::Direction(i)))
				forbiddenDirs |= 1 << (i + 1);
			else if (gameField.pathInfo[nextGrid.row][nextGrid.col].isImpasse)
			{
				//forbiddenDirs |= 1 << (i + 1);
				int fleeLength = gameField.pathInfo[nextGrid.row][nextGrid.col].fleeLength;
				bool enemyFlag = false;
				//��·���ڸ���û��������ʱ����������
				//��ʵֻ��Ҫ����·��������fleeLength + 1�������˻�û�ߣ�, ���Ż�
				for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
				{
					if (_ == myID) continue;
					if (Helpers::Distance(gameField, gameField.players[_], *gameField.pathInfo[nextGrid.row][nextGrid.col].pExit) <= fleeLength + 1)
						enemyFlag = true;
				}
				if (!enemyFlag) continue;
				//ע��ֻ��һ��gamefield ģ������AIʱע��action�Ļ�ԭ
				Pacman::Direction tmpDir[MAX_PLAYER_COUNT];
				for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
					tmpDir[_] = gameField.actions[_];
				for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
				{
					if (_ == myID)
						continue;
					if (gameField.players[_].dead)
						continue;
					gameField.actions[_] = NaiveAttackAI(gameField, _);
				}
				gameField.actions[myID] = dir;
				gameField.NextTurn();
				if (SimpleSearch(gameField, myID, 4, NaiveAttackAI, Pacman::Direction::stay, true, true) <= DEATH_EVAL)
					forbiddenDirs |= 1 << (i + 1);
				gameField.RollBack(1);
				for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
					gameField.actions[_] = tmpDir[_];
			}
		}

		if (forbiddenDirs == 31)//����������������
			return Helpers::SimpleRandom(gameField, myID);


		
		for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
		{
			Pacman::Player & rival = gameField.players[_];
			if (rival.dead || _ == myID)
				continue;
			if (Helpers::DeltaATK(gameField, myID, _) > 0)
			{
				bool preyFlag = gameField.pathInfo[rival.row][rival.col].isImpasse
					&& (gameField.pathInfo[rival.row][rival.col].fleeLength + 2 >= Helpers::Distance(gameField, gameField.players[myID], *gameField.pathInfo[rival.row][rival.col].pExit));
				bool tryPreyFlag = gameField.pathInfo[rival.row][rival.col].isExit
					&& Helpers::Distance(gameField, myID, _) <= 2
					&& Helpers::DeltaATK(gameField, myID, _) > 1;
				//�е��ﱻ׷������AI
				if (!preyFlag && !tryPreyFlag)
				{
					int dirCount = 4;
					for (int i = 0; i < 4; ++i)
					{
						if (gameField.fieldStatic[rival.row][rival.col] & Pacman::direction2OpposingWall[i])
						{
							--dirCount;
							continue;
						}
						Pacman::FieldProp checkPos;
						checkPos.row = (rival.row + Pacman::dy[i] + gameField.height) % gameField.height;
						checkPos.col = (rival.col + Pacman::dx[i] + gameField.width) % gameField.width;
						if (gameField.fieldContent[checkPos.row][checkPos.col] & Pacman::playerMask)
							for (int checkID = 0; checkID < 4; ++checkID)
							{
								if (gameField.fieldContent[checkPos.row][checkPos.col] & Pacman::playerID2Mask[checkID]
									&& gameField.players[checkID].strength > rival.strength)
								{
									--dirCount;
									break;
								}
							}
					}
					if (dirCount == 1 && Helpers::Distance(gameField, myID, _) <= 2)
						tryPreyFlag = true;
				}
				if (preyFlag)
					playerTarget |= Pacman::playerID2Mask[_];
				if (tryPreyFlag)
					tryPlayerTarget |= Pacman::playerID2Mask[_];
			}
		}
		fruitInfo = Helpers::GetToTarget(gameField, myID, fruitTarget, forbiddenDirs);
		playerInfo = Helpers::GetToTarget(gameField, myID, playerTarget, forbiddenDirs);
		tryPlayerInfo = Helpers::GetToTarget(gameField, myID, tryPlayerTarget, forbiddenDirs);
		//һ�����ʷ�����ǰ����
		if (fruitInfo == '\0' && Helpers::RandBetween(0, 2) == 0)
			fruitInfo = Helpers::GetToTarget(gameField, myID, fruitTarget, forbiddenDirs | 1);
#ifdef DEBUG
        //		cout << '#' << myID << ' ' << (fruitInfo >> 3) << ' ' << Pacman::dirStr[fruitInfo & 7] << ' ' << (playerInfo >> 3) << ' ' << Pacman::dirStr[playerInfo & 7] << endl;
#endif // DEBUG
        fruitDirInfo = fruitInfo & 7;
        playerDirInfo = playerInfo & 7;
        tryPlayerDirInfo = tryPlayerInfo & 7;

        int info = (fruitDirInfo < 5) + ((tryPlayerDirInfo < 5) << 1) + ((playerDirInfo < 5) << 2);

        if (info >= 4)
            dir = Pacman::Direction(playerDirInfo - 1);
        else
            if (info >= 2)
                dir = Pacman::Direction(tryPlayerDirInfo - 1);
            else
                if (info >= 1)
                    dir = Pacman::Direction(fruitDirInfo - 1);
                else
                    dir = Pacman::Direction((Helpers::GetToNearbyGenerator(gameField, myID, forbiddenDirs) & 7) - 1);

		if (dir != Pacman::Direction::stay && dir != Pacman::Direction::ur)
			return dir;
		if (Helpers::RandBetween(0, 4) <= 2 && dir == Pacman::Direction::stay)
			return dir;
		else
			return Helpers::SimpleRandom(gameField, myID, forbiddenDirs);
	}

    int GreedyEval(const Pacman::GameField &gameField, int myID)
    {
#ifdef PROFILING
        auto startTime = clock();
#endif
        int minGeneratorDis = 100;
        int generatorDisSum = 0;
        int strengthSum = 0;
        if (gameField.players[myID].dead)
            return DEATH_EVAL;
        for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
            strengthSum += gameField.players[i].strength;

        if (!gameField.hasNext)
            return 1000 * gameField.players[myID].strength / strengthSum - 250;

        int e = 0;
        int tmp;

        for (int i = 0; i < gameField.generatorCount; i++)
        {
            tmp = Helpers::ApprDirectDistance(gameField.generators[i], gameField.players[myID]);
            generatorDisSum += tmp;
            if (minGeneratorDis > tmp)
                minGeneratorDis = tmp;
        }
        if (minGeneratorDis > gameField.generatorTurnLeft)
            e -= minGeneratorDis - gameField.generatorTurnLeft;

        int fruitEvalSum = 0;
        for (int i = 0; i < gameField.height; i++)
            for (int j = 0; j < gameField.width; j++)
                if ((tmp = gameField.GetFruitValue(i, j)) != 0)
                    fruitEvalSum += tmp * Helpers::Distance(gameField, Pacman::FieldProp(i, j), gameField.players[myID]);

		//e -= fruitEvalSum / 100;
		if (gameField.players[myID].powerUpLeft <= 0)
			e += gameField.players[myID].strength;
		else
			e += gameField.players[myID].strength - 10;// + gameField.players[myID].powerUpLeft;
#ifdef PROFILING
        auto&& d = Debug::debugData["profiling"]["GreedyEval()"];
        d = d.asDouble() + double(clock() - startTime) / CLOCKS_PER_SEC;
#endif
        return e;
    }

    // weaZen:�򵥵����������÷�����߹�ֵ ����һ����������仯�򲻸���lastDir
    int SimpleSearch(Pacman::GameField &gameField, int myID, int depth, Pacman::Direction(*rivalAI)(Pacman::GameField &, int), Pacman::Direction lastDir, bool top, bool rivalFlag)
    {
        int max = DEATH_EVAL;
        int tmp;
        int strength = gameField.players[myID].strength;
		int powerUpLeft = gameField.players[myID].powerUpLeft;
        //cout << depth << ' ';

        if (Debug::TimeOut() || depth == 0 || gameField.players[myID].dead || !gameField.hasNext)
            return GreedyEval(gameField, myID);
        for (Pacman::Direction dir = Pacman::stay; dir <= Pacman::left; ++dir)
        {
            if (top && !rivalFlag)
            {
                if (averagedEvals[dir + 1] <= DEATH_EVAL)
                {
                    tmpEvals[dir + 1] = averagedEvals[dir + 1];
                    continue;
                }
                if (!gameField.ActionValid(myID, dir))
                {
                    averagedEvals[dir + 1] = tmpEvals[dir + 1] = INVALID_EVAL;
                    continue;
                }
                if (Helpers::DangerJudge(gameField, myID, dir))
                {
                    averagedEvals[dir + 1] = tmpEvals[dir + 1] = DEATH_EVAL;
                    continue;
                }
            }
            if (!gameField.ActionValid(myID, dir) || Helpers::DangerJudge(gameField, myID, dir))
                continue;
            Pacman::FieldProp nextGrid;
            nextGrid.row = (gameField.players[myID].row + Pacman::dy[dir] + gameField.height) % gameField.height;
            nextGrid.col = (gameField.players[myID].col + Pacman::dx[dir] + gameField.width) % gameField.width;
            //������������²����������
            //1.û���������ӻ��������ȴ�������������������
            //2.������������Χ��ǰλ��û�й���ȴ�������������
            if (lastDir != Pacman::Direction::stay
                && Pacman::dy[dir] + Pacman::dy[lastDir] == 0
                && Pacman::dx[dir] + Pacman::dx[lastDir] == 0
                && !(gameField.fieldContent[nextGrid.row][nextGrid.col] & Pacman::playerMask))
                continue;
            if (!top && dir == Pacman::Direction::stay
                && (!Helpers::isBesideGenerator(gameField, gameField.players[myID]) || gameField.generatorTurnLeft > 3)
				&& !(gameField.fieldContent[gameField.players[myID].row][gameField.players[myID].col] & (Pacman::GridContentType::smallFruit | Pacman::GridContentType::largeFruit)))
                continue;

            for (int i = 0; i < MAX_PLAYER_COUNT; i++)
            {
                if (i == myID)
                    continue;
                if (gameField.players[i].dead)
                    continue;
                gameField.actions[i] = rivalAI(gameField, i);
#ifdef DEBUG
                if (top && !rivalFlag) cout << "AI " << i << ' ' << Pacman::dirStr[gameField.actions[i] + 1] << endl;
#endif // DEBUG
            }

            gameField.actions[myID] = dir;
            gameField.NextTurn();

            //�������ص��ڹ�����������
            if (gameField.players[myID].strength - strength == 0 && !(gameField.fieldContent[gameField.players[myID].row][gameField.players[myID].col] & (Pacman::GridContentType::smallFruit | Pacman::GridContentType::largeFruit)))
            {
                if (dir == Pacman::Direction::stay)
                    tmp = SimpleSearch(gameField, myID, depth - 1, rivalAI, lastDir);
                else
                    tmp = SimpleSearch(gameField, myID, depth - 1, rivalAI, dir);
            }
            else tmp = SimpleSearch(gameField, myID, depth - 1, rivalAI);

			//������·�ϳԵ��˵��� ��Ϊ�з����Ȼ�ԭ��˵
			if (gameField.players[myID].strength - strength > 1
				&& gameField.players[myID].powerUpLeft - powerUpLeft != 9
				&& !gameField.pathInfo[gameField.players[myID].row][gameField.players[myID].col].isImpasse
				&& !gameField.pathInfo[gameField.players[myID].row][gameField.players[myID].col].isExit)
				tmp = tmp - (gameField.players[myID].strength - strength) * depth + 1;


			if (tmp > 0) 
                tmp += GreedyEval(gameField, myID);
            if (top
				&& tmp > 0
				&& dir == Pacman::Direction::stay
				&& !(gameField.fieldContent[gameField.players[myID].row][gameField.players[myID].col] & (Pacman::GridContentType::smallFruit | Pacman::GridContentType::largeFruit))
				&& gameField.players[myID].strength - strength == 0)
                tmp = int(tmp * (1 - float(gameField.generatorTurnLeft - 1) / gameField.GENERATOR_INTERVAL));
            if (top && !rivalFlag) 
                tmpEvals[dir + 1] = tmp;
            max = std::max(max, tmp);
			gameField.RollBack(1);

            // ��ʱ����
            if (Debug::TimeOut())
                return max;
        }
        if (top && !rivalFlag)
        {
            for (int d = 0; d < 5; d++)
            {
                Debug::debugData[Helpers::depth2String(depth)][Pacman::dirStr[d]] = to_string(tmpEvals[d]);
                if (!Debug::TimeOut())
                {
                    if (depth == DEFAULT_DEPTH || tmpEvals[d] <= DEATH_EVAL)
                        averagedEvals[d] = tmpEvals[d];
                    else
                        averagedEvals[d] = (tmpEvals[d] + averagedEvals[d]) / 2;
                }
            }
        }
        return max;
    }

    // Jet :����һ�����Ƕ��ӷֲ�������й��Ƶ�AI
    Solution GreedySearchAI(Pacman::GameField &gameField, int myID, int depth = DEFAULT_DEPTH)
    {
        int maxD = 0, max = INVALID_EVAL;

        SimpleSearch(gameField, myID, depth, NaiveThinkAI, Pacman::Direction::stay, true);

        for (int i = 0; i < 5; ++i)
        {
            if (tmpEvals[i] >= max)
            {
                max = tmpEvals[i];
                maxD = i;
            }
        }
        return std::make_pair(Pacman::Direction(maxD - 1), max);
    }

    Pacman::Direction IterativeGreedySearch(Pacman::GameField &gameField, int myID)
    {
        std::vector<Solution> solutions;
        double max = -1e+07;
        Pacman::Direction dir = {};

        for (int depth = DEFAULT_DEPTH; depth <= MAX_DEPTH; depth++)
        {
            clock_t startTime = clock();
            AI::Solution sol;
            sol = GreedySearchAI(gameField, myID, depth);
            if (Debug::TimeOut())
            {
                Debug::debugData[Helpers::depth2String(depth)]["*solution"]["notFinished"] = true;
                break;
            }
            else
                solutions.push_back(sol);
            Debug::debugData[Helpers::depth2String(depth)]["*solution"]["direction"] = Pacman::dirStr[solutions.back().first + 1];
            Debug::debugData[Helpers::depth2String(depth)]["*solution"]["maxEval"] = solutions.back().second;
            Debug::debugData[Helpers::depth2String(depth)]["*solution"]["timeCosumed"] = double(clock() - startTime) / CLOCKS_PER_SEC;
        }

        for (int i = 0; i < 5; ++i)
        {
            if (max < averagedEvals[i])
            {
                max = averagedEvals[i];
                dir = Pacman::Direction(i - 1);
            }
        }
        cout << endl;
        if (solutions.size() == 0)
        {
            Debug::debugData["*choice"]["NAIVE"] = true;
            return NaiveAttackAI(gameField, myID);
        }

        Debug::debugData["*choice"]["direction"] = Pacman::dirStr[dir + 1];
        Debug::debugData["*choice"]["finalEval"] = max;
        return dir;
    }
}

int main()
{
    auto AI = AI::IterativeGreedySearch;
    auto TAUNT = Helpers::KeepSilentMakeFortune;

    Pacman::GameField mainGameField;
    string data, globalData; // ���ǻغ�֮����Դ��ݵ���Ϣ
                             // ����ڱ��ص��ԣ���input.txt����ȡ�ļ�������Ϊ����
                             // �����ƽ̨�ϣ��򲻻�ȥ�������input.txt
#ifdef _BOTZONE_ONLINE
    Debug::presetString.clear();
#endif

    int myID = mainGameField.ReadInput("input.txt", data, globalData); // ���룬������Լ�ID
    srand(unsigned(Pacman::seed + myID));

    // �����ǰ��Ϸ����״̬�Թ����ص��ԡ�ע���ύ��ƽ̨�ϻ��Զ��Ż��������ص��ġ�
    mainGameField.DebugPrint();

#ifndef _BOTZONE_ONLINE
    Debug::startTime = clock();
    Debug::printInfo = true;
#endif

    // �������һ��Ҫ����
    Pacman::Direction choice = AI(mainGameField, myID); Debug::debugData["profiling"]["TimeUsed"] = Debug::TimeThrough();
    mainGameField.WriteOutput(choice, TAUNT(), data, globalData, Debug::debugData);

#ifndef _BOTZONE_ONLINE
    system("pause");
#endif
    return 0;
}