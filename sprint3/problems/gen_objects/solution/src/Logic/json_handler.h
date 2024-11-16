#pragma once
#include "application.h"
#include <string>
#include <tuple>
#include <optional>

namespace jsonOperation {

	const std::string RESPONSE_CODE = "code";
	const std::string RESPONSE_MESSAGE = "message";
	const std::string RESPONSE_AUTHTOKEN = "authToken";
	const std::string RESPONSE_PLAYERID = "playerId";
	const std::string RESPONSE_NAME = "name";
	const std::string RESPONSE_PLAYERS = "players";
	const std::string RESPONSE_DOG_POSITION = "pos";
	const std::string RESPONSE_DOG_SPEED = "speed";
	const std::string RESPONSE_DOG_DIRECTION = "dir";
	const std::string RESPONSE_LOST_OBJECTS = "lostObjects";
	const std::string RESPONSE_LOST_OBJ_TYPE = "type";
	const std::string RESPONSE_LOST_OBJ_POSITION = "pos";

	const std::string REQUEST_NAME = "userName";
	const std::string REQUEST_MAPID = "mapId";
	const std::string REQUEST_MOVE = "move";
	const std::string REQUEST_TIME_DELTA = "timeDelta";

	std::string GameToJson(const model::Game::Maps& game);		
	std::string MapToJson(const model::Map& map);				
	std::string PageNotFound();									
	std::string BadRequest();									
	std::string MapNotFound();							

	
	std::string JoinToGameInvalidArgument();															//Передали кривые аргументы
	std::string JoinToGameMapNotFound();																//При заходе в игру не нашли карту
	std::string JoinToGameEmptyPlayerName();															//Задали пустое имя
	std::string PlayersListOnMap(const std::vector< std::weak_ptr<app::Player> >& players);				//Выдать списко игорьков
	std::string InvalidMethod();																		//Выдать invalid method
	std::string EmptyAuthorization();																	//Выдать, что никто не авторихвоан при запросе списка игорьков
	std::string UnknownToken();																			//Выдать, что кривой токен при запросе списка игорьков
	std::string JoinToGame(const std::string& token, size_t player_id);									//Выдать, что вошли в игру
	std::string GameState(const std::vector< std::weak_ptr<app::Player> >& players, 
						  const app::GameSession::lostObjectsId lostObjects);							//Выдать состояние игры + потерянные объекты
	std::string PlayerAction();																			//Действия игорька
	std::string PlayerInvalidAction();																	//Кривые действия игорька
	std::string InvalidContentType();																	//Кривое поле контент тупе
	std::string SetDeltaTime();																			//Установка дельты времени
	std::string InvalidDeltaTime();																		//Пришла кривая дельта
	std::string InvalidEndpoint();																		//Кривой эндпоинт

	std::string OnlyPostMethodAllowed();															
	std::optional< std::tuple<std::string, model::Map::Id> > ParseJoinToGame(const std::string& msg);	//Распарсить реквест входа в игру
	std::optional<std::string> ParsePlayerActionRequest(const std::string& msg);						//Парсим то, что передали на действие
	std::optional<int> ParseSetDeltaTimeRequest(const std::string& msg);								//Парсим дельта тайм запрос	

}