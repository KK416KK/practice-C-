#include<sqlite3.h>
#include<iostream>

int main(){
    sqlite3* DB;
    char* messageError;
    int exit = 0;

    exit = sqlite3_open("practice.db",&DB);

    if(exit){
        std::cerr << "データベースの接続に失敗:" << sqlite3_errmsg(DB) << std::endl;
        return -1;
    }

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
}