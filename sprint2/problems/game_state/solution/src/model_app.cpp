#include "model_app.h"

namespace model {

int Dog::dog_id_counter_ = 0;
int Player::player_id_counter_ = 0;

Token GetToken() {
    std::random_device random_device_;
    std::mt19937_64 generator1_{[&random_device_] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    std::mt19937_64 generator2_{[&random_device_] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    std::stringstream ss;
    ss << std::setw(16) << std::setfill('0') << std::hex << generator1_(); 
    ss << std::setw(16) << std::setfill('0') << std::hex << generator2_(); 
    std::string result = ss.str();
    assert(result.size() == 32);
    return Token(ss.str());
} 

Dog::Dog(Token pl_tok) :
    dog_id_(++dog_id_counter_),
    token_(pl_tok),
    dog_position_(0., 0.),
    dog_speed_(0., 0.)
    {
        SetDogDirection("U");
    }

int Dog::GetId() const {
    return dog_id_;
}

Token Dog::GetToken() const {
    return token_;
}

bool Dog::SetToken(const Token& pl_token) {
    if ((*token_).empty()) {
        token_ = pl_token;
        return true;
    }
    return false;
}
const ParamPairDouble Dog::GetDogPosition() const {
    return dog_position_;
}

const ParamPairDouble Dog::GetDogSpeed() const {
    return dog_speed_;
}

void Dog::SetPosition(const ParamPairDouble& position) {
    dog_position_.x_ = position.x_;
    dog_position_.y_ = position.y_;
}

void Dog::SetSpeed(const ParamPairDouble& speed) {
    dog_speed_.x_ = speed.x_;
    dog_speed_.y_ = speed.y_;
}

const std::string& Dog::GetDogDirection() const {
    return dir_;
}

void Dog::SetDogDirection(const std::string& dir) {
    if (dir == "U" || dir == "R" || dir == "D" || dir == "L") {
        dir_ = dir;
        return;
    }
    throw std::invalid_argument("Unknown direction...");
}

void Dog::Move(const ParamPairDouble& move_dist) {
    dog_position_.x_ += move_dist.x_;
    dog_position_.y_ += move_dist.y_;
}

Player::Player(const Token& token, const std::string& name) :
    player_token_(token), 
    player_name_(name),
    player_id_(++player_id_counter_){
        SetDog(std::make_shared<Dog>(token));
    }

Token Player::GetPlayerToken() const {
    return player_token_;
}

int Player::GetId() const noexcept {
    return player_id_;
}

const std::shared_ptr<GameSession> Player::GetPlayersSession() const {
    return session_;
}

std::shared_ptr<GameSession> Player::GetPlayersSession() {
    return session_;
}

const std::string& Player::GetName() const {
    return player_name_;
}

void Player::SetName(const std::string& name) {
    if (!player_name_.empty()) {
        throw std::logic_error("Cannot rename player...");
    }
    if (name.empty()) {
        throw std::logic_error("Cannot set empty name...");
    }
    player_name_ = name;
}

std::shared_ptr<Dog> Player::GetDog() const {
    return dog_;
}

void Player::SetDog(std::shared_ptr<Dog> dog) {
    if (dog_) {
        throw std::logic_error("Player already owns a dog...");
    }
    if (!dog) {
        throw std::logic_error("Dog not specified...");
    }
    dog_ = dog;
}

Player* PlayerList::FindPlayer(const Token& token) {
    if (players_.contains(token)) {
        return &players_.at(token);
    }
    return nullptr;
}

Player& PlayerList::AddPlayer(const std::string& name) {
    Token token = GetToken();
    auto p = players_.emplace(token, Player(token, name));
    if (p.second) {
        return players_.at(token);
    }
    throw std::runtime_error("Failed to add player...");
}

const std::unordered_map<Token, Player, TokenHasher>& PlayerList::GetPlayersList() const {
    return players_;
}

auto PlayerList::begin() {
    return players_.begin();
}

auto PlayerList::end() {
    return players_.end();
}



} // namespace model 