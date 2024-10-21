#pragma once

#include <cassert>
#include <iomanip>
#include <memory>
#include <random>
#include <vector>
#include <unordered_map>
#include "tagged.h"

namespace model {

namespace detail {
struct TokenTag {};
}  // namespace detail

using namespace std::literals;

using Token = util::Tagged<std::string, detail::TokenTag>;
using TokenHasher = util::TaggedHasher<Token>;

template <typename T>
struct ParamPair {
    ParamPair() = default;

    ParamPair(const T& x, const T& y) :
        x_(x),
        y_(y) 
    {}

    T x_;
    T y_;
};

using ParamPairInt = ParamPair<int>;
using ParamPairDouble = ParamPair<double>;

class Player;
class GameSession;

Token GetToken();

class Dog {
public:
    explicit Dog(Token pl_tok);

    int GetId() const;

    Token GetToken() const;

    bool SetToken(const Token& pl_token);

    const ParamPairDouble GetDogPosition() const;

    const ParamPairDouble GetDogSpeed() const;

    void SetPosition(const ParamPairDouble& position);

    void SetSpeed(const ParamPairDouble& speed);

    const std::string& GetDogDirection() const;

    void SetDogDirection(const std::string& dir);

    void Move(const ParamPairDouble& move_dist);

private:
    Token token_;
    int dog_id_;
    static int dog_id_counter_;

    std::string dir_;
    ParamPairDouble dog_position_;
    ParamPairDouble dog_speed_;
};

class Player {
public:
    Player(const Token& token, const std::string& name);

    Token GetPlayerToken() const;

    int GetId() const noexcept;

    const std::shared_ptr<GameSession> GetPlayersSession() const;

    std::shared_ptr<GameSession> GetPlayersSession();

    const std::string& GetName() const;

    void SetName(const std::string& name);

    std::shared_ptr<Dog> GetDog() const;

    void SetDog(std::shared_ptr<Dog> dog);

private:
    Token player_token_;
    std::string player_name_;
    std::shared_ptr<GameSession> session_;
    std::shared_ptr<Dog> dog_;
    int player_id_;
    static int player_id_counter_;
};

class PlayerList {
public:    
    Player* FindPlayer(const Token& token);

    Player& AddPlayer(const std::string& name);

    const std::unordered_map<Token, Player, TokenHasher>& GetPlayersList() const;
    
    auto begin();

    auto end();

private:
    std::unordered_map<Token, Player, TokenHasher> players_;
};

} // namespace model 