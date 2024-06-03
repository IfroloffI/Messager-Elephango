//*conninfo= "dbname=messenger user=postgres password=rivvaste host addr=127.0.0.1/32 port=5432";

#ifndef PROJECT_MESSENGER_DB_H
#define PROJECT_MESSENGER_DB_H
#include "libpq-fe.h"
#include <iostream>
#include <string>
#include <cstring>

class messenger_db {
private:
    PGconn *conn;

    void CheckConnStatus()
    {
        if(PQstatus(conn)!=CONNECTION_OK){
            std::cerr << "connection to database failed: "<<PQerrorMessage(conn)<<"\n";
            PQfinish(conn);
            exit(1);
        }
    }

    void executeQuery(const char *query){
        PGresult *res= PQexec(conn,query);
        if(PQresultStatus(res)!=PGRES_COMMAND_OK && PQresultStatus(res)!=PGRES_TUPLES_OK){
            std::cerr<<"Query execution failed: "<<PQerrorMessage(conn)<<"\n";
            PQclear(res);
            PQfinish(conn);
            exit(1);
        }
        PQclear(res);
    }
public:
    messenger_db(const std::string& conninfo){
        conn= PQconnectdb(conninfo.c_str());
        CheckConnStatus();
    }
    ~messenger_db(){
        PQfinish(conn);
    }

    int registerUser(const std::string& username, const std::string& password) {
        std::string query = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "') RETURNING id;";
        PGresult *res = PQexec(conn, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::cerr << "Registration failed: " << PQerrorMessage(conn) << std::endl;
            PQclear(res);
            return -1;
        }
        int userId = atoi(PQgetvalue(res, 0, 0));
        PQclear(res);
        return userId;
    }

    int userExists(const std::string& username) {
        std::string query = "SELECT EXISTS(SELECT 1 FROM users WHERE username = '" + username + "');";
        PGresult *res = PQexec(conn, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::cerr << "User check failed: " << PQerrorMessage(conn) << std::endl;
            PQclear(res);
            return -1;
        }
        int id = (PQntuples(res) > 0) ? atoi(PQgetvalue(res, 0, 0)) : -1;
        PQclear(res);
        return id;
    }

    void sendMessage(int senderId, int receiverId, const std::string& content) {
        std::string query = "INSERT INTO messages (sender_id, receiver_id, content) VALUES (" + std::to_string(senderId) + ", " + std::to_string(receiverId) + ", '" + content + "');";
        executeQuery(query.c_str());
    }
    void getMessages(int userId) {
        std::string query = "SELECT u.username, m.content, m.timestamp FROM messages m JOIN users u ON m.sender_id = u.id WHERE m.receiver_id = " + std::to_string(userId) + " ORDER BY m.timestamp;";
        PGresult *res = PQexec(conn, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::cerr << "Failed to retrieve messages: " << PQerrorMessage(conn) << std::endl;
            PQclear(res);
            return;
        }

/*        int rows = PQntuples(res);
        for (int i = 0; i < rows; ++i) {
            std::cout << "From: " << PQgetvalue(res, i, 0) << " - " << PQgetvalue(res, i, 1) << " at " << PQgetvalue(res, i, 2) << std::endl;
        }*/

        PQclear(res);
    }
};


#endif //PROJECT_MESSENGER_DB_H
