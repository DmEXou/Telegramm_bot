all:
	clang++ telegram.cpp -o bot_e -std=c++20 -lcurl -lTgBot -lboost_system -lssl -lcrypto -lpthread
