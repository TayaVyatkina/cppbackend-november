#pragma once
#include "tagged.h"
#include "player.h"


#include <random>
#include <unordered_map>
#include <memory>
#include <string>

namespace auth {

    struct TokenTag {};            

    using Token = util::Tagged<std::string, TokenTag>;
    using TokenHasher = util::TaggedHasher<Token>;

    class PlayerTokens {
    public:
        PlayerTokens() = default;
        PlayerTokens(const PlayerTokens& other) = default;
        PlayerTokens(PlayerTokens&& other) = default;
        PlayerTokens& operator = (const PlayerTokens& other) = default;
        PlayerTokens& operator = (PlayerTokens&& other) = default;
        virtual ~PlayerTokens() = default;

        Token AddPlayer(std::weak_ptr<app::Player> player);                                      
        std::weak_ptr<app::Player> FindPlayerByToken(Token token);                               
        void SetPlayerToken(Token token, std::shared_ptr<app::Player> player);                   

    private:
        std::unordered_map< Token, std::weak_ptr<app::Player>, TokenHasher > tokenToPlayer_;      

        std::random_device random_device_;
        std::mt19937_64 generator1_{ [this] {
            std::uniform_int_distribution<std::mt19937_64::result_type> dist;
            return dist(random_device_);
        }() };
        std::mt19937_64 generator2_{ [this] {
            std::uniform_int_distribution<std::mt19937_64::result_type> dist;
            return dist(random_device_);
        }() };

    };

}