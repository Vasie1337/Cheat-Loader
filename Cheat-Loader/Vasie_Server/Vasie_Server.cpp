#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <iomanip>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)

using json = nlohmann::json;

typedef std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, int>>> _licenses;

int port_panel = 8357;
int port_login = 1957;

const std::string licenses_file = "licenses.json";

unsigned char bytes[] = {
    0x4D, 0x64
};
namespace file
{
    _licenses get()
    {
        _licenses list;

        json j;
        std::ifstream file(licenses_file);
        file >> j;
        file.close();

        for (auto& license : j["licenses"])
        {
            std::string key = license["key"];
            std::string hwid = license["hwid"];
            std::string length = license["length"];
            int timestamp = license["timestamp"];

            list.push_back(std::make_pair(std::make_pair(key, hwid), std::make_pair(length, timestamp)));
        }

        return list;
    }
    void update_timestamp(const std::string& license_key, const int seconds)
    {
        std::ifstream file(licenses_file);
        json licenses;
        file >> licenses;

        for (auto& license : licenses["licenses"]) {
            if (license["key"] == license_key)
            {
                license["timestamp"] = time(nullptr) + seconds;
                break;
            }
        }

        std::ofstream outfile(licenses_file);
        outfile << licenses.dump(4) << std::endl;
    }
    void update_hwid(const std::string& license_key, const std::string& hwid)
    {
        std::ifstream file(licenses_file);
        json licenses;
        file >> licenses;

        for (auto& license : licenses["licenses"]) {
            if (license["key"] == license_key)
            {
                license["hwid"] = hwid;
                break;
            }
        }

        std::ofstream outfile(licenses_file);
        outfile << licenses.dump(4) << std::endl;
    }
    void delete_license(const std::string& license_key)
    {
        json j;
        std::ifstream file(licenses_file);
        if (file.is_open()) {
            file >> j;
            file.close();
        }

        auto& licenses = j["licenses"];
        for (auto it = licenses.begin(); it != licenses.end(); ++it) {
            if ((*it)["key"] == license_key) {
                licenses.erase(it);
                break;
            }
        }

        std::ofstream outfile(licenses_file);
        if (outfile.is_open()) {
            outfile << j.dump(4);
            outfile.close();
        }
    }
    void create_license(const std::string& key, const std::string& length)
    {
        std::ifstream ifs(licenses_file);
        json j;
        ifs >> j;

        json new_license = {
            {"hwid", ""},
            {"key", key},
            {"length", length},
            {"timestamp", 0}
        };

        j["licenses"].push_back(new_license);

        std::ofstream ofs(licenses_file);
        ofs << std::setw(4) << j << std::endl;
    }
    void pull_license(int client_fd)
    {
        _licenses list = file::get();

        std::string message = "";

        for (int i = 0; i < list.size(); i++)
        {
            std::string license = list[i].first.first + ':' + list[i].first.second + ':' + list[i].second.first + ':' + std::to_string(list[i].second.second);
            message += license + "|";
        }

        send(client_fd, message.c_str(), strlen(message.c_str()), 0);
    }
    void delete_all()
    {
        _licenses list = get();
        for (int i = 0; i < list.size(); i++)
        {
            delete_license(list[i].first.first);
        }
    }
}

void xor_array(unsigned char* data, int size)
{
    for (int i = 0; i < size; i++)
    {
        data[i] ^= "CP6WBNPF"[i % sizeof("CP6WBNPF")];
    }
}

void panel_thread()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_panel);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    while (true)
    {
        int client_fd = accept(server_fd, nullptr, nullptr);

        char buffer[256];
        int n = recv(client_fd, buffer, sizeof(buffer), 0);
        std::string received(buffer, n);

        size_t pos = received.find(':');
        size_t _pos = received.find(';');
        if (pos == std::string::npos || _pos == std::string::npos)
        {
            continue;
        }

        std::string type = received.substr(0, pos);
        std::string license = received.substr(pos + 1, _pos - (pos + 1));
        std::string length = received.substr(_pos + 1);

        if (type == "create")
        {
            file::create_license(license, length);
        }
        else if (type == "delete")
        {
            file::delete_license(license);
        }
        else if (type == "pull")
        {
            file::pull_license(client_fd);
        }
        else if (type == "deleteall")
        {
            file::delete_all();
        }
        else
        {

            continue;
        }

        close(client_fd);
    }
}

void login_thread()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_login);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    while (true)
    {
        int client_fd = accept(server_fd, nullptr, nullptr);

        char buffer[256];
        int n = recv(client_fd, buffer, sizeof(buffer), 0);
        std::string received(buffer, n);

        size_t pos = received.find(':');
        if (pos == std::string::npos)
            continue;

        size_t _pos = received.find(';');
        if (_pos == std::string::npos)
            continue;

        std::string license = received.substr(0, pos);
        std::string hwid = received.substr(pos + 1, _pos - pos - 1);
        std::string version = received.substr(_pos + 1);

        _licenses list = file::get();

        for (const auto& item : list)
        {
            if (item.first.first != license)
                continue;

            if (item.first.second.empty())
            {
                file::update_hwid(item.first.first, hwid);
                if (item.second.first == "day")
                    file::update_timestamp(item.first.first, 86400);
                else if (item.second.first == "month")
                    file::update_timestamp(item.first.first, 2592000);
                else
                    continue;


                send(client_fd, static_cast<void*>(bytes), sizeof(bytes), 0);
                break;
            }

            if (item.first.second != hwid)
                continue;

            send(client_fd, static_cast<void*>(bytes), sizeof(bytes), 0);
        }

        close(client_fd);

    }

}

void expire_thread()
{
    while (true)
    {
        _licenses licenses = file::get();

        for (const auto& item : licenses)
        {
            if (item.second.second <= time(nullptr) && !item.first.second.empty())
            {
                file::delete_license(item.first.first);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    xor_array(bytes, sizeof(bytes));
    std::thread login(login_thread); login.detach();
    std::thread panel(panel_thread); panel.detach();
    std::thread expire(expire_thread); expire.detach();

    while (true) {}
}