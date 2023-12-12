#include <iostream>
#include <vector>
#include <string>

#include <tgbot/tgbot.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

class to_cyrillic {
public:
    std::string trans(const std::string& str) {
        std::string result;
        for (const auto& ch : str) {
            if (ch == ' ') {
                result += ' ';
                continue;
            }
            if (ch == -72) {
                result += "\xD1\x91";
                continue;
            }
            if (ch == -88) {
                result += "\xD0\x81";
                continue;
            }
            if (ch >= -64 && ch <= -17) {
                result += '\xD0';
                char tmp = ch + -48;
                result += tmp;
                continue;
            }
            if (ch >= -16 && ch <= -1) {
                result += '\xD1';
                char tmp = ch + -112;
                result += tmp;
                continue;
            }
            result += ch;
        }
        return result;
    }
};

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
        bot_ = std::move(bot);
        data_ = std::move(data);
	    pars_ = std::move(pars);
    }

    void bc_cost(TgBot::CallbackQuery::Ptr query, TgBot::CallbackQuery::Ptr bc_query) {
        double cost_begin = 10000.0, cost_end = 20000.0;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        if (bc_query->data == "up") {
            bot_->getApi().sendMessage(query->message->chat->id, "UP!!!");
        }
        if (bc_query->data == "down") {
            bot_->getApi().sendMessage(query->message->chat->id, "DOWN!!!");
        }
        //pars_.crow_update();
        //auto cost_bc = pars_.get_json().find("bit_c").value();
        //if (!cost_bc.is_null()) {
        //    if (cost_bc.is_number())
        //        cost_begin = std::stod(cost_bc.dump());
        //    std::this_thread::sleep_for(std::chrono::minutes(5));
        //    pars_.crow_update();
        //    cost_bc = pars_.get_json().find("bit_c").value();
        //    if (cost_bc.is_number())
        //        cost_end = std::stod(cost_bc.dump());
        //}
        //bc_[name] = cost_end - cost_begin;
    }

    void work_bot() {
        using namespace std::literals;
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        std::vector<TgBot::InlineKeyboardButton::Ptr> our_button;
        TgBot::InlineKeyboardButton::Ptr electric_btn(new TgBot::InlineKeyboardButton);
        TgBot::InlineKeyboardButton::Ptr exchange_btn(new TgBot::InlineKeyboardButton);
        TgBot::InlineKeyboardButton::Ptr bc_game_btn(new TgBot::InlineKeyboardButton);
        to_cyrillic trans;
        electric_btn->text = trans.trans("Откл-я Тываэнерго");
        exchange_btn->text = trans.trans("Курс валют");
        bc_game_btn->text = trans.trans("Игра в биткоин");
        electric_btn->callbackData = "electric"s;
        exchange_btn->callbackData = "exchange"s;
        bc_game_btn->callbackData = "bc_game"s;
        our_button.push_back(electric_btn);
        our_button.push_back(exchange_btn);
        our_button.push_back(bc_game_btn);
        keyboard->inlineKeyboard.push_back(our_button);

        bot_->getEvents().onCommand("start", [&](TgBot::Message::Ptr message) {
            bot_->getApi().sendMessage(message->chat->id, trans.trans("Привет ") + message->chat->firstName);
            bot_->getApi().sendMessage(message->chat->id, trans.trans("Выберете меню"), false, 0, keyboard);
            });

        bot_->getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr query) {
            if (query->data == "exchange") {
	    	    pars_.crow_update();
	            data_ = pars_.get_json();
                for (const auto& obj_exc : data_.find("rates").value()) {

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
                    bot_->getApi().sendMessage(query->message->chat->id, message);
                }
            }
            if (query->data == "electric") {
	            pars_.crow_update();
	            data_ = pars_.get_json();
                auto it = data_.find("energy").value().at("tuva_energo").begin();
                for (; it != data_.find("energy").value().at("tuva_energo").end(); ++it) {
                    std::string str = it.key();
                    str += " - ";
                    bot_->getApi().sendMessage(query->message->chat->id, str);
                }
            }
            if (query->data == "bc_game") {
                TgBot::InlineKeyboardMarkup::Ptr bc_keyboard(new TgBot::InlineKeyboardMarkup);
                TgBot::InlineKeyboardButton::Ptr up_btn(new TgBot::InlineKeyboardButton), down_btn(new TgBot::InlineKeyboardButton);
                up_btn->text = trans.trans("На повышение");
                down_btn->text = trans.trans("На понижение");
                up_btn->callbackData = "up"s;
                down_btn->callbackData = "down"s;
                std::vector<TgBot::InlineKeyboardButton::Ptr> bc_button{ up_btn , down_btn };
                bc_keyboard->inlineKeyboard.push_back(bc_button);
                bot_->getApi().sendMessage(query->message->chat->id, trans.trans("На повышение или понижение?"), false, 0, bc_keyboard);

                bot_->getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr bc_query) {
                    std::thread bc_th(&Telegram_bot::bc_cost, this, query, bc_query);
                    bc_th.detach();
                    });
            }
            });

        try {
            printf("Bot username: %s\n", bot_->getApi().getMe()->username.c_str());
            TgBot::TgLongPoll longPoll(*bot_);
            while (true) {
                longPoll.start();
            }
        }

        catch (TgBot::TgException& e) {
            time_t rawtime;
            time(&rawtime);
            std::cout << "error: %s\n" << e.what() << " - " << localtime(&rawtime)->tm_mday << " " 
                << localtime(&rawtime)->tm_hour << ":" << localtime(&rawtime)->tm_min << std::endl;
        }
    }

private:
    std::unique_ptr<TgBot::Bot> bot_;
    nlohmann::json data_;
    Crow_pars pars_;
    std::map<std::string, double> bc_;
};

int main() {
    Crow_pars pars;
    pars.crow_update();
    Telegram_bot bot_(pars.get_json(), pars);
    bot_.work_bot();

	return 0;
}
