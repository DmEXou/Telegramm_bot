#define _CRT_SECURE_NO_WARNINGS

#include <future>
#include <map>
#include <set>
#include <string>

#include <tgbot/tgbot.h>
#include <curl/curl.h>

#pragma comment(lib,"libcurl.lib")

using namespace std;

struct work_date {
    int day = 0;
    int mon = 0;
    int year = 0;
};

string to_str(const work_date& date) {
    return string(to_string(date.day) + '.' + to_string(date.mon) + '.' + to_string(date.year));
}
map<string, set<string>> rebuild(); // Форвард деклорейшон:)

class HTML_reader {
public:

    string out_str(const string& adres, const int line_n) {
        string* readBuffer_UTF8 = new string;
        readBuffer_UTF8->reserve(82000);
        CURL* curl;
        CURLcode res_inter;
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, adres.data());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &*readBuffer_UTF8);//
            res_inter = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        else {
            return "";
        }
        if (res_inter != 0) {
            return "";
        }

        for (size_t i = 0; i < line_n - 1; ++i) {
            readBuffer_UTF8->erase(0, readBuffer_UTF8->find('\n') + 1);
        }
        auto result = readBuffer_UTF8->substr(0, readBuffer_UTF8->find('\n'));
        delete(readBuffer_UTF8);
        return result;
    }

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
};

class Date_Parser {
public:

    Date_Parser(const string& str)
        :working_line(str)
    {
        set_local_data();
        parser();
    }

    void give_str(const string& str) {
        working_line = str;
    }

    void parser() { //
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
                pair check_date = short_pars(working_line.substr(pos + 11, 17));
                if (check_date.first.year == date.year) { // Проверка дат. Нужны только даты текущего дня и будущие. Не больше 7ми дней.
                    if (check_date.first.mon == date.mon) {
                        if (check_date.first.day >= date.day) {
                            count++;
                            base_dates.push_back(check_date);
                        }
                    }
                    else if (check_date.first.mon > date.mon) {
                        count++;
                        base_dates.push_back(check_date);
                    }
                }
                else if (check_date.first.year > date.year) {
                    count++;
                    base_dates.push_back(check_date);
                }
                working_line.erase(pos, working_line.length() - pos - 1);
            }
        }
    }

    vector<pair<work_date, string>> get_base() {
        return base_dates;
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
    vector<pair<work_date, string>> base_dates;
    work_date date;
};

class Search_off {
public:
    Search_off(const string& str) {
        if (str.find("1127403") != string::npos) result.insert("Shagonar"s);
        if (str.find("1127336") != string::npos) result.insert("Shagonar"s);
        if (str.find("314113") != string::npos) result.insert("Bazhyn-Alaak"s);
        if (str.find("21628") != string::npos) result.insert("Sug-Aksy"s);
        if (str.find("314259") != string::npos) result.insert("Chadan"s);
        if (str.find("1127411") != string::npos) result.insert("Chaa-Khol"s);
        if (!result.empty()) check = true; // Коздать метод возвращающий result.empty(); Отдельно избавится от Флага!!!
    }

    set<string> Get_result() {
        return result;
    }

    bool Get_check() {
        return check;
    }

private:
    set<string> result;
    bool check = false;
};

class Telegram_bot {
public:
    Telegram_bot(const map<string, set<string>>& obj)
        :base_off(obj)
    {
    }

    void colback(TgBot::Bot& bot) {
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        std::vector<TgBot::InlineKeyboardButton::Ptr> our_button;
        TgBot::InlineKeyboardButton::Ptr all_but(new TgBot::InlineKeyboardButton), shagonar_bat(new TgBot::InlineKeyboardButton);
        all_but->text = "All settlements."s;
        shagonar_bat->text = "Shagonar!"s;
        all_but->callbackData = "all"s;
        shagonar_bat->callbackData = "sh_only"s;
        our_button.push_back(all_but);
        our_button.push_back(shagonar_bat);
        keyboard->inlineKeyboard.push_back(our_button);

        bot.getEvents().onCommand("start", [&bot, &keyboard](TgBot::Message::Ptr message) {
            bot.getApi().sendMessage(message->chat->id, "Hi! "s + message->chat->firstName);
            bot.getApi().sendMessage(message->chat->id, "Choise"s, false, 0, keyboard);
            });

        bot.getEvents().onCallbackQuery([&bot, &keyboard, this](TgBot::CallbackQuery::Ptr query) {
            if (query->data == "all") {
                if (base_off.empty()) {
                    bot.getApi().sendMessage(query->message->chat->id, "No shutdowns planned"s);
                }
                else {
                    bot.getApi().sendMessage(query->message->chat->id, base_to_string());
                }
            }
            else if (query->data == "sh_only") {
                bool flag = base_off.find("1127403") != base_off.end();
                if (flag || base_off.find("1127336") != base_off.end()) {
                    string need_str;//Требуется Тестеровка!!!! 
                    if (flag) {
                        need_str = set_to_string(base_off.find("1127403")->second);
                        if (base_off.find("1127336") != base_off.end()) {
                            need_str += " " + set_to_string(base_off.find("1127336")->second);
                        }
                    }
                    bot.getApi().sendMessage(query->message->chat->id, need_str);
                }
                else {
                    bot.getApi().sendMessage(query->message->chat->id, "No shutdowns planned"s);
                }
            }
            else {
                bot.getApi().sendMessage(query->message->chat->id, "ERROR!!!"s);
            }
            });
        try {
            printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
            TgBot::TgLongPoll longPoll(bot);
            while (true) {
                time_t rawtime;
                time(&rawtime);
                //cout << "Hours " << localtime(&rawtime)->tm_hour << endl;
                if (localtime(&rawtime)->tm_hour % 6 == 0 && flag_time) {
                    cout << " rebuild and flag false.\n";
                    base_off = rebuild();
                    flag_time = false;
                }
                if (localtime(&rawtime)->tm_hour % 6 != 0 && !flag_time) { 
                    cout << " flag true.\n";
                    flag_time = true;
                }
                longPoll.start();
            }
        }
        catch (TgBot::TgException& e) {
	    time_t rawtime;
	    time(&rawtime);
            cout << "error: %s\n" << e.what() << " - " << localtime(&rawtime)->tm_mday << " " << localtime(&rawtime)->tm_hour << ":" << localtime(&rawtime)->tm_min << endl;
        }
    }

private:
    string base_to_string() {
        string result;
        for (const auto& pair : base_off) {
            result += pair.first + " : ";
            for (const auto& str : pair.second) {
                result += str + ' ';
            }
            result += '\n';
        }
        return result;
    }

    string set_to_string(const set<string>& obj) {
        string result;
        for (const auto& str : obj) {
            result += str + " ";
        }
        result.pop_back();
        return result;
    }

    map<string, set<string>> base_off;
    bool flag_time = true;
};

map<string, set<string>> rebuild() {
    HTML_reader object;
    string adress = "http://www.tuvaenergo.ru/clients/offlist_p/index.php";
    auto str_dates = object.out_str(adress, 1586);
    if (str_dates.empty()) {
        cout << "ERROR - String is empty.";
        return {};
    }

    Date_Parser parser(str_dates);

    map<string, set<string>> date_name_off_ener;
    adress = "http://www.tuvaenergo.ru/clients/offlist_p/pof.php?dt=";
    vector<future<string>> future_bufer;
    vector<string> date;
    for (const auto& pair : parser.get_base()) { // Может быть стоит добавить mutex!!!
        string tmp_adress = adress + pair.second;
        future_bufer.push_back(async(&HTML_reader::out_str, &object, tmp_adress, 1587));//
        date.push_back(to_str(pair.first));
    }
    for (int i = 0; i < future_bufer.size(); ++i) {
        string str_to_searcer = future_bufer[i].get();
        if (str_to_searcer.empty()) {
            cout << "ERROR - String is empty.";
            continue;
        }
        Search_off oblect_off(str_to_searcer);
        if (oblect_off.Get_check()) {
            date_name_off_ener[date[i]] = oblect_off.Get_result();
        }
    }
    return date_name_off_ener;
}

int main(){
    TgBot::Bot bot("5726778307:AAGg4pdWcDK2UKDJzOEjvto4mb8JNqfQeeo");
    Telegram_bot build(rebuild());
    build.colback(bot);
    return 0;
}
