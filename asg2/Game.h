#pragma once

#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>

// Structs are just public classes
//SR - Shape Radius
//CR - Collision Radius
//F
struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig{ int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
	sf::RenderWindow m_window; // window drawing to
	EntityManager m_entities; // vector of entities to maintain
	sf::Font m_font; // font used to draw
	sf::Text m_text; // score text to show on screen
	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	BulletConfig m_bulletConfig;
	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;
	bool m_paused = false; // whether game logic should be updated
	bool m_running = true; // whether game is running

	std::shared_ptr<Entity> m_player;

	void init(const std::string& config);
	void setPaused(bool paused);

	void sMovement(); // Entity position and Movement update
	void sUserInput();
	void sLifespan();
	void sRender(); // Render/drawing of entities
	void sEnemySpawner(); // Spawn enemy entities
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

public:
	Game(const std::string& config); // takes in game config file
	void run();
};