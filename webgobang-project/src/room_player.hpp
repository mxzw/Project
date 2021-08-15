#pragma once
#include <pthread.h>
#include <jsoncpp/json/json.h>
#include <unordered_map>
#include <iostream>

using namespace std;

typedef enum PlayStatus{
    ONLINE=0, //在线状态
    MATCHING, //匹配状态
    PLAYING //游戏状态
}status_t;

class Player{
    friend class PlayerManager;
    public:
        Player()
        {}

        ~Player()
        {}

        Player(int user_id)
        {
            //在线状态，还未开始匹配
            user_id_ = user_id; 
            room_id_ = -1;
            player_status_ = ONLINE;
            chess_name_.clear();
        }

    public:
        //用户id
        int user_id_;
        //房间号
        int room_id_;
        //玩家状态
        status_t player_status_;

        //当前玩家所执棋子
        string chess_name_;
};


class PlayerManager
{
    public:
        PlayerManager()
        {
            player_map_.clear();
            pthread_mutex_init(&map_lock_,NULL);
        }
        ~PlayerManager()
        {
            pthread_mutex_destroy(&map_lock_);
        }

        /*
         * insertPlayer2Map：当用户登录之后，将登录的用户保存在map中
         */ 
        void insertPlayer2Map(int user_id)
        {
            pthread_mutex_lock(&map_lock_);
            Player p(user_id);
            player_map_.insert({user_id,p});
            pthread_mutex_unlock(&map_lock_);
        }

        /*
         * SetUserStartStatus : 设置用户的状态
         */ 
        void SetUserStartStatus(int user_id,status_t st)
        {
            player_map_[user_id].player_status_ = st;
        }

        /*
         * SetRoomID : 设置当前的房间号
         */ 
        void SetRoomID(int user_id,int room_id)
        {
            player_map_[user_id].room_id_ = room_id;
        }

        /*
         * GetPlayerInfo ： 获取玩家的信息
         */ 
        Player& GetPlayerInfo(int user_id)
        {
            return player_map_[user_id];
        }
        
        /*
         * SetUserChessName ：设置玩家当前是黑棋/白棋
         */ 
        void SetUserChessName(int user_id,const string chess_name)
        {
            player_map_[user_id].chess_name_ = chess_name;
        }

        /* 
         *  ResetUserGameInfo ： 重置当前用户状态
         */ 
        void ResetUserGameInfo(int user_id)
        {
            player_map_[user_id].room_id_ = -1;
            player_map_[user_id].chess_name_ = "";
            player_map_[user_id].player_status_ = MATCHING;
        }
        
    
    private:
        unordered_map<int,Player> player_map_;
        pthread_mutex_t map_lock_;


};

class Room
{
    public:
        Room()
        {}
        Room(int p1,int p2,int room_id) : p1_(p1),p2_(p2),room_id_(room_id)
        {
            //当前默认p1先走，即拿黑棋
            whose_turn_ = p1;
            step_num_ = 0;
            step_vec.clear();
            winner_ = -1;
        }
        ~Room()
        {}

        /*
         * IsMyTurn : 判断当前是否是自己的轮回
         */ 
        int IsMyTurn(int user_id)
        {
            return user_id == whose_turn_ ? 1:0;
        }

        /*
         * Step:将用户所走的每一步保存下来
         */ 
        int Step(int user_id,const string& body)
        {
            if(user_id != whose_turn_)
            {
                return 0;
            }

            step_vec.push_back(body);
            whose_turn_ = (user_id == p1_ ? p2_ : p1_);
            step_num_++;
            return 1;
        }

        /*
         * 获取对端的落子位置
         */ 
        int GetPeerStep(int user_id,string& s)
        {
            if(step_num_ <= 0)
                return -1;
            if(user_id != whose_turn_)
            {
                //说明对方还没有落子,则不进行获取位置直接返回
                return -2;
            }

            s = step_vec[step_num_-1];
            return 0;
        }
        /*
         * SetWinner : 设置胜者信息
         */ 
        void SetWinner(int user_id)
        {
            winner_ = user_id;
        }

        vector<string>& GetRoomStepInfo()
        {
            return step_vec;
        }
    public:
        //一个房间里有两个玩家
        int p1_;
        int p2_;

        //记录当前是谁的轮回
        int whose_turn_;
        //记录走了多少步
        int step_num_;
        //当前房间号
        int room_id_;

        //保存用户走的每一个步骤
        vector<string> step_vec;

        //保存胜者信息
        int winner_;
};

class RoomManager
{
    public:
        RoomManager()
        {
            map_room_.clear();
            pthread_mutex_init(&m_lock_,NULL);
            prepare_id_ = 1000;
        }
        ~RoomManager()
        {
            pthread_mutex_destroy(&m_lock_);
        }

        /*
         * CreateRoom ： 创建房间，为传入进来的两个user_id创建房间，并将房间号返回
         */ 
        int CreateRoom(int p1,int p2)
        {
            pthread_mutex_lock(&m_lock_);
            int room_id = prepare_id_++;
            Room r(p1,p2,room_id);
            map_room_.insert(make_pair(room_id,r));
            pthread_mutex_unlock(&m_lock_);
            return room_id;
        }
        
        /*
         * 根据传入的房间号和用户号判断当前是否是自己的轮回
         */ 
        int IsMyTurn(const int room_id, const int user_id)
        {
            return map_room_[room_id].IsMyTurn(user_id);
        }

        /*
         * Step ： 保存当前房间中用户所走的每一步
         *         并更该下一次该下棋的人数
         */ 
        int Step(const string& body)
        {
            Json::Reader r;
            Json::Value v;
            r.parse(body,v);

            int room_id = v["room_id"].asInt();
            int user_id = v["user_id"].asInt();
            
            //cout << "user_id" << user_id << endl;

            /*
             * 0 : failed
             * 1 : success 
             */ 
            return map_room_[room_id].Step(user_id,body);
        }

        /*
         * GetRoomStep ：获取房间的落子位置
         */ 
        int GetRoomStep(const string& body,string& s)
        {
            Json::Reader r;
            Json::Value v;
            r.parse(body,v);

            int room_id = v["room_id"].asInt();
            int user_id = v["user_id"].asInt();

            return map_room_[room_id].GetPeerStep(user_id,s);
            
        }

        //设置当前房间胜者
        void SetRoomWinner(const int room_id,const int user_id)
        {
            map_room_[room_id].SetWinner(user_id); 
        }
        //获取当前房间信息
        Room& GetRoomInfo(int room_id)
        {
            return map_room_[room_id];
        }

        //移除当前房间
        int RemoveRoom(int room_id)
        {
            pthread_mutex_lock(&m_lock_);
            auto it = map_room_.find(room_id);
            if(it == map_room_.end())
            {
                pthread_mutex_unlock(&m_lock_);
                return -1;
            }
            map_room_.erase(it);
            pthread_mutex_unlock(&m_lock_);
            return 0;
        }

    private:
        //为了查询方便使用unordered_map来存储当前房间信息
        unordered_map<int,Room> map_room_;
        //为了保证线程安全
        pthread_mutex_t m_lock_;

        //预分配的房间id
        int prepare_id_;

};
