cmake_minimum_required(VERSION 3.10)

project(Risk_Game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mapDriver
    map/map.cpp
    map/mapDriver.cpp
    Player/Player.cpp
    Orders/Orders.cpp
    cards/Cards.cpp
    LoggingObserver/LoggingObserver.cpp)

add_executable(cardsDriver
    cards/Cards.cpp
    Player/Player.cpp
    Orders/Orders.cpp
    map/map.cpp
    cards/CardsDriver.cpp
    LoggingObserver/LoggingObserver.cpp)

add_executable(ordersDriver
  Orders/Orders.cpp
  Orders/OrdersDriver.cpp
  Player/Player.cpp
  map/map.cpp
  cards/Cards.cpp
  LoggingObserver/LoggingObserver.cpp
)

add_executable(startupPhaseDriver
  GameEngine/StartupPhaseDriver.cpp
  GameEngine/GameEngine.cpp
  map/map.cpp
  Player/Player.cpp
  cards/Cards.cpp
  Orders/Orders.cpp
  CommandProcessor/CommandProcessing.cpp
  LoggingObserver/LoggingObserver.cpp
  PlayerStrategy/PlayerStrategies.cpp
)

add_executable(mainGameLoopDriver
  GameEngine/MainGameLoopDriver.cpp
  GameEngine/GameEngine.cpp
  map/map.cpp
  Player/Player.cpp
  cards/Cards.cpp
  Orders/Orders.cpp
  CommandProcessor/CommandProcessing.cpp
  LoggingObserver/LoggingObserver.cpp
  PlayerStrategy/PlayerStrategies.cpp
)


add_executable(loggingObserverDriver
    LoggingObserver/LoggingObserver.cpp
    LoggingObserver/LoggingObserverDriver.cpp
    CommandProcessor/CommandProcessing.cpp
    GameEngine/GameEngine.cpp
    map/map.cpp
    Player/Player.cpp
    cards/Cards.cpp
    Orders/Orders.cpp
    PlayerStrategy/PlayerStrategies.cpp
)

add_executable(playerStrategyDriver
    PlayerStrategy/PlayerStrategies.cpp
    PlayerStrategy/PlayerStrategiesDriver.cpp
    GameEngine/GameEngine.cpp
    CommandProcessor/CommandProcessing.cpp
    map/map.cpp
    Player/Player.cpp
    cards/Cards.cpp
    Orders/Orders.cpp
    LoggingObserver/LoggingObserver.cpp
)

add_executable(demo
  demo.cpp
  map/map.cpp
  Player/Player.cpp
  cards/Cards.cpp
  Orders/Orders.cpp
  GameEngine/GameEngine.cpp
  CommandProcessor/CommandProcessing.cpp
  LoggingObserver/LoggingObserver.cpp
  PlayerStrategy/PlayerStrategies.cpp
)

if(APPLE)
    target_compile_options(demo PRIVATE -stdlib=libc++)
    target_compile_options(mapDriver PRIVATE -stdlib=libc++)
    target_compile_options(cardsDriver PRIVATE -stdlib=libc++)
    target_compile_options(ordersDriver PRIVATE -stdlib=libc++)
    target_compile_options(startupPhaseDriver PRIVATE -stdlib=libc++)
    target_compile_options(mainGameLoopDriver PRIVATE -stdlib=libc++)
    target_compile_options(loggingObserverDriver PRIVATE -stdlib=libc++)
    target_compile_options(playerStrategyDriver PRIVATE -stdlib=libc++)
endif()

set(CMAKE_BUILD_TYPE Release)
