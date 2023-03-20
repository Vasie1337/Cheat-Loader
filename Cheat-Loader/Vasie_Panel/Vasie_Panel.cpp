#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <atlsecurity.h> 
#include <vector>
#include <sstream>
#include <iomanip>

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)

int port_panel = 8357;

const char* addres = "[YOUR_IP]";

typedef std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, int>>> _licenses;

void Create(std::string license, std::string length)
{
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_panel);
    inet_pton(AF_INET, addres, &server_addr.sin_addr);

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    std::string type = "create";

    std::string message = type + ":" + license + ";" + length;
    send(sock, message.c_str(), strlen(message.c_str()), 0);

    closesocket(sock);
    WSACleanup();
}

void Delete(std::string license)
{
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_panel);
    inet_pton(AF_INET, addres, &server_addr.sin_addr);

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    std::string type = "delete";
    std::string length = "0";

    std::string message = type + ":" + license + ";" + length;
    send(sock, message.c_str(), strlen(message.c_str()), 0);

    closesocket(sock);
    WSACleanup();
}

void DeleteAll()
{
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_panel);
    inet_pton(AF_INET, addres, &server_addr.sin_addr);

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    std::string type = "deleteall";
    std::string license = "0";
    std::string length = "0";

    std::string message = type + ":" + license + ";" + length;
    send(sock, message.c_str(), strlen(message.c_str()), 0);

    closesocket(sock);
    WSACleanup();
}

std::vector<std::string> split(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

void Pull()
{
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_panel);
    inet_pton(AF_INET, addres, &server_addr.sin_addr);

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    std::string message = "pull:0;0";
    send(sock, message.c_str(), strlen(message.c_str()), 0);

    char buffer[1024];
    int bytes_received;

    bytes_received = recv(sock, buffer, sizeof(buffer), 0);

    // Split the received string into individual licenses using the delimiter character '|'
    std::string received_str(buffer, bytes_received);
    std::vector<std::string> licenses = split(received_str, '|');

    std::cout << "----------------------------------------------" << std::endl;
    // Process each license
    for (auto& license_str : licenses) {
        std::vector<std::string> fields = split(license_str, ':');
        std::string key = fields[0];
        std::string hwid = fields[1];
        std::string length = fields[2];
        std::time_t timestamp = std::stoi(fields[3]);

        std::time_t now = std::time(nullptr);
        std::time_t diff = timestamp - now;

        int days = diff / 86400;
        int hours = (diff % 86400) / 3600;
        int minutes = (diff % 3600) / 60;
        int seconds = diff % 60;

        std::cout << "License key: " << key << std::endl;
        std::cout << "HWID: " << hwid << std::endl;
        std::cout << "Length: " << length << std::endl;
        std::cout << "Time left: ";
        if (days > 0) {
            std::cout << days << " day(s) ";
        }
        std::cout << std::setfill('0') << std::setw(2) << hours << ":"
            << std::setfill('0') << std::setw(2) << minutes << ":"
            << std::setfill('0') << std::setw(2) << seconds << std::endl;

        std::cout << "----------------------------------------------" << std::endl;
    }

    closesocket(sock);
    WSACleanup();
}

std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

int main()
{
    srand((unsigned)time(NULL) * getpid());

    while (true)
    {
        system("cls");
        std::cout << "Current licenses :\n" << std::endl;
        Pull();


        std::cout << "[1] : Create license [2] : Delete license [3] : Delete all" << std::endl;
        int option{};
        std::cin >> option;
        switch (option)
        {
        case 1:
        {
            std::string out = gen_random(12);
            Create(out, "month");
            std::cout << "Created license : " << out << "." << std::endl;
            break;
        }
        case 2:
        {
            std::string in;
            std::cin >> in;
            Delete(in);
            std::cout << "Deleted license : " << in << "." << std::endl;
            break;
        }
        case 3:
        {
            DeleteAll();
            break;
        }
        default:
        {
            std::cout << "Invalid option." << std::endl;
            break;
        }
        }
        Sleep(1000);

    }
}