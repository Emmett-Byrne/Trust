// author Peter Lowe

#include "Game.h"
#include <iostream>



Game::Game() :
	m_window{ sf::VideoMode{ 800, 600, 32 }, "SFML Game" },
	m_exitGame{false},
	m_angle{ 90 },
	m_trust{ 50 },
	m_position{ 400,300 },
	m_velocity{ 0,0 },
	m_gravity{ 0, 9.8 * pixelsToMeters },
	m_lives{ 5 },
	m_score{ 0 },
	m_delay{ 0 }
{
	setupFontAndText(); // load font 

	m_player.setFillColor(sf::Color::Green);
	m_player.setSize(sf::Vector2f(pixelsToMeters, pixelsToMeters));
	m_player.setOrigin(pixelsToMeters / 2, pixelsToMeters /2 );
	m_player.setPosition(m_position);

	for (int i = 0; i < NO_BULLETS; i++)
	{
		m_bullets[i].m_position = sf::Vector2f(400, 300);
		m_bullets[i].m_velocity = sf::Vector2f(0, 0);
		m_bullets[i].active = false;

		m_bullets[i].m_body.setFillColor(sf::Color::Cyan);
		m_bullets[i].m_body.setRadius(pixelsToMeters / 10);
		m_bullets[i].m_body.setOrigin(pixelsToMeters / 10, pixelsToMeters / 10);
		m_bullets[i].m_body.setPosition(m_bullets[i].m_position);
	}


	m_target.setFillColor(sf::Color::Blue);
	m_target.setRadius(pixelsToMeters / 2);
	m_target.setOrigin(pixelsToMeters / 2, pixelsToMeters / 2);
	m_target.setPosition(m_targetPos);

	m_ground.setFillColor(sf::Color::Red);
	m_ground.setSize(sf::Vector2f(800, 50));
	m_ground.setPosition(0, 550);
}


Game::~Game()
{
}


void Game::run()
{
	srand(time(NULL));

	m_targetPos = sf::Vector2f(rand() % 600 + 100, rand() % 400 + 50);

	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame = sf::seconds(1.f / 60.f); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame); //60 fps
		}
		render(); // as many as possible
	}
}
/// <summary>
/// handle user and system events/ input
/// get key presses/ mouse moves etc. from OS
/// and user :: Don't do game update here
/// </summary>
void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if ( sf::Event::Closed == event.type) // window message
		{
			m_window.close();
		}
		if (sf::Event::KeyPressed == event.type) //user key press
		{
			if (sf::Keyboard::Escape == event.key.code)
			{
				m_exitGame = true;
			}
		}
	}

	

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_angle += 0.1;

		if (m_angle > 360)
		{
			m_angle -= 360;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_angle -= 0.1;

		if (m_angle < 0)
		{
			m_angle += 360;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_delay > .1)
	{
		m_delay = 0;
		for (int i = 0; i < NO_BULLETS; i++)
		{
			if (m_bullets[i].active == false)
			{
				m_bullets[i].active = true;

				m_bullets[i].m_velocity = sf::Vector2f(m_trust * cos(m_angle * DEG_TO_RAD), -(m_trust * sin(m_angle * DEG_TO_RAD))) * 20.0f;
				m_bullets[i].m_position = m_position;
				return;
			}
		}
	}
}

/// <summary>
/// Update the game world
/// </summary>
/// <param name="t_deltaTime">time interval per frame</param>
void Game::update(sf::Time t_deltaTime)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		sf::Vector2f temp = sf::Vector2f(m_trust * cos(m_angle * DEG_TO_RAD), -(m_trust * sin(m_angle * DEG_TO_RAD)));
		
		if (length(m_velocity + temp * .2f) < 300)
		{
			m_velocity += temp * .2f;
		}
	}

	m_position += m_velocity * t_deltaTime.asSeconds() + .5f * m_gravity * (t_deltaTime.asSeconds() * t_deltaTime.asSeconds());
	m_velocity += m_gravity * t_deltaTime.asSeconds();

	for (int i = 0; i < NO_BULLETS; i++)
	{
		if (m_bullets[i].active)
		{
			m_bullets[i].m_position += m_bullets[i].m_velocity * t_deltaTime.asSeconds();

			if (isOffscreen(m_bullets[i].m_position))
			{
				m_bullets[i].active = false;
			}
		}
	}

	if (m_position.y + pixelsToMeters > 550)
	{
		m_lives--;
		reset();
	}

	for (int i = 0; i < NO_BULLETS; i++)
	{
		if (m_bullets[i].active && length(m_targetPos - m_bullets[i].m_position) < m_target.getRadius() + m_bullets[i].m_body.getRadius())
		{
			//std::cout << "hit" << std::endl;
			m_targetPos = sf::Vector2f(rand() % 600 + 100, rand() % 400 + 50);
			m_bullets[i].active = false;

			m_score++;
		}
	}

	if (m_player.getGlobalBounds().intersects(m_target.getGlobalBounds()))
	{
		m_lives--;
		reset();
	}

	if (m_exitGame || m_lives < 0)
	{
		m_window.close();
	}

	m_delay += t_deltaTime.asSeconds();
	 
	std::cout << "Lives: " << m_lives << std::endl;
	std::cout << "Score: " << m_score << std::endl;
}

/// <summary>
/// draw the frame and then switch bufers
/// </summary>
void Game::render()
{
	m_window.clear(sf::Color::Black);

	m_target.setPosition(m_targetPos);
	m_window.draw(m_target);

	for (int i = 0; i < NO_BULLETS; i++)
	{
		if (m_bullets[i].active)
			{
				m_bullets[i].m_body.setPosition(m_bullets[i].m_position);
				m_window.draw(m_bullets[i].m_body);
			}
	}
	

	m_player.setPosition(m_position);
	m_player.setRotation(-m_angle);
	m_window.draw(m_player);
	m_window.draw(m_ground);

	m_window.display();
}

bool Game::isOffscreen(sf::Vector2f pos)
{
	if (pos.x < 0 || pos.x > 800 || pos.y < 0 || pos.y > 600)
	{
		return true;
	}
	
	return false;
}

void Game::reset()
{
	m_position = sf::Vector2f(400, 300);
	m_velocity = sf::Vector2f(0, 0);
	m_angle = 90;
}

float Game::length(sf::Vector2f vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y);
}

/// <summary>
/// load the font and setup the text message for screen
/// </summary>
void Game::setupFontAndText()
{
	if (!m_ArialBlackfont.loadFromFile("ASSETS\\FONTS\\ariblk.ttf"))
	{
		std::cout << "problem loading arial black font" << std::endl;
	}
}