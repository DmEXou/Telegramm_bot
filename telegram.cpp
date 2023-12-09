#include <iostream>
#include <vector>
#include <string>

#include <tgbot/tgbot.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

class Crow_pars {
public:

    void crow_update() {
        std::string readBuffer_UTF8;
        readBuffer_UTF8.reserve(82000);
        CURL* curl;
        CURLcode res_inter;
        curl = curl_easy_init();


        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, "http://45.10.246.155");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer_UTF8);
            res_inter = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        if (!readBuffer_UTF8.empty()) {
            _data_json = nlohmann::json::parse(readBuffer_UTF8);
        }
    }

    nlohmann::json get_json() {
        return _data_json;
    }

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

private:
    nlohmann::json _data_json;
};

class Telegram_bot {
public:
    Telegram_bot(const nlohmann::json& data, const Crow_pars& pars) {
        std::unique_ptr<TgBot::Bot> bot = std::make_unique<TgBot::Bot>(TgBot::Bot("5726778307:AAGg4pdWcDK2UKDJzOEjvto4mb8JNqfQeeo"));
        _bot = std::move(bot);
        _data = std::move(data);
	_pars = std::move(pars);
    }

    void work_bot() {
        using namespace std::literals;
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        std::vector<TgBot::InlineKeyboardButton::Ptr> our_button;
        TgBot::InlineKeyboardButton::Ptr electric_btn(new TgBot::InlineKeyboardButton), exchange_btn(new TgBot::InlineKeyboardButton);
        electric_btn->text = "ELEKTRICHESTVO";
        exchange_btn->text = "KURS VALUT";
        electric_btn->callbackData = "electric"s;
        exchange_btn->callbackData = "exchange"s;
        our_button.push_back(electric_btn);
        our_button.push_back(exchange_btn);
        keyboard->inlineKeyboard.push_back(our_button);

        _bot->getEvents().onCommand("start", [&](TgBot::Message::Ptr message) {
	    std::string tmp_line_for_message = "\xD0\x90";
	    tmp_line_for_message += " ";
            _bot->getApi().sendMessage(message->chat->id, tmp_line_for_message  + message->chat->firstName);
            _bot->getApi().sendMessage(message->chat->id, "Vibiraite menu"s, false, 0, keyboard);
            });

        _bot->getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr query) {
            if (query->data == "exchange") {
	    	_pars.crow_update();
		_data = _pars.get_json();
                for (const auto& obj_exc : _data.find("rates").value()) {

                    auto message = obj_exc.find("CharCode").value().dump();
                    message += " - ";
                    message += obj_exc.find("Value").value().dump();
                    if (obj_exc.find("Value").value() > obj_exc.find("Previous").value()) {
                        message += " ";
                        message += "\xF0\x9F\x93\x88";
                    }
                    else {
                        message += " ";
                        message += "\xF0\x9F\x93\x89";
                    }
                    _bot->getApi().sendMessage(query->message->chat->id, message);
                }
            }
            if (query->data == "electric") {
		_pars.crow_update();
		_data = _pars.get_json();
                auto it = _data.find("energy").value().at("tuva_energo").begin();
                for (; it != _data.find("energy").value().at("tuva_energo").end(); ++it) {
                    std::string str = it.key();
                    str += " - ";

                    _bot->getApi().sendMessage(query->message->chat->id, str);
                }
            }
            });

        try {
            printf("Bot username: %s\n", _bot->getApi().getMe()->username.c_str());
            TgBot::TgLongPoll longPoll(*_bot);
            while (true) {
                longPoll.start();
            }
        }
        catch (TgBot::TgException& e) {
            time_t rawtime;
            time(&rawtime);
            std::cout << "error: %s\n" << e.what() << " - " << localtime(&rawtime)->tm_mday << " " << localtime(&rawtime)->tm_hour << ":" << localtime(&rawtime)->tm_min << std::endl;
        }
    }

private:
    std::unique_ptr<TgBot::Bot> _bot;
    nlohmann::json _data;
    Crow_pars _pars;
};

int main() {
    Crow_pars pars;
    pars.crow_update();
    Telegram_bot bot_(pars.get_json(), pars);
    bot_.work_bot();

	return 0;
}
