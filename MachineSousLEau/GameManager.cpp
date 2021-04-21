#include "GameManager.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "Submarine.h"
#include "Enemie.h"

#define PI 3.14159265

GameManager* GameManager::Instance = nullptr;

GameManager::GameManager() {


}

void GameManager::gameLoop() {
    //game loop
    while (Window->isOpen())
    {
        updateDeltaTime();

        processEvent();

        updateEntity();

        deleteElement();

        updateScreen();
    }
}

float GameManager::distanceTwoPoint(sf::Vector2f a, sf::Vector2f b)
{
    float X = (a.x - b.x);
    float Y = (a.y - b.y);

    return abs(sqrt(X * X + Y * Y));
}

void GameManager::updateDeltaTime()
{
    DeltaTime = DeltaClock->getElapsedTime().asSeconds();
    DeltaClock->restart();
}

void GameManager::processEvent()
{
    //Process event
    sf::Event event;
    while (Window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            Window->close();

        if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Up))
            Player->motorIsOn(true);
        if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Down))
            Player->motorIsOn(false);
        if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Right))
            RightDown = true;
        if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Left))
            LeftDown = true;

        if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Right))
            RightDown = false;
        if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Left))
            LeftDown = false;
    }
}

void GameManager::updateEntity()
{
    UpdateMinimap();

    if (RightDown) Player->addRotation(0.05);
    if (LeftDown) Player->addRotation(-0.05);

    Player->Tick(DeltaTime);
}

void GameManager::deleteElement()
{
    
}

void GameManager::updateScreen()
{
    // Clear screen
    Window->clear();

    //Panel
    Window->draw(*rect);
    Window->draw(*rect2);

    //Game right panel
    Window->draw(*GameWindow);
    Window->draw(*Player->getShape());

    for (Enemie* e : AllEnemies) {
        sf::Vector2f relativePosition(e->getCoordinate() - Player->getCoordinate());

        if (distanceTwoPoint(e->getCoordinate(), Player->getCoordinate()) < GameWindow->getRadius() + GameWindow->getOutlineThickness()) {
            e->setScreenPosition(Player->getScreenPosition() + relativePosition);

            Window->draw(*e->getShape());
        }
    }

    Window->draw(*Minimap);
    Window->draw(*PlayerIcon);
    for (sf::CircleShape* cs : AllEnemiesIcon) {
        Window->draw(*cs);
    }

    // Update the window
    Window->display();
}

GameManager* GameManager::getInstance()
{
    if (Instance == nullptr) {
        Instance = new GameManager();
        Instance->initialize();
    }
    return Instance;
}

void GameManager::initialize() {

    srand((unsigned)time(NULL));

    // Create the main window
    Window = new sf::RenderWindow(sf::VideoMode(1500, 850), "SFML window");

    DeltaClock = new sf::Clock;
    DeltaTime = 0;

    //LeftPanel
    rect = new sf::RectangleShape(sf::Vector2f(650, 850));
    rect->setFillColor(sf::Color(100, 180, 100, 70));
    rect->setOutlineColor(sf::Color::Green);
    rect->setOutlineThickness(-5);
    //RightPanel
    rect2 = new sf::RectangleShape(sf::Vector2f(850, 850));
    rect2->setFillColor(sf::Color(100, 100, 100, 150));
    rect2->setPosition(650, 0);
    //GameCamera
    GameWindow = new sf::CircleShape(850 / 2, 80);
    GameWindow->setFillColor(sf::Color::Black);
    GameWindow->setOutlineColor(sf::Color::Green);
    GameWindow->setOutlineThickness(-5);
    GameWindow->setPosition(650, 0);
    //Minimap
    Minimap = new sf::CircleShape(120, 40);
    Minimap->setFillColor(sf::Color::Black);
    Minimap->setOutlineColor(sf::Color::Green);
    Minimap->setOutlineThickness(-2);
    Minimap->setPosition(20, 20);
    //Player minimap icon
    PlayerIcon = new sf::CircleShape(5, 10);
    PlayerIcon->setFillColor(sf::Color::Red);
    PlayerIcon->setOrigin(5, 5);
    PlayerIcon->setPosition(Minimap->getPosition() + sf::Vector2f(Minimap->getRadius(), Minimap->getRadius()));

    Player = new Submarine(GameWindow->getPosition() + sf::Vector2f(GameWindow->getRadius(), GameWindow->getRadius()));
    Player->setCoordinate(GameWindow->getPosition() + sf::Vector2f(GameWindow->getRadius(), GameWindow->getRadius()));

    AllEnemies.push_back(new Enemie(GameWindow->getPosition() + sf::Vector2f(265, 565)));
    AllEnemies.push_back(new Enemie(GameWindow->getPosition() + sf::Vector2f(265 + 200, 565)));
    AllEnemies.push_back(new Enemie(GameWindow->getPosition() + sf::Vector2f(265 + 250, 565)));
    AllEnemies.push_back(new Enemie(GameWindow->getPosition() + sf::Vector2f(700, 386)));
    AllEnemies.push_back(new Enemie(GameWindow->getPosition() + sf::Vector2f(250, 352)));
}

void GameManager::UpdateMinimap()
{
    AllEnemiesIcon.clear();
    sf::Vector2f posP = Player->getCoordinate();
    for (Enemie* e : AllEnemies) {
        sf::Vector2f posE = e->getCoordinate();

        float x = (posP.x - posE.x);
        float y = (posP.y - posE.y);

        float Distance = sqrt(x * x + y * y);
        float Angle = std::atan2(y, -x);

        Angle = Angle - (90 - Player->getRotation()) / 180*PI;

        float reduction = 10;

        float xP = PlayerIcon->getPosition().x + (1.f / reduction) * Distance * std::cos(Angle);
        float yP = PlayerIcon->getPosition().y - (1.f / reduction) * Distance * std::sin(Angle);

        if (distanceTwoPoint(sf::Vector2f(xP,yP), PlayerIcon->getPosition()) < Minimap->getRadius() + Minimap->getOutlineThickness()) {
            sf::CircleShape* icon = new sf::CircleShape(3, 10);
            icon->setFillColor(sf::Color::Yellow);
            icon->setOrigin(1.5, 1.5);
            icon->setPosition(xP, yP);
            AllEnemiesIcon.push_back(icon);
        }
    }
}
