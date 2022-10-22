#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <ctime>
#include <vector>
#include <locale>
#include <codecvt>
//#include <UTF8-CPP.h>

#include <curl.h>


#pragma comment(lib,"libcurl.lib")

using namespace std;

struct work_date {
    int day = 0;
    int mon = 0;
    int year = 0;
};

class HTML_reader {
public:

    string out_str(const string& adres, const int line_n) {

        string readBuffer_UTF8;

        CURL* curl;
        CURLcode res_inter;
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, adres.data());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer_UTF8);//
            res_inter = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        else {
            //ERROR!!!
        }
        stringstream stream_to_need_line;

        stream_to_need_line << readBuffer_UTF8;
        int count = 0;
        string ssbufer;
        if (res_inter != 0) {
            return "0";
        }
        while (getline(stream_to_need_line, ssbufer)) {
            count++;
            if (count == line_n) {
                break;
            }
        }



        return ssbufer;
    }

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
};

class Str_Parser {
public:

    Str_Parser(const string& str)
        :working_line(str)
    {
        set_local_data();
        parser();
    }

    void give_str(const string& str) {
        working_line = str;
    }

    void parser() {
        if (working_line.empty()) {
            cout << "String is empty.";
        }
        else {
            size_t count = 0;
            while (count < 7) {
                int pos = working_line.rfind("pof.php?dt=");
                if (pos == -1) {
                    break;
                }
                pair tmp = short_pars(working_line.substr(pos + 11, 17));
                if (tmp.first.year == date.year) { // ПРОВЕРКА НА ДАТУ
                    if (tmp.first.mon == date.mon) {
                        if (tmp.first.day >= date.day) {
                            count++;
                            base.push_back(tmp);
                        }
                    }
                    else if(tmp.first.mon > date.mon){
                        count++;
                        base.push_back(tmp);
                    }
                }
                else if (tmp.first.year > date.year) {
                    count++;
                    base.push_back(tmp);
                }
                working_line.erase(pos, working_line.length() - pos - 1);
            }
        }
    }

    vector<pair<work_date, string>> get_base() {
        return base;
    }

private:
    pair<work_date, string> short_pars(const string& str) {
        string date_str = str.substr(str.find_first_of('>') + 1, str.length() - 1);
        work_date date;
        date.day = stoi(date_str.substr(0, 2));
        date.mon = stoi(date_str.substr(3, 2));
        date.year = stoi(date_str.substr(6, 4));
        auto tmp_wstr = str.substr(0, str.find_first_of('"'));
        return { date, {tmp_wstr.begin(), tmp_wstr.end() } };
    }

    void set_local_data() {
        time_t rawtime;
        struct tm* timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        date.day = timeinfo->tm_mday;
        date.mon = timeinfo->tm_mon + 1;
        date.year = timeinfo->tm_year - 100 + 2000;
    }

private:
    string working_line;
    vector<pair<work_date, string>> base;
    work_date date;
};

int main(void)
{
    setlocale(LC_ALL, "");

    HTML_reader object;
    string adress = "http://www.tuvaenergo.ru/clients/offlist_p/index.php";
    auto str = object.out_str(adress, 1586);
    if (str == "0"s) {
        cout << "error";
        return 0;
    }

    Str_Parser parser(str);
    parser.parser();
    vector<int> tmp_test;
    adress = "http://www.tuvaenergo.ru/clients/offlist_p/pof.php?dt=";
    for (const auto& pair : parser.get_base()) {
        string tmp = adress + pair.second;
        //tmp_test.push_back(object.out_str(tmp, 1587).find("р-н Дзун-Хемчикский, г Чадан, ул Шахтерская"));
        cout << object.out_str(tmp, 1587) << endl;
    }

    return 0;
}