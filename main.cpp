#include <SFML/Graphics.hpp>
#include <cmath>
#include <utility>

const double PI = M_PI;

float dotProduct(const sf::Vector2f &vec1, const sf::Vector2f &vec2) {
    return vec1.x * vec2.x + vec1.y * vec2.y;
}

float getDistance(const sf::Vector2f &pos1, const sf::Vector2f &pos2) {
    sf::Vector2f diff = pos2 - pos1;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

float getMagnitude(const sf::Vector2f &vec) {
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

float square(float val) { return val * val; }

class Particle {
   private:
    sf::CircleShape particle;
    sf::Vector2f velocity;
    sf::RenderWindow &window;  // for boundary check in update

   public:
    sf::CircleShape &getDrawable() { return particle; }

    /* IMPORTANT:
    My getPosition returns the center of the circle.
    SF2 implementation returns the top left of the shape */
    sf::Vector2f getPosition() const {
        return sf::Vector2f(particle.getPosition().x + particle.getRadius(),
                            particle.getPosition().y + particle.getRadius());
    }
    float getRadius() const { return particle.getRadius(); }
    void setVelocity(const sf::Vector2f &newVelocity) { velocity = newVelocity; }

    Particle(float radius, std::pair<float, float> startingPosition,
             std::pair<float, float> startingVelocity, sf::Color color,
             sf::RenderWindow &_window)
        : particle(sf::CircleShape(radius)),
          velocity(sf::Vector2f(startingVelocity.first, startingVelocity.second)),
          window(_window) {
        particle.setFillColor(color);
        particle.setPosition(startingPosition.first, startingPosition.second);
    }

    void update() {
        particle.setPosition(particle.getPosition() + velocity);

        sf::Vector2f pos = particle.getPosition();
        float radius = particle.getRadius();

        if (pos.x < 0 || pos.x > window.getSize().x - radius * 2) {
            velocity.x = -velocity.x;
        }
        if (pos.y < 0 || pos.y > window.getSize().y - radius * 2) {
            velocity.y = -velocity.y;
        }
    }

    bool collidesWith(const Particle &other) {
        float radiusSum = getRadius() + other.getRadius();
        float euclideanDist = getDistance(getPosition(), other.getPosition());

        return euclideanDist <= radiusSum;
    }

    sf::Vector2f velocityAfterCollisionWith(const Particle &other) const {
        float mass = PI * getRadius() * getRadius();
        float otherMass = PI * other.getRadius() * other.getRadius();

        sf::Vector2f velocityDiff = velocity - other.velocity;
        sf::Vector2f centerDist = getPosition() - other.getPosition();

        return velocity - ((2 * otherMass) / (mass + otherMass)) *
                              (dotProduct(velocityDiff, centerDist) /
                               square(getMagnitude(centerDist))) *
                              centerDist;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Simulation");

    Particle p1(50, {400.0, 300.0}, {0.01, 0.02}, sf::Color::Red, window);
    Particle p2(100, {200.0, 100.0}, {0.02, 0.01}, sf::Color::Blue, window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        p1.update();
        p2.update();

        // if particles are closer than their radius sum, then they hit each other!
        if (p1.collidesWith(p2)) {
            sf::Vector2f newVel1 = p1.velocityAfterCollisionWith(p2);
            sf::Vector2f newVel2 = p2.velocityAfterCollisionWith(p1);

            p1.setVelocity(newVel1);
            p2.setVelocity(newVel2);
        }

        window.clear();
        window.draw(p1.getDrawable());
        window.draw(p2.getDrawable());
        window.display();
    }

    return 0;
}
