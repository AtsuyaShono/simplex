//simplex
#include "simplex.hpp"

int main(int argc, char **filename){ //オプションにファイル名(lpファイル)を要求

        simplex lp;

        lp.fileload(filename[1]); //ファイルから入力
        cout << endl;
        lp.calc(); //計算

        //結果出力
        cout << "ANS" << endl;
        for(const auto& e : lp.base)
                cout << e << " = " << lp.ans[e] << endl;

        return 0;
}
