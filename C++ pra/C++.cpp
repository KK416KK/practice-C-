#include<sqlite3.h>
#include<iostream>
#include <regex>
#include <limits>


void adddata(sqlite3* DB);
void serch(sqlite3* DB);
int main(){
    sqlite3* DB;
    char* messageError;
    int exit = 0;

    //データベースを開く
    exit = sqlite3_open("practice.db",&DB);

    //データベース接続に失敗したとき
    if(exit){
        std::cerr << "データベースの接続に失敗:" << sqlite3_errmsg(DB) << std::endl;
        return -1;
    }

    //データベースを作成するための文
    std::string sql = "CREATE TABLE IF NOT EXISTS PRODUCT("
    "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
    "NAME TEXT NOT NULL, "
    "ACCITEM TEXT NOT NULL, "
    "COMPANY TEXT NOT NULL, "
    "DATE TEXT NOT NULL, "
    "PRICE REAL NOT NULL, "
    "NUMBER INT NOT NULL, "
    "INCOREXP INTEGER NOT NULL);";

    exit = sqlite3_exec(DB,sql.c_str(),NULL,0,&messageError);
    if (exit != SQLITE_OK){
        std::cerr << "データベースの作成に失敗" << messageError << std::endl;
        sqlite3_free(messageError);
    }else{
        //データベース作成成功
    }
    //メニュー画面
    while (true)
    {
        std::cout << "アプリメニュー\n";

        std::cout << "1: データを追加\n";
        std::cout << "2: 全データを表示\n";
        std::cout << "3: データ検索\n";
        std::cout << "4: データの変更\n";
        std::cout << "5: 保存して終了\n";

        std::cout << "選択:";

        //数字によって処理を分ける
        int choice;
        std::cin >> choice;
        switch(choice){
            case 1:
                adddata(DB);
                break;
            case 2:
                alldata(DB); 
                break;
            case 3:
                serch(DB);
                break;
            case 4:
                update(DB);
                break;
            case 5:
                std::cout << "保存して終了します。" << std::endl;
                 sqlite3_close(DB);
                return 0;
            default:
                //1~5以外を入力したとき
                break;
        }
    }
}

void update(sqlite3* DB){
    char* messageError;
    int ids;
    std::cout << "変更したいデータのIDを入力してください:" << std::endl;
    std::cin >> ids;
    std::cout << "変更したいフィールドを入力してください" << std::endl;
    while(true){
        int field;
        std::cout << "1: 品目" << std::endl;
        std::cout << "2: 勘定科目" << std::endl;
        std::cout << "3: 取引相手の会社名" << std::endl;
        std::cout << "4: 取引の日付(書式:YYYY-MM-DD)" << std::endl;
        std::cout << "5: １つ分の価格" << std::endl;
        std::cout << "6: 個数" << std::endl;
        std::cout << "7: 出庫か入庫か（出庫の場合:0入庫の場合:1）" << std::endl;
        std::cout << "8: キャンセル" << std::endl;
        std::cin >> field;

        if (field == 8) {
            std::cout << "操作を終了しました。" << std::endl;
            break;
        }

        std::string fieldName;
        std::string newValue;
        //どのカラムを選択したか
        switch (field) {
            case 1:
                fieldName = "NAME";
                break;
            case 2:
                fieldName = "ACCITEM";
                break;
            case 3:
                fieldName = "COMPANY";
                break;
            case 4:
                fieldName = "DATE";
                break;
            case 5:
                fieldName = "PRICE";
                break;
            case 6:
                fieldName = "NUMBER";
                break;
            case 7:
                fieldName = "INCOREXP";
                break;
            default:
                std::cout << "無効な選択です。" << std::endl;
                return;
        }
        std::cout << "新しい値を入力してください:" << std::endl;
        std::cin >> newValue;

        //データベース用に変更する
        if (field == 5 || field == 6 || field == 7) {
            try {
                std::stoi(newValue); //数値に変換
            } catch (...) {
                std::cerr << "無効な値が入力されました。もう一度試してください。" << std::endl;
                continue;
            }
        } else {
            newValue = "'" + newValue + "'"; //文字列はシングルクォーテーションで囲む
        }

        //フィールドの変更を実行する
        std::string sql = "UPDATE PRODUCT SET " + fieldName + " = " + newValue + " WHERE ID = " + std::to_string(ids) + ";";
        int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "データの変更に失敗: " << messageError << std::endl;
            sqlite3_free(messageError);
        } else {
            std::cout << "データが正常に変更されました。" << std::endl;
        }
        std::cout << "他に変更するフィールドはありますか？" << std::endl;
    }
}

void alldata(sqlite3* DB){
    char* messageError;

    //すべての行のIDと品目を表示する
    const char* sql = "SELECT ID, NAME FROM PRODUCT;";

    int exit = sqlite3_exec(DB, sql, callback, nullptr, &messageError); 
    if (exit != SQLITE_OK){
        std::cerr << "クエリ作成に失敗: " << messageError << std::endl;
        sqlite3_free(messageError);
    }else{
    }
}

void adddata(sqlite3* DB){
    char* messageError;

    std::string name;
    std::string accitem;
    std::string company;
    std::string date;
    int price = 0;
    int number = 0;
    int POM = -1;

    //データの入力
    std::cout << "品目を入力してください" << std::endl;
    std::cin >> name;
    std::cout << "勘定科目を入力してください" << std::endl;
    std::cin >> accitem;
    std::cout << "取引相手の会社名を入力してください" << std::endl;
    std::cin >> company;
    do {
        std::cout << "取引の日付を入力してください(書式:YYYY-MM-DD)" << std::endl;
        std::cin >> date;
        //書式があっているか確認
        if (!isValidDate(date)) {
            std::cout << "日付の形式が無効です。もう一度入力してください。" << std::endl;
        }
    } while (!isValidDate(date));
    std::cout << "１つ分の価格を入力してください" << std::endl;
    while (!(std::cin >> price)) {
        //数値が入力されているか
        std::cout << "無効な入力です。数値を入力してください。" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cout << "個数を入力してください" << std::endl;
    while (!(std::cin >> number)) {
        std::cout << "無効な入力です。数値を入力してください。" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "出庫の場合:0入庫の場合:1を入力してください" << std::endl;
    std::cin >> POM;
    while (!(std::cin >> POM) || (POM != 0 && POM != 1)) {
        std::cout << "無効な入力です。出庫の場合:0 入庫の場合:1 を入力してください。" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::string sql = "INSERT INTO PRODUCT (NAME, ACCITEM, COMPANY, DATE, PRICE, NUMBER, INCOREXP) VALUES('" + 
    name + "', '" + accitem + "', '" + company + "', '" + date + "', " + std::to_string(price) + ", " + std::to_string(number) + ", " + std::to_string(POM) + ");";

    //実行
    int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "データ挿入に失敗: " << messageError << std::endl;
        sqlite3_free(messageError);
    } else {
        std::cout << "データが正常に追加されました。" << std::endl;
    }
}

void serch(sqlite3* DB){
    char* messageError;

    std::cout << "何で検索しますか？\n";

    std::cout << "1: 品目\n";
    std::cout << "2: 年ごと\n";
    std::cout << "3: 月ごと\n";
    std::cout << "4: 勘定科目\n";
    std::cout << "5: 相手の会社名\n";

    int num;    
    std::cin >> num;

    std::string que;

    //年、月、それ以外で分ける
    switch (num)
    {
    case 2:
        std::cout << "検索(書式:YYYY): ";
        std::cin >> que;
        break;
    case 3:
        std::cout << "検索(書式:YYYY-MM): ";
        std::cin >> que;
        break;
    case 1:
    case 4:
    case 5:
        std::cout << "検索ワード: ";
        std::cin >> que;
        break;
    default:
        break;
    }

    std::string wheres;
    switch (num)
    {
    case 1:
        wheres = "SELECT * FROM PRODUCT WHERE NAME = '" + que + "';";
        break;
    case 2:
        wheres = "SELECT * FROM PRODUCT WHERE DATE LIKE '" + que + "%';";
        break;
    case 3:
        wheres = "SELECT * FROM PRODUCT WHERE DATE LIKE '" + que + "%';";
        break;
    case 4:
        wheres = "SELECT * FROM PRODUCT WHERE ACCITEM = '" + que + "';";
        break;
    case 5:
        wheres = "SELECT * FROM PRODUCT WHERE COMPANY = '" + que + "';";
        break;
    default:
        return;
        break;
    }
    
    int exit = sqlite3_exec(DB, wheres.c_str(), callback, 0, &messageError);
    if (exit != SQLITE_OK){
        std::cerr << "クエリ作成に失敗: " << messageError << std::endl;
        sqlite3_free(messageError);
    }else{
    }

    return;
}

bool isValidDate(const std::string& date) {
    // 正規表現を使用してYYYY-MM-DD形式を確認
    std::regex datePattern(R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$)");
    return std::regex_match(date, datePattern);
}

static int callback(void* NotUused,int argc, char** argv, char** azColName){
    for (int i = 0; i < argc; i++){
        std::cout << azColName[i] << ": " << argv[i] << std::endl;
    }
    std::cout << std::endl;
    return 0;
}