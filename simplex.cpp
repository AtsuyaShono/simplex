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
int sub; //2段階の補助変数の数カウント
bool second_step; //2段階フラグ true:2段階 false:1段階
map<string, float> obj_func; //目的関数:obj_func[基底変数] = 係数
vector<map<string, float> > restriction; //制約式:restriction[制約式の番号][基底変数] = 係数
vector<float> con_num; //定数項:con_num[制約式の番号] = 定数

public:
int slack; //スラック変数の数カウント
vector<vector <float> > tab;   //計算用配列
vector<string> base; //基底変数名
map<string, float> ans; //解:ans[変数名] = 解 ans[z] = 最適解

void fileload(char *filename); //ファイルから入力,形式はREADME.md参照
void inputTab(); //配列に入力
void printTab(); //配列を表示
void calc(); //計算 入力:2段階の1段階目の場合true 最初呼び出す場合はfalse

simplex() : sub(0),second_step(false),slack(0){
}

};

void simplex::fileload(char *filename){

        string data; //読み込み変数
        ifstream ifs(filename); //ファイルストリーム

        if(!ifs) { //開かなかった場合
                cout << "Cannot read file, please enter the \"lp\" file path in the options" << endl;
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
                                        if(!minus) obj_func[data] = coef; //係数記憶
                                        else obj_func[data] = -coef; //係数記憶
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
                                        con_num.emplace_back(atof(data.c_str())); //定数格納
                                        ++slack; //スラック変数カウント
                                        data = "s_" + to_string(slack); //スラック変数名
                                        base.emplace_back(data); //基底変数記憶
                                        //for (auto iter = mp.begin(); iter != mp.end(); iter++)
                                        //        iter->second *= -1; //符号変換
                                        mp[data] = -1; //係数格納

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
}

void simplex::inputTab(){
        tab.resize(restriction.size()+1,{}); //配列のメモリ確保

        //シンプレックスタブローに格納
        cout << "Simplex table" << endl;
        tab[0].push_back(-obj_func[base[0]]);
        for(int i = 1; i < base.size(); ++i) {
                if(obj == MAXIMIZE)
                        tab[0].push_back(-obj_func[base[i]]);
                if(obj == MINIMIZE)
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
        cout << "--------------------------------------------------------------------------------------------------------------------------------\n\n";
}

void simplex::calc(){

        if(second_step)
                cout << "2 steps" << endl;

        if(obj == MAXIMIZE)
                cout << "Object is Maximize" << endl;
        if(obj == MINIMIZE)
                cout << "Object is Minimize" << endl;

        inputTab();

        bool second_flag = false;
        if(!second_step)
                for(int i = 0; i < tab.size(); ++i)
                        for(int j = 0; j < slack; ++j)
                                if(tab[i][tab[i].size()-2-j]*tab[i][tab[i].size()-1] < 0) second_flag = true;

        if(second_flag) {
                simplex lp2;

                lp2.obj = MAXIMIZE;
                lp2.second_step = true;
                copy(restriction.begin(), restriction.end(), back_inserter(lp2.restriction));
                copy(con_num.begin(), con_num.end(), back_inserter(lp2.con_num));
                copy(base.begin(), base.end(), back_inserter(lp2.base));

                for(int i = 0; i < lp2.con_num.size(); ++i) {
                        if(lp2.con_num[i] > 0) {
                                ++lp2.sub;
                                string data = "w_" + to_string(lp2.sub);
                                lp2.obj_func[data] = -1;
                                lp2.base.emplace_back(data);
                                lp2.restriction[i-1][data] = 1;
                        }
                        if(lp2.con_num[i] < 0) {
                                ++lp2.sub;
                                string data = "w_" + to_string(lp2.sub);
                                lp2.obj_func[data] = -1;
                                lp2.base.emplace_back(data);
                                lp2.restriction[i-1][data] = -1;
                                for(int j = 0; j < lp2.base.size(); ++j)
                                        lp2.restriction[i-1][lp2.base[j]] *= -1;
                                lp2.con_num[i] *= -1;
                        }
                        lp2.con_num[0] -= lp2.con_num[i];
                }

                for(int i = 0; i < lp2.restriction.size(); ++i)
                        for(int j = 0; j < lp2.base.size(); ++j)
                                lp2.obj_func[lp2.base[j]] += lp2.restriction[i][lp2.base[j]];

                lp2.calc();

                for(int i = 1; i < tab.size(); ++i) {
                        for(int j = 1; j < tab[i].size()-1; ++j)
                                tab[i][j] = lp2.tab[i][j];
                        tab[i][tab[i].size()-1] = lp2.tab[i][lp2.tab[i].size()-1];
                }

                for(int i = 1; i < tab[0].size(); ++i) {
                        int x = -1;
                        for(int j = 1; j < tab.size(); ++j) {
                                if(tab[j][i] == 0) continue;
                                if(tab[j][i] == 1) x = j;
                                else {
                                        x = -1;
                                        break;
                                }
                        }
                        if(x > 0) {
                                float num = tab[0][i];
                                for(int j = 1; j < tab[0].size(); ++j)
                                        tab[0][j] -= num*tab[x][j];
                        }
                }

                cout << "Return to the original simplex" << endl;
                printTab();
        }

        while (1) {
                int x,y;
                float p,min;

                if(second_step)
                        if(tab[0][tab[0].size()-1] > -0.0001) break;

                // 列選択
                min = 9999;
                for (int i = 1; i < tab[0].size()-1; ++i) {
                        if (tab[0][i] <= min) {
                                min = tab[0][i];
                                y = i;
                        }
                }
                if (min >= 0) break;

                //添字係数規則
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
                        if(tab[i][tab[i].size()-1] < 0) continue;
                        float f = tab[i][tab[i].size()-1]/tab[i][y];
                        if (tab[i][y] > 0 && f < min) {
                                min = f;
                                x = i;
                        }
                }

                // ピボット係数
                p = tab[x][y];

                // ピボット係数を p で除算
                for (int i = 0; i < tab[x].size(); ++i) {
                        tab[x][i] = tab[x][i] / p;
                }

                // ピボット列の掃き出し
                for (int i = 0; i < tab.size(); i++) {
                        if (i != x) {
                                float f = tab[i][y];
                                for (int j = 0; j < tab[i].size(); ++j)
                                        tab[i][j] = tab[i][j] - f * tab[x][j];
                        }
                }

                cout << "Pivot:" << "row " << x << " column " << y << endl << endl;
                printTab();
        }

        if(!second_step) {
                for(int i = 1; i < base.size(); ++i) {
                        ans[base[i]] = 0;
                        for(int j = 1; j < tab.size(); ++j) {
                                if(tab[j][i] == 1)
                                        ans[base[i]] = tab[j][tab[j].size()-1];
                        }
                }
                ans["z"] = tab[0][tab[0].size()-1];
        }
}

int main(int argc, char **filename){ //オプションにファイル名(lpファイル)を要求

        simplex lp;

        lp.fileload(filename[1]);
        cout << endl;
        lp.calc();

        cout << "ANS" << endl;
        for(int i = 0; i < lp.base.size() - lp.slack; ++i)
                cout << lp.base[i] << " = " << lp.ans[lp.base[i]] << endl;

        return 0;
}
