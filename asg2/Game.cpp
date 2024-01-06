#include "Game.h"

#include <iostream>
#include <fstream>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& path)
{
	// TODO: Read config file here using player/enemy/bullet variables
	std::ifstream fin(path);
	std::string temp;
	//WINDOW
	int windowWidth = 0, windowHeight = 0, windowFrameLimit = 0, windowFullscreen = 0;
	//FONT
	std::string fontFile = "";
	int fontSize = 0, fontR = 0, fontG = 0, fontB = 0;
	
	while (fin >> temp)
	{
		if (temp == "Window") fin >> windowWidth >> windowHeight >> windowFrameLimit >> windowFullscreen;
		if (temp == "Font") fin >> fontFile >> fontSize >> fontR >> fontG >> fontB;
		if (temp == "Player") fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
		if (temp == "Enemy") fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
		if (temp == "Bullet") fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
	}

	// Window setup
	m_window.create(sf::VideoMode(windowWidth, windowHeight), "Owen Geometry Wars", windowFullscreen == 1 ? sf::Style::Fullscreen : sf::Style::Default);
	m_window.setFramerateLimit(windowFrameLimit);

	// Font setup
	if (!m_font.loadFromFile(fontFile)) {
		std::cerr << "Couldn't load font\n";
		exit(-1);
	}
	m_text = sf::Text("SCORE: " + std::to_string(m_score), m_font, fontSize);

	// Player setup
	spawnPlayer();

	// Enemy setup done in spawnEnemies()
	// Bullet setup triggered upon user input

	// Create seed for random
	srand((time(0)));
}

void Game::run()
{
	// TODO: Add pause functionality
	// Rendering will still work while game is paused

	while (m_running) // iterates through what should be done each frame
	{
		m_entities.update(); 

		if (!m_paused)
		{
			sEnemySpawner();
			sMovement();
			sCollision();
			sLifespan();
		}

		// Needs to be usable on pause so user can unpause
		sUserInput();
		sRender();

		// increment current frame, move this line to implement game pause logic
		if (!m_paused) m_currentFrame++;
	}
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

// respawn player in middle of screen
void Game::spawnPlayer()
{
	// TODO: Add all properties of player with correct values from config.txt

	// Create all entities with entitymanager's .addEntity(tag)
	auto entity = m_entities.addEntity("player");

	float mx = m_window.getSize().x / 2.0f;// find middle of screen's x
	float my = m_window.getSize().y / 2.0f;// find middle of screen's y

	// Give entity a transform so it spawns at 200,200 with velocity 1,1 and angle 0
	entity->cTransform = std::make_shared<CTransform>(Vec2(mx,my), Vec2(1.0f, 1.0f), 0.0f);

	// Entity shape will have these attributes (radius,sides,color fill, outline color and thickness)
	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

	// Add input component so player can move around using keyboard inputs
	entity->cInput = std::make_shared<CInput>();

	entity->cScore = std::make_shared<CScore>(0);

	// Since we want this entity to be our player, save it to the game class
	// Not supposed to be good and goes against EntityManager paradigm but its ok because there is only 1 player
	m_player = entity;
}

// spawn enemy at random location
void Game::spawnEnemy()
{
	// TODO: Make sure enemy is spawned properly with m_enemyConfig variables
	// enemy must be spawned completely within window bounds
	
	auto entity = m_entities.addEntity("enemy");

	float ex = m_enemyConfig.SR + rand() / (RAND_MAX / (((float)(m_window.getSize().x) - m_enemyConfig.SR) - m_enemyConfig.SR));
	float ey = m_enemyConfig.SR + rand() / (RAND_MAX / (((float)(m_window.getSize().y) - m_enemyConfig.SR) - m_enemyConfig.SR));
	
	// get random x and y directions for speed
	float sx = -1 + 2 * ((float)rand()) / RAND_MAX;
	float sy = -1 + 2 * ((float)rand()) / RAND_MAX;
	// get random speed magnitude
	float s = m_enemyConfig.SMAX == m_enemyConfig.SMIN ? m_enemyConfig.SMIN : m_enemyConfig.SMIN + rand() / (RAND_MAX / (m_enemyConfig.SMAX - m_enemyConfig.SMIN));

	// Give entity a transform so it spawns with position, velocity and angle
	entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(sx, sy).normalize() * s, 0.0f);

	float ev = m_enemyConfig.VMAX == m_enemyConfig.VMIN ? m_enemyConfig.VMIN : (rand() % m_enemyConfig.VMAX) + m_enemyConfig.VMIN;
	float FR = rand() % 255, FG = rand() % 255, FB = rand() % 255;
	// Entity shape will have these attributes (radius,sides,color fill, outline color and thickness)
	entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, ev, sf::Color(FR, FG, FB), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
	
	//add more components here

	// records when most recent enemy was spaned
	m_lastEnemySpawnTime = m_currentFrame;
}

//spawns small enemies when big one explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> enemyEntity)
{
	// TODO: spawn small enemies at location of input enemy

	//when creating smaller enemy, read values of original enemy
	/*- spawn number of small enemies equal to vertices
	- set each small enemy to be same colour as original but half the size
	- small enemies are worth double points of the original enemy*/
	size_t verticeCount = enemyEntity->cShape->circle.getPointCount();
	for (size_t i = 1;i < (verticeCount + 1);i++)
	{
		const double PI = 3.141592653589793238463; // Most simple and efficient way to get pi in c++
		auto speed = enemyEntity->cTransform->velocity.magnitude();
		float angleIncrement = 2*PI / verticeCount;
		
		auto entity = m_entities.addEntity("smallEnemies");

		/*std::cout << "Vertices: " << verticeCount << "\n";
		std::cout << "Small enemy no.: " << i <<"\n";
		std::cout << "Angle is: " << angleIncrement << "\n";
		std::cout << "X is: " << cosf(angleIncrement * i) << "\n";
		std::cout << "Y is: " << sinf(angleIncrement * i) << "\n";*/
		entity->cTransform = std::make_shared<CTransform>(Vec2(enemyEntity->cTransform->pos.x, enemyEntity->cTransform->pos.y), Vec2(cosf(angleIncrement * i), sinf(angleIncrement * i)) * speed, 0.0f);
		entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR/2, verticeCount, enemyEntity->cShape->circle.getFillColor(), enemyEntity->cShape->circle.getOutlineColor(), m_enemyConfig.OT);
		entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	// TODO: implement spawning of bullet which travels towards target
	/*- bullet speed is given as scalar speed
	- set velocity of bullet using a formula*/
	auto bulletEntity= m_entities.addEntity("bullet");

	// Give entity a transform so it spawns with position, velocity and angle
	bulletEntity->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, (target - (entity->cTransform->pos)).normalize() * m_bulletConfig.S, 0.0f);
	// Entity shape will have these attributes (radius,sides,color fill, outline color and thickness)
	bulletEntity->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB, 150), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity, const Vec2& target)
{
	// TODO: EXTRA MARKS
	const auto& baseTargetDir = (target - (entity->cTransform->pos)).normalize();
	std::cout << "Base at: " << baseTargetDir.x << ", " << baseTargetDir.y << "\n";
	
	Vec2 targetDir = baseTargetDir;
	size_t i = 0;

	while(true)
	{
		// Could probably do this loop without a while true but i cant think rn
		auto spawnPos = targetDir + entity->cTransform->pos;
		bool xWithinWindow = spawnPos.x > 0 && spawnPos.x < m_window.getSize().x;
		bool yWithinWindow = spawnPos.y > 0 && spawnPos.y < m_window.getSize().y;

		if (!xWithinWindow || !yWithinWindow) break;

		//std::cout << "Spawned at: " << entity->cTransform->pos.x + targetDir.x << ", " << entity->cTransform->pos.y + targetDir.y << "\n";
		auto e = m_entities.addEntity("bullet");
		e->cTransform = std::make_shared<CTransform>(entity->cTransform->pos + targetDir, Vec2(0, 0), 0.0f);
		e->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB, 150), sf::Color::Transparent, 0.0f);
		e->cLifespan = std::make_shared<CLifespan>(30);
		i++;
		targetDir = baseTargetDir + (baseTargetDir*entity->cShape->circle.getRadius())*i;
	}

	m_lastSpecialWeaponTime = m_currentFrame;
}

void Game::sMovement()
{
	//TODO: implement all entity movement in this function
	// read the m_player->cInput component to determine if player is moving

	// Makes sure player doesn't move as it relies on user input instead
	m_player->cTransform->velocity = { 0,0 };

	float moveX = 0.0f;
	float moveY = 0.0f;

	if (m_player->cInput->up) moveY += -m_playerConfig.S;
	if (m_player->cInput->down) moveY += m_playerConfig.S;;
	if (m_player->cInput->left) moveX += -m_playerConfig.S;;
	if (m_player->cInput->right) moveX += m_playerConfig.S;

	Vec2 moveDir(moveX, moveY);
	auto tempPos = m_player->cTransform->pos + (moveDir.normalize() * m_playerConfig.S);
	
	// Check if future position results in player shape leaving window and prevent that if it does
	bool withinHorizontalBounds = tempPos.x >= m_playerConfig.SR && tempPos.x < (m_window.getSize().x - m_playerConfig.SR);
	bool withinVerticalBounds = tempPos.y >= m_playerConfig.SR && tempPos.y < (m_window.getSize().y - m_playerConfig.SR);
	if (withinHorizontalBounds && withinVerticalBounds) m_player->cTransform->pos = tempPos;

	for (auto& e : m_entities.getEntities())
	{
		// Just add velocity if not enemy (aka bullets)
		if (e->tag() != "enemy")
		{
			e->cTransform->pos.x += e->cTransform->velocity.x;
			e->cTransform->pos.y += e->cTransform->velocity.y;
			continue;
		}
		// Enemies must bounce off screen 
		auto futureX = e->cTransform->pos.x + e->cTransform->velocity.x;
		auto futureY = e->cTransform->pos.y + e->cTransform->velocity.y;
		bool withinHorizontalBounds = futureX >= m_enemyConfig.SR && futureX <= (m_window.getSize().x - m_enemyConfig.SR);
		bool withinVerticalBounds = futureY >= m_enemyConfig.SR && futureY <= (m_window.getSize().y - m_enemyConfig.SR);
		if (!withinHorizontalBounds) e->cTransform->velocity.x *= -1.0f;
		if (!withinVerticalBounds) e->cTransform->velocity.y *= -1.0f;
		e->cTransform->pos.x = futureX;
		e->cTransform->pos.y = futureY;
	}
}

void Game::sLifespan()
{
	//TODO: implement all lifespan functionality
	// for all entities
	//	if no lifespan component, skip
	//	if entity >0 lifespan, -1
	//	if has lifespan and alive,
	//		scale alpha channel property (opacity)
	//	if has lifespan and time up
	//		destroy entity
	for (auto& e : m_entities.getEntities())
	{
		if (e->cLifespan == NULL) continue;
		if (e->cLifespan->remaining > 0)
		{
			e->cLifespan->remaining -= 1;

			auto& frgba = e->cShape->circle.getFillColor();
			auto& orgba = e->cShape->circle.getOutlineColor();
			float hp = ((float)(e->cLifespan->remaining) / (float)(e->cLifespan->total)) * 255.0f;
			
			//std::cout << "Health: " << e->cLifespan->remaining << "/" << e->cLifespan->total << "\n";
			e->cShape->circle.setFillColor(sf::Color(frgba.r, frgba.g, frgba.b, hp));
			e->cShape->circle.setOutlineColor(sf::Color(orgba.r, orgba.g, orgba.b, hp));
			continue;
		}
		// If remaining lifespan<=0, destroy entity
		e->destroy();
	}

}

void Game::sCollision()
{
	// TODO: Implement all proper collision between entities
	// use collision radius, not shape radius

	// Collision between player and enemy
	for (auto& e : m_entities.getEntities("enemy"))
	{
		auto& enemyPos = e->cShape->circle.getPosition();
		auto& playerPos = m_player->cShape->circle.getPosition();
		// Convert to Vec2 defined in project instead of SFML
		auto ep = Vec2(enemyPos.x, enemyPos.y);
		auto pp = Vec2(playerPos.x, playerPos.y);
		if (pp.dist(ep) <= (m_enemyConfig.CR + m_playerConfig.CR))
		{
			//std::cout << "Collision detected!!\n";
			m_player->destroy();
			e->destroy();
			spawnSmallEnemies(e);
			spawnPlayer();
		}
	}

	for (auto& e : m_entities.getEntities("smallEnemies"))
	{
		auto& enemyPos = e->cShape->circle.getPosition();
		auto& playerPos = m_player->cShape->circle.getPosition();
		// Convert to Vec2 defined in project instead of SFML
		auto ep = Vec2(enemyPos.x, enemyPos.y);
		auto pp = Vec2(playerPos.x, playerPos.y);
		if (pp.dist(ep) <= (m_enemyConfig.CR + m_playerConfig.CR))
		{
			if (!e->isActive())continue;
			std::cout << "Score +" << e->cShape->circle.getPointCount() * 5 << "\n";
			m_player->cScore->score += e->cShape->circle.getPointCount() * 5;
			e->destroy();
		}
	}
	
	// Collision between bullet and enemy
	for (auto& b : m_entities.getEntities("bullet"))
	{
		for (auto& e : m_entities.getEntities("enemy"))
		{
			if (!e->isActive())continue;
			auto& bulletPos = b->cShape->circle.getPosition();
			auto& enemyPos = e->cShape->circle.getPosition();
			auto bp = Vec2(bulletPos.x, bulletPos.y);
			auto ep = Vec2(enemyPos.x, enemyPos.y);

			if (bp.dist(ep) <= (m_enemyConfig.CR + m_bulletConfig.CR))
			{
				//std::cout << "Collision detected!!\n";
				std::cout << "Score +" << e->cShape->circle.getPointCount() * 100 << "\n";
				m_player->cScore->score += e->cShape->circle.getPointCount() * 100;

				b->destroy();
				e->destroy();
				spawnSmallEnemies(e);
			}
		}
	}
}

void Game::sEnemySpawner()
{
	// TODO: implement enemy spawning here
	// use (m_currentFrame - m_lastEnemySpawnTime) to find how long it has been since last enemy spawned
	int timeSinceLastEnemySpawned = m_currentFrame - m_lastEnemySpawnTime;
	// replace 60 with interval specified in config
	if (timeSinceLastEnemySpawned > m_enemyConfig.SI) spawnEnemy();
}

void Game::sRender()
{
	// TODO: change code below to draw all entities
	// sample drawing of player entity created above
	m_window.clear();

	// draw entity's sf::CircleShape
	for (auto& e : m_entities.getEntities())
	{
		// set pos of shape based on entity transform pos
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		// set rotation of shape based on entity transform angle
		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		// draw entities's sf::CircleShape
		m_window.draw(e->cShape->circle);
	}
	int timeSinceLastSpecial = m_currentFrame - m_lastSpecialWeaponTime;
	m_player->cShape->circle.setOutlineColor(sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB, timeSinceLastSpecial > 120 ? 255 : timeSinceLastSpecial / 120.0f * 255.0f));
	m_text.setString("SCORE: " + std::to_string(m_player->cScore->score));
	m_window.draw(m_text);
	m_window.display();
}

void Game::sUserInput()
{
	// TODO: handle user input here
	// only set player's input component variables here
	// do not implement player movement logic here
	// movement system will read variables set in this functionm

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// event triggered when window closed
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		// event triggered when key pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			// close game when click esc
			case sf::Keyboard::Escape:
				m_running = false;
				break;
			// pause game when p clicked
			case sf::Keyboard::P:
				//std::cout << "P pressed\n";
				setPaused(!m_paused);
				break;
			case sf::Keyboard::W:
				//std::cout << "W pressed\n";
				m_player->cInput->up = true;
				//TODO: set player input component up to true
				break;
			case sf::Keyboard::A:
				//std::cout << "A pressed\n";
				m_player->cInput->left = true;
				//TODO: set player input component left to true
				break;
			case sf::Keyboard::S:
				//std::cout << "S pressed\n";
				m_player->cInput->down = true;
				//TODO: set player input component down to true
				break;
			case sf::Keyboard::D:
				//std::cout << "D pressed\n";
				m_player->cInput->right = true;
				//TODO: set player input component right to true
				break;
			default: break;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				//std::cout << "W released\n";
				m_player->cInput->up = false;
				//TODO: Set player input component up to false
				break;
			case sf::Keyboard::A:
				//std::cout << "A released\n";
				m_player->cInput->left = false;
				//TODO: Set player input component left to false
				break;
			case sf::Keyboard::S:
				//std::cout << "S released\n";
				m_player->cInput->down = false;
				//TODO: Set player input component down to false
				break;
			case sf::Keyboard::D:
				//std::cout << "D released\n";
				m_player->cInput->right = false;
				//TODO: Set player input component right to false
				break;
			default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//std::cout << "Left mouse button clicked at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")\n";
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				//std::cout<<"Right mouse button clicked at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")\n";
				// call spawnSpecialWeapon here
				int timeSinceLastSpecial = m_currentFrame - m_lastSpecialWeaponTime;
				if (timeSinceLastSpecial > 120)  // Usable every 2s
				{
					spawnSpecialWeapon(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
				}
				else std::cout << "Wait " << timeSinceLastSpecial / 60.0f << "s before lauching another special attack!\n";
			}
		}
	}
}