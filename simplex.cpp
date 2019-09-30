//simplex

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;

#define MAXIMIZE 0 //目的が最大化
#define MINIMIZE 1 //目的が最小化

class simplex
{
int obj; //0:Maximize 1:Minimize
int slack; //スラック変数の数カウント
int sub;
map<string, float> obj_func; //目的関数:obj_func[基底変数] = 係数
vector<map<string, float> > restriction; //制約式:restriction[制約式の番号][基底変数] = 係数
vector<float> con_num; //定数項:con_num[制約式の番号] = 定数
vector<string> base; //基底変数名

public:
vector<vector <float> > tab;   //計算用配列
map<string, float> ans;

void fileload(char *filename); //ファイルから入力,形式はREADME.md参照
void selfinput(); //手動入力
void inputTab(); //配列に入力
void printTab(); //配列を表示
void calc(bool sec_stage); //計算 入力:2段階の1段階目の場合true 最初呼び出す場合はfalse

simplex() : slack(0),sub(0){

}

};

void simplex::fileload(char *filename){

        string data; //読み込み変数
        ifstream ifs(filename); //ファイルストリーム

        if(!ifs) { //開かなかった場合
                cout << "Cannot read file!" << endl;
                exit(0);
        }

        getline(ifs, data); //1行読み込み
        //目的確認
        if(data == "Maximize") obj = MAXIMIZE;
        else if(data == "Minimize") obj = MINIMIZE;
        else {
                cout << "Unknown object!" << endl;
                exit(0);
        }

        while(1) {
                getline(ifs, data); //1行読み込み
                istringstream stream(data); //stringストリーム

                stream >> data; //空白行ごとに読み込み
                if(data == "End") break; //終了
                if(data == "obj:") { //目的関数読み込み
                        bool minus = false; //マイナスフラグ
                        float coef = 1; //係数

                        base.emplace_back("z"); //目的関数を”z”と置く
                        obj_func["z"] = 1; //係数は1
                        con_num.emplace_back(0); //定数は0
                        while(stream >> data) { //行末まで空白行ごとに読み込み
                                if(data == "+") minus = false; //"+"だからマイナスフラグ初期化
                                else if(data == "-") minus = true; //"-"だからフラグたて
                                else if(data.find("x") != string::npos) { //変数は"x"が含まれていないといけない。
                                        //見つけたらマイナスフラグを考慮して保存済の係数を格納
                                        base.emplace_back(data); //基底記憶
                                        if(obj == MAXIMIZE) { //目的が最大化のとき
                                                if(!minus) obj_func[data] = -coef;
                                                else obj_func[data] = coef;
                                        }
                                        if(obj == MINIMIZE) { //目的が最小化のとき
                                                if(!minus) obj_func[data] = coef;
                                                else obj_func[data] = -coef;
                                        }
                                        coef = 1; //係数初期化
                                }
                                else coef = atof(data.c_str()); //係数記憶
                        }
                }
                if(data.find("r") != string::npos) { //制約式の前には"r"が含まれた文字列を置かなければならない
                        bool minus = false; //マイナスフラグ
                        float coef = 1; //係数
                        map<string, float> mp; //記憶用map
                        while(stream >> data) { //行末まで空白行ごとに読み込み
                                if(data == "+") minus = false; //"+"だからマイナスフラグ初期化
                                else if(data == "-") minus = true; //"-"だからフラグたて
                                else if(data == "<=") { //不等号の場合
                                        stream >> data; //必ず定数を読み込むので記憶
                                        con_num.emplace_back(atof(data.c_str())); //定数格納
                                        ++slack; //スラック変数カウント
                                        data = "s_" + to_string(slack); //スラック変数名
                                        base.emplace_back(data); //基底変数記憶
                                        mp[data] = 1; //係数格納
                                }
                                else if(data == ">=") { //不等号の場合
                                        stream >> data; //必ず定数を読み込むので記憶
                                        con_num.emplace_back(-atof(data.c_str())); //定数格納
                                        ++slack; //スラック変数カウント
                                        data = "s_" + to_string(slack); //スラック変数名
                                        base.emplace_back(data); //基底変数記憶
                                        for (auto iter = mp.begin(); iter != mp.end(); iter++)
                                                iter->second *= -1; //符号変換
                                        mp[data] = 1; //係数格納

                                }
                                else if(data == "=") { //等号の場合
                                        stream >> data; //必ず定数を読み込むので記憶
                                        con_num.emplace_back(atof(data.c_str())); //定数格納
                                }
                                else if(data.find("x") != string::npos) { //変数が現れたとき
                                        if(find(base.begin(), base.end(), data) == base.end())
                                                base.emplace_back(data); //基底変数記憶
                                        //正負を判断して係数格納
                                        if(!minus) mp[data] = coef;
                                        else mp[data] = -coef;
                                        coef = 1;
                                }
                                else coef = atof(data.c_str()); //係数記憶
                        }
                        restriction.emplace_back(mp); //読み込んだmapを格納
                }

        }
        //sort(base.begin(),base.end(),greater<string>()); //文字列をsort
        //base.erase(unique(base.begin(), base.end()), base.end()); //重複して記憶してしまっている基底変数を消去

}

void simplex::selfinput(){


}

void simplex::inputTab(){
        tab.resize(restriction.size()+1,{}); //配列のメモリ確保

        //シンプレックスタブローに格納
        for(int i = 0; i < base.size(); ++i) {
                tab[0].push_back(obj_func[base[i]]);
        }
        tab[0].push_back(con_num[0]);

        for(int i = 0; i < restriction.size(); ++i) {
                for(int j = 0; j < base.size(); ++j) {
                        tab[i+1].push_back(restriction[i][base[j]]);
                }
                tab[i+1].push_back(con_num[i+1]);
        }

        printTab(); //出力
}

void simplex::printTab(){

        cout<< fixed << setprecision(3);

        if(obj == MAXIMIZE)
                cout << "Object is Maximize" << endl;
        if(obj == MINIMIZE)
                cout << "Object is Minimize" << endl;

        for(int i = 0; i < base.size(); ++i) {
                cout << " " << setw(8) << base[i];
        }
        cout << endl;

        for(int i = 0; i < tab.size(); ++i) {
                for(int j = 0; j < tab[i].size(); ++j) {
                        cout << " " << setw(8) << tab[i][j];
                }
                cout << endl;
        }
        cout << endl;
}

void simplex::calc(bool sec_stage){

        inputTab();

        bool second_flag = false;
        if(!sec_stage)
                for(int i = 0; i < con_num.size(); ++i)
                        if(con_num[i] < 0) second_flag = true;

        if(second_flag) {
                simplex lp2;

                lp2.obj = MAXIMIZE;
                copy(restriction.begin(), restriction.end(), back_inserter(lp2.restriction));
                copy(con_num.begin(), con_num.end(), back_inserter(lp2.con_num));
                copy(base.begin(), base.end(), back_inserter(lp2.base));

                for(int i = 0; i < lp2.con_num.size(); ++i) {
                        if(lp2.con_num[i] > 0) {
                                ++lp2.sub;
                                string data = "w_" + to_string(lp2.sub);
                                lp2.obj_func[data] = 1;
                                lp2.base.emplace_back(data);
                                lp2.restriction[i-1][data] = 1;
                        }
                        if(lp2.con_num[i] < 0) {
                                ++lp2.sub;
                                string data = "w_" + to_string(lp2.sub);
                                lp2.obj_func[data] = 1;
                                lp2.base.emplace_back(data);
                                lp2.restriction[i-1][data] = -1;
                                for(int j = 0; j < lp2.base.size(); ++j) {
                                        lp2.restriction[i-1][lp2.base[j]] *= -1;
                                }
                                lp2.con_num[i] *= -1;
                        }
                        lp2.con_num[0] -= lp2.con_num[i];
                }

                for(int i = 0; i < lp2.restriction.size(); ++i) {
                        for(int j = 0; j < lp2.base.size(); ++j) {
                                lp2.obj_func[lp2.base[j]] -= lp2.restriction[i][lp2.base[j]];
                        }
                }

                lp2.calc(true);

                for(int i = 1; i < tab.size(); ++i) {
                        for(int j = 1; j < tab[i].size()-1; ++j) {
                                tab[i][j] = lp2.tab[i][j];
                        }
                        tab[i][tab[i].size()-1] = lp2.tab[i][lp2.tab[i].size()-1];
                }

                for(int i = 1; i < tab[0].size()-1; ++i) {
                        if(tab[0][i] != 0) {
                                int num = tab[0][i];
                                tab[0][i] = 0;
                                for(int j = 0; j < tab.size(); ++j) {
                                        if(tab[j][i] == 1) {
                                                for(int k = 0; k < tab[j].size()-1; ++k) {
                                                        if(!(k == i))
                                                                tab[0][k] += num*tab[j][k];
                                                }
                                                tab[0][tab[j].size()-1] -= num*tab[j][tab[j].size()-1];
                                        }
                                }
                        }
                }
                printTab();
        }

        while (1) {
                int x,y;
                float p,min;

                // 列選択
                min = 9999;
                for (int i = 1; i < tab[0].size()-1; ++i) {
                        if (tab[0][i] < min) {
                                min = tab[0][i];
                                y = i;
                        }
                }
                if (min >= 0) break;

                //bool flag = true;
                //for (int i = 1; i < tab[0].size()-1; ++i) {
                //        if (tab[0][i] < 0) {
                //                y = i;
                //                flag = false;
                //        }
                //}
                //if(flag) break;

                // 行選択
                min = 9999;
                for (int i = 1; i < tab.size(); ++i) {
                        //if(tab[i][tab[i].size()-1] < 0) continue;
                        float f = tab[i][tab[i].size()-1]/tab[i][y];
                        if (tab[i][y] > 0 && f < min) {
                                min = f;
                                x = i;
                        }
                }

                // ピボット係数
                p = tab[x][y];

                // ピボット係数を p で除算
                for (int i = 0; i < tab[x].size(); ++i)
                        tab[x][i] = tab[x][i] / p;

                // ピボット列の掃き出し
                for (int i = 0; i < tab.size(); i++) {
                        if (i != x) {
                                float f = tab[i][y];
                                for (int j = 0; j < tab[i].size(); ++j)
                                        tab[i][j] = tab[i][j] - f * tab[x][j];
                        }
                }

                if(sec_stage) cout << "2nd stage" << endl;
                cout << "Pivot:" << "row " << x << " column " << y << endl;
                printTab();

        }

}

int main(int argc, char **filename){ //オプションにファイル名(lpファイル)を要求

        simplex lp;

        lp.fileload(filename[1]);
        lp.calc(false);

        return 0;
}
