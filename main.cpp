#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Simulation");

    sf::CircleShape particle1(5.f);
    particle1.setFillColor(sf::Color::Red);
    particle1.setPosition(400.f, 300.f);
	sf::Vector2f velocity1(0.1f, 0.1f);



    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

		sf::Vector2f pos1 = particle1.getPosition();
		pos1 += velocity1;
		particle1.setPosition(pos1);

		if (pos1.x < 0 || pos1.x > window.getSize().x - particle1.getRadius() * 2) {
			velocity1.x = -velocity1.x;
		}
		if (pos1.y < 0 || pos1.y > window.getSize().y - particle1.getRadius() * 2) {
			velocity1.y = -velocity1.y;
		}

        window.clear();
        window.draw(particle1);
        window.display();
    }

    return 0;
}


