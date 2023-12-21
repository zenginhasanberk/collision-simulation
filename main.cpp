#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <utility>

Mix_Chunk *initializeSoundEffects();

const double PI = M_PI;
const size_t NUM_PARTICLES = 10;
const bool COLLISIONS = true;

/* Initialize sound effects */
Mix_Chunk *collisionSound = initializeSoundEffects();

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
    sf::RenderWindow *window;  // for boundary check in update

   public:
    static Particle createRandomParticle(sf::RenderWindow &window) {
        sf::Color colors[7] = {sf::Color::White, sf::Color::Red,    sf::Color::Green,
                               sf::Color::Blue,  sf::Color::Yellow, sf::Color::Magenta,
                               sf::Color::Cyan};

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distrib(0, 1);

        int r = 30 + std::ceil(distrib(gen) * 30);
        std::pair<float, float> spos = {distrib(gen) * (window.getSize().x - 2 * r),
                                        distrib(gen) * (window.getSize().y - 2 * r)};
        std::pair<float, float> svel = {distrib(gen) / 25, distrib(gen) / 25};

        sf::Color color = colors[(int)std::floor(distrib(gen) * 7)];

        return Particle(r, spos, svel, color, &window);
    }

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
             sf::RenderWindow *_window)
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

        if (pos.x < 0 || pos.x > window->getSize().x - radius * 2) {
            velocity.x = -velocity.x;

            // Sound effect
            Mix_PlayChannel(-1, collisionSound, 0);
        }
        if (pos.y < 0 || pos.y > window->getSize().y - radius * 2) {
            velocity.y = -velocity.y;

            // Sound effect
            Mix_PlayChannel(-1, collisionSound, 0);
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
    sf::RenderWindow window(sf::VideoMode(800, 600), "Collision Simulation");
    std::vector<Particle> particles;

    for (size_t i = 0; i < NUM_PARTICLES; ++i) {
        bool collision;

        do {
            collision = false;
            Particle p = Particle::createRandomParticle(window);

            for (int j = 0; j < particles.size(); ++j) {
                if (p.collidesWith(particles[j])) {
                    collision = true;
                    break;
                }
            }

            if (!collision) {
                particles.push_back(p);
            }

        } while (collision);
    }

    while (window.isOpen()) {
        sf::Event event;

        // TODO delete this?
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        for (size_t i = 0; i < NUM_PARTICLES; ++i) {
            particles[i].update();
        }

        // It is possible to turn off particle collisions
        if (COLLISIONS) {
            // Standard collision detection solution O(n ^ 2)
            for (size_t i = 0; i < NUM_PARTICLES; ++i) {
                for (size_t j = i + 1; j < NUM_PARTICLES; ++j) {
                    if (particles[i].collidesWith(particles[j])) {
                        sf::Vector2f newVel1 =
                            particles[i].velocityAfterCollisionWith(particles[j]);
                        sf::Vector2f newVel2 =
                            particles[j].velocityAfterCollisionWith(particles[i]);

                        particles[i].setVelocity(newVel1);
                        particles[j].setVelocity(newVel2);

                        // Sound effect
                        Mix_PlayChannel(-1, collisionSound, 0);
                    }
                }
            }

            /* TODO: Implement optimized collision detection algorithm!
            Sweep and Prune or Uniform Grid Partitioning algorithm? */

            window.clear();

            for (size_t i = 0; i < NUM_PARTICLES; ++i) {
                window.draw(particles[i].getDrawable());
            }

            window.display();
        }
    }
    Mix_FreeChunk(collisionSound);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}

Mix_Chunk *initializeSoundEffects() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL initialization failed!";
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Audio initialization failed!";
        exit(1);
    }

    Mix_Chunk *collisionSound = Mix_LoadWAV("./Collision.wav");
    if (collisionSound == NULL) {
        std::cout << "Error loading collision sound!";
        exit(1);
    }

    Mix_VolumeChunk(collisionSound, MIX_MAX_VOLUME / 2);

    return collisionSound;
}