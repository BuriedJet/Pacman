/*
* Edited By Jet, Moriarty, weaZen
* 2016/4/29 13:43
* Naive AI Guzuta
*/

/*
* Pacman ��������
* ���ߣ�zhouhy
* ʱ�䣺2016/3/22 15:32:51
* �����£�2016/4/22 16:18
* ���������ݡ�
* �޸����ļ�������ʱ����ͨ������̨�����Bug����
* �޸ĵĲ�λ��������fstream�⡢ReadInput�ĺ�������ǰ���з����˱仯����ʹ��freopen�ˡ�
*
* ������������
*  r/R/y/Y��Row���У�������
*  c/C/x/X��Column���У�������
*  ������±궼��[y][x]��[r][c]��˳��
*  ��ұ��0123
*
* ������ϵ��
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
* ����ʾ�������ʹ��
* #ifndef _BOTZONE_ONLINE
* ������Ԥ����ָ����������������ͱ�������
*
* ����ʾ��һ����ı��༭������֧�ֽ�������۵�����
* ���������Դ�����̫���߳������Կ��ǽ�����namespace�۵�
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
#include "jsoncpp/json.h"

#define FIELD_MAX_HEIGHT 20
#define FIELD_MAX_WIDTH 20
#define MAX_GENERATOR_COUNT 4 // ÿ������1
#define MAX_PLAYER_COUNT 4
#define MAX_TURN 100
#define TIME_LIMIT 0.99
#define QUEUE_MAX 121
#define MAX_INT 0x3fffffff

//#define DEBUG

// ��Ҳ����ѡ�� using namespace std; ���ǻ���Ⱦ�����ռ�
using std::cin;
using std::cout;
using std::endl;
using std::swap;
using std::string;
using std::getline;
using std::to_string;
using std::runtime_error;

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

    const time_t seed = time(0);
    const int dx[] = {0, 1, 0, -1, 1, 1, -1, -1}, dy[] = {-1, 0, 1, 0, -1, 1, 1, -1};

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
        inline bool operator==(const FieldProp &a)
        {
            return (row == a.row && col == a.col);
        }
        inline bool operator!=(const FieldProp &a)
        {
            return (row != a.row || col != a.col);
        }
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
    private:
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
                                          // ���ѡ���Ķ���
        Direction actions[MAX_PLAYER_COUNT];

        // �ָ����ϴγ���״̬������һ·�ָ����ʼ��
        // �ָ�ʧ�ܣ�û��״̬�ɻָ�������false
        bool PopState()
        {
            if (turnID <= 0)
                return false;

            const TurnStateTransfer &bt = backtrack[--turnID];
            int i, _;

            // �������ָ�״̬

            for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
            {
                Player &_p = players[_];
                GridContentType &content = fieldContent[_p.row][_p.col];
                TurnStateTransfer::StatusChange change = bt.change[_];

                if (!_p.dead)
                {
                    // 5. �󶹻غϻָ�
                    if (_p.powerUpLeft || change & TurnStateTransfer::powerUpCancel)
                        _p.powerUpLeft++;

                    // 4. �³�����
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

                // 2. �������
                if (change & TurnStateTransfer::die)
                {
                    _p.dead = false;
                    aliveCount++;
                    content |= playerID2Mask[_];
                }

                // 1. ���λ�Ӱ
                if (!_p.dead && bt.actions[_] != stay)
                {
                    fieldContent[_p.row][_p.col] &= ~playerID2Mask[_];
                    _p.row = (_p.row - dy[bt.actions[_]] + height) % height;
                    _p.col = (_p.col - dx[bt.actions[_]] + width) % width;
                    fieldContent[_p.row][_p.col] |= playerID2Mask[_];
                }

                // 0. ���겻�Ϸ������
                if (change & TurnStateTransfer::error)
                {
                    _p.dead = false;
                    aliveCount++;
                    content |= playerID2Mask[_];
                }

                // *. �ָ�����
                if (!_p.dead)
                    _p.strength -= bt.strengthDelta[_];
            }

            // 3. �ջض���
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

            return true;
        }

        // Jet:��PopState��װ��һ�� ����һЩ
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

        // �ж�ָ�������ָ�������ƶ��ǲ��ǺϷ��ģ�û��ײǽ��û�вȵ����Ӳ�������
        inline bool ActionValid(int playerID, Direction &dir) const
        {
            if (dir == stay)
                return true;
            const Player &p = players[playerID];
            const GridStaticType &s = fieldStatic[p.row][p.col];
            return dir >= -1 && dir < 4 && !(s & direction2OpposingWall[dir]);
        }

        // ����actionsд����Ҷ�����������һ�غϾ��棬����¼֮ǰ���еĳ���״̬���ɹ��պ�ָ���
        // ���վֵĻ��ͷ���false
        bool NextTurn()
        {
            int _, i, j;

            TurnStateTransfer &bt = backtrack[turnID];
            memset(&bt, 0, sizeof(bt));

            // 0. ɱ�����Ϸ�����
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
                        // �������Լ�ǿ��׳������ǲ���ǰ����
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

            // 1. λ�ñ仯
            for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
            {
                Player &_p = players[_];
                if (_p.dead)
                    continue;

                bt.actions[_] = actions[_];

                if (actions[_] == stay)
                    continue;

                // �ƶ�
                fieldContent[_p.row][_p.col] &= ~playerID2Mask[_];
                _p.row = (_p.row + dy[actions[_]] + height) % height;
                _p.col = (_p.col + dx[actions[_]] + width) % width;
                fieldContent[_p.row][_p.col] |= playerID2Mask[_];
            }

            // 2. ��һ�Ź
            for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
            {
                Player &_p = players[_];
                if (_p.dead)
                    continue;

                // �ж��Ƿ��������һ��
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

                    // ��Щ��ҽ��ᱻɱ��
                    int lootedStrength = 0;
                    for (i = begin; i < containedCount; i++)
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

            // 3. ��������
            if (--generatorTurnLeft == 0)
            {
                generatorTurnLeft = GENERATOR_INTERVAL;
                NewFruits &fruits = newFruits[newFruitsCount++];
                fruits.newFruitCount = 0;
                for (i = 0; i < generatorCount; i++)
                    for (Direction d = up; d < 8; ++d)
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
            for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
            {
                Player &_p = players[_];
                if (_p.dead)
                    continue;

                GridContentType &content = fieldContent[_p.row][_p.col];

                // ֻ���ڸ�����ֻ���Լ���ʱ����ܳԵ�����
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

            // 5. �󶹻غϼ���
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

            // �Ƿ�ֻʣһ�ˣ�
            if (aliveCount <= 1)
            {
                for (_ = 0; _ < MAX_PLAYER_COUNT; _++)
                    if (!players[_].dead)
                    {
                        bt.strengthDelta[_] += smallFruitCount;
                        players[_].strength += smallFruitCount;
                    }
                return false;
            }

            // �Ƿ�غϳ��ޣ�
            if (turnID >= 100)
                return false;

            return true;
        }

        // ��ȡrow, colλ�õ�ˮ����ֵ
        char GetFruitValue(int row, int col)const
        {
            char v = 0;
            if (fieldContent[row][col] & smallFruit)
                v += 1;
            if (fieldContent[row][col] & largeFruit)
                v += 6;
            return v;
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
            Json::Reader reader;
            Json::Value input;
            reader.parse(str, input);

            int len = input["requests"].size();

            // ��ȡ���ؾ�̬״��
            Json::Value field = input["requests"][(Json::Value::UInt) 0],
                staticField = field["static"], // ǽ��Ͳ�����
                contentField = field["content"]; // ���Ӻ����
            height = field["height"].asInt();
            width = field["width"].asInt();
            LARGE_FRUIT_DURATION = field["LARGE_FRUIT_DURATION"].asInt();
            LARGE_FRUIT_ENHANCEMENT = field["LARGE_FRUIT_ENHANCEMENT"].asInt();
            generatorTurnLeft = GENERATOR_INTERVAL = field["GENERATOR_INTERVAL"].asInt();

            PrepareInitialField(staticField, contentField);

            // ������ʷ�ָ�����
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

        // ��ɾ��ߣ���������
        // action ��ʾ���غϵ��ƶ�����stay Ϊ���ƶ�
        // tauntText ��ʾ��Ҫ��������������������ַ�����������ʾ����Ļ�ϲ������κ����ã����ձ�ʾ������
        // data ��ʾ�Լ���洢����һ�غ�ʹ�õ����ݣ����ձ�ʾɾ��
        // globalData ��ʾ�Լ���洢���Ժ�ʹ�õ����ݣ��滻����������ݿ��Կ�Ծ�ʹ�ã���һֱ������� Bot �ϣ����ձ�ʾɾ��
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
                const Player &p = players[_];
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

    int distance[FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH][FIELD_MAX_HEIGHT][FIELD_MAX_WIDTH]{};
    int randomPlayCount = 0;
    clock_t startTime = clock();
    std::vector<string> jiangXuan = {
        /*	u8"�Ͻ���һ�� +1s",
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
        u8"I'm angry!", ��ʱ������*/
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
        "No Future For U"
    };

    inline double TimeThrough()
    {
        return double(clock() - Helpers::startTime) / CLOCKS_PER_SEC;
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


    // Jet:���������ĸ�Ч�ʱȽϸ�
    typedef std::vector<Pacman::FieldProp> Path;
    //typedef std::vector<Pacman::Direction> Path;

    // Jet: ��cc�ĸĵ�
    int Distance(const Pacman::GameField &gameField, Pacman::FieldProp startPos, Pacman::FieldProp endPos)
    {
        if (distance[startPos.row][startPos.col][endPos.row][endPos.col])
            return distance[startPos.row][startPos.col][endPos.row][endPos.col];
        if (startPos == endPos) 
            return 0;

        //��ʼ����������
        int** step = new int*[gameField.height];
        for (int i = 0; i < gameField.height; i++)
        {
            step[i] = new int[gameField.width];
            for (int j = 0; j < gameField.width; j++)
                step[i][j] = MAX_INT;
        }
        step[startPos.row][startPos.col] = 0;

        //��ʼ�����Ѷ���
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
                    if (step[newPos.row][newPos.col] > step[queue[nowFlag].row][queue[nowFlag].col] + 1) //�µĵ��Ǻõ�
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

    // Moriartycc: �μ�λ�������ȼ�
    int Distance(const Pacman::GameField &gameField, int alphaID, int betaID)
    {
        return Distance(gameField, gameField.players[alphaID], gameField.players[betaID]);
    }

    // Jet: ��д�˸�ģ��
    template <typename __Pred>
    Pacman::Direction GetTo(Pacman::GameField &gameField, int myID, __Pred pr)
    {
        Pacman::FieldProp startPos = gameField.players[myID];
        if (pr(gameField, startPos))
            return Pacman::Direction::stay;

        //��ʼ����������
        Pacman::Direction** dirInfo = new Pacman::Direction*[gameField.height];
        for (int i = 0; i < gameField.height; i++)
        {
            dirInfo[i] = new Pacman::Direction[gameField.width];
            for (int j = 0; j < gameField.width; j++)
                dirInfo[i][j] = Pacman::Direction::stay;
        }

        //��ʼ�����Ѷ���
        Pacman::FieldProp queue[QUEUE_MAX];
        queue[0] = startPos;
        int nowFlag = 0, endFlag = 0;
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
                        dirInfo[newPos.row][newPos.col] = dir;
                        queue[++endFlag] = newPos;
                        if (pr(gameField, newPos))
                        {
                            hasEaten = true;
                            break;
                        }
                    }
                }
            }
            ++nowFlag;
        }
        if (!hasEaten)
            return Pacman::Direction::ul;

        //cout << '*' << queue[endFlag].row << ' ' << queue[endFlag].col << endl;

        //����
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

        return dir;
    }

    //weaZen:����cc�Ĺ���д�˸�Ѱ�ҷ��� target��GridContentType������ ������һ�³�����//��\\)
    Pacman::Direction GetToTarget(Pacman::GameField &gameField, int myID, int target)
    {
        return GetTo(gameField, myID, 
                     [target] (const Pacman::GameField& gameField, const Pacman::FieldProp& pos) 
        { return gameField.fieldContent[pos.row][pos.col] & target; });
    }

    // Jet: û���ӳԵ�ʱ��ȥ�������Աߵ���
    Pacman::Direction GetToNearbyGenerator(Pacman::GameField &gameField, int myID)
    {
        return GetTo(gameField, myID,
                     [](const Pacman::GameField& gameField, const Pacman::FieldProp& pos)
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
        });
    }

    // weaZen:�򵥵�Σ���ж�
    bool DangerJudge(Pacman::GameField &gameField, int myID, Pacman::Direction myDir)
    {
        Pacman::Player & me = gameField.players[myID];
        for (int _ = 0; _ < 4; ++_)
        {
            if (DeltaATK(gameField, myID, _) < 0)
            {
                Pacman::Player & rival = gameField.players[_];
                int xdis = 0, ydis = 0, dis;
                if (myDir != Pacman::Direction::stay)
                {
                    xdis = (rival.row - me.row - Pacman::dy[myDir] + gameField.height) % gameField.height;
                    ydis = (rival.col - me.col - Pacman::dx[myDir] + gameField.width) % gameField.width;
                }
                else
                {
                    xdis = (rival.row - me.row + gameField.height) % gameField.height;
                    ydis = (rival.col - me.col + gameField.width) % gameField.width;
                }
                xdis = 2 * xdis < gameField.height ? xdis : gameField.height - xdis;
                ydis = 2 * ydis < gameField.width ? ydis : gameField.width - ydis;
                dis = xdis + ydis;
#ifdef DEBUG
                cout << myDir << '@' << xdis << ' ' << ydis << endl;
#endif // DEBUG
                if (dis <= 1) 
                    return true;
            }
        }
        return false;
    }

     //weaZen:����Ҹ������Եķ���(�������)
     //Jet:��Ҫ�������ֵ����������Է���AI::RandomAI()����
    Pacman::Direction SimpleRandom(Pacman::GameField &gameField, int myID)
    {
        Pacman::Direction dir;
        int vCount = 0;
        Pacman::Direction valid[5];
        for (Pacman::Direction d = Pacman::stay; d < 4; ++d)
            if (gameField.ActionValid(myID, d) && !DangerJudge(gameField, myID, d))
                valid[vCount++] = d;
#ifdef DEBUG
        cout << '*' << vCount << endl;
#endif // DEBUG
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
            // ��ÿ�������������ĺϷ�����
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

            // ����һ������仯
            // NextTurn����true��ʾ��Ϸû�н���
            bool hasNext = gameField.NextTurn();
            count++;

            if (!hasNext)
                break;
        }

        // �������
        int rank2player[] = {0, 1, 2, 3};
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

        // �ָ���Ϸ״̬�����غϳ�
        gameField.RollBack(count);

        return ((myAct + 1) << 2) + actionScore;
    }
}

namespace AI
{
    using namespace EnumExt;
    Pacman::Direction MCTS_AI(Pacman::GameField &gameField, int myID, bool noStay = false)
    {
        int actionScore[5]{};
        char tmp;
        while (Helpers::TimeThrough() <= TIME_LIMIT)
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

    //weaZen���ȳԵ�����˵���������ⱻ�˳Ե�
    Pacman::Direction NaiveAI(Pacman::GameField &gameField, int myID)
    {
        Pacman::Direction dir;
        int target = (Pacman::GridContentType::smallFruit | Pacman::GridContentType::largeFruit);
        for (int _ = 0; _ < MAX_PLAYER_COUNT; _++)
        {
            if (_ == myID)
                continue;
            if (Helpers::DeltaATK(gameField, myID, _) > 0)
                target |= Pacman::playerID2Mask[_];
        }
        dir = Helpers::GetToTarget(gameField, myID, target);
        if (dir <= Pacman::Direction::left && dir != Pacman::Direction::stay && !Helpers::DangerJudge(gameField, myID, dir))
            return dir;
        if (dir == Pacman::Direction::ul)
            return Helpers::GetToNearbyGenerator(gameField, myID);
        return MCTS_AI(gameField, myID);
        //return MCTS_AI(gameField, myID, true);
    }

    // Jet :����һ�����Ƕ��ӷֲ�������й��ƵĹ�ֵ����
    int GreedyEval(const Pacman::GameField &gameField, int myID)
    {
        int e = 0;
        for (int i = 0; i < gameField.generatorCount; i++)
            e -= Helpers::Distance(gameField, gameField.generators[i], gameField.players[myID]);
        for (int i = 0; i < MAX_PLAYER_COUNT; i++)
        {
            if (i == myID)
                continue;
            e += Helpers::Distance(gameField, myID, i) * ((Helpers::DeltaATK(gameField, myID, i) - 1) / 2);
        }

        char tmp;
        for (int i = 0; i < gameField.height; i++)
            for (int j = 0; j < gameField.width; j++)
                if ((tmp = gameField.GetFruitValue(i, j)) != 0)
                    e -= tmp * Helpers::Distance(gameField, Pacman::FieldProp(i, j), gameField.players[myID]);
        return e;
    }

    // Jet :����һ�����Ƕ��ӷֲ�������й��Ƶ�AI
    Pacman::Direction GreedyEvalAI(Pacman::GameField &gameField, int myID)
    {
        int *evals = new int[5];
        for (Pacman::Direction dir = Pacman::stay; dir <= Pacman::left; ++dir)
        {
            for (int i = 0; i < MAX_PLAYER_COUNT; i++)
                gameField.actions[i] = Pacman::stay;
            gameField.actions[myID] = dir;
            gameField.NextTurn();
            evals[dir + 1] = GreedyEval(gameField, myID);
            gameField.RollBack(1);
        }

        int maxD = - (1 << 10), d;
        for (d = 0; d < 5; d++)
            if (evals[d] > evals[maxD])
                maxD = d;

        return Pacman::Direction(maxD - 1);
    }

    Pacman::Direction JetAI(Pacman::GameField &gameField, int myID)
    {
        throw new std::exception("Not Implemented");
    }
}

int main()
{
    auto AI = AI::GreedyEvalAI;
    auto TAUNT = Helpers::MoHa;

    Pacman::GameField mainGameField;
    string data, globalData; // ���ǻغ�֮����Դ��ݵ���Ϣ
                             // ����ڱ��ص��ԣ���input.txt����ȡ�ļ�������Ϊ����
                             // �����ƽ̨�ϣ��򲻻�ȥ�������input.txt
    int myID = mainGameField.ReadInput("input.txt", data, globalData); // ���룬������Լ�ID
    srand(unsigned(Pacman::seed + myID));

    // �����ǰ��Ϸ����״̬�Թ����ص��ԡ�ע���ύ��ƽ̨�ϻ��Զ��Ż��������ص��ġ�
    mainGameField.DebugPrint();

#ifndef _BOTZONE_ONLINE
    Helpers::startTime = clock();
#endif

    // �������һ��Ҫ����
    mainGameField.WriteOutput(AI(mainGameField, myID), TAUNT(), data, globalData,
                              to_string(Helpers::randomPlayCount + Helpers::TimeThrough() - TIME_LIMIT));

    //cout << Helpers::Distance(mainGameField, 1, 1);

#ifndef _BOTZONE_ONLINE
    system("pause");
#endif
    return 0;
}
