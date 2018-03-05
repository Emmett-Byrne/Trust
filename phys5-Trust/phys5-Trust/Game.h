// author Peter Lowe
#ifndef GAME
#define GAME

#include <SFML/Graphics.hpp>
#include <time.h>

struct Bullets
{
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	bool active;
	sf::CircleShape m_body;
};

class Game
{
public:
	Game();
	~Game();
	/// <summary>
	/// main method for game
	/// </summary>
	void run();

private:

	double const DEG_TO_RAD = 3.14159265359 / 180;
	float pixelsToMeters = 20;
	int const NO_BULLETS = 50;

	void processEvents();
	void update(sf::Time t_deltaTime);
	void render();
	
	bool isOffscreen(sf::Vector2f pos);

	void reset();

	float length(sf::Vector2f vec);

	void setupFontAndText();

	sf::RenderWindow m_window; // main SFML window
	sf::Font m_ArialBlackfont; // font used by message
	bool m_exitGame; // control exiting game

	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_gravity;
	sf::Vector2f m_targetPos;
	float m_angle;
	float m_trust;
	float m_delay;
	int m_lives;
	int m_score;

	sf::CircleShape m_target;
	sf::RectangleShape m_player;
	sf::RectangleShape m_ground;

	Bullets m_bullets[50];
};

#endif // !GAME

