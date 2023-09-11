
#ifndef _UI_CURSOR_HPP_
#define _UI_CURSOR_HPP_

#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>

#define VISIBLE_TIME_MILLIS 500
#define INVISIBLE_TIME_MILLIS 70

class UICursor : public UIBase {
public:
    UICursor(const sf::Time &delayVisible = sf::milliseconds(VISIBLE_TIME_MILLIS), const sf::Time &delayInvisible = sf::milliseconds(INVISIBLE_TIME_MILLIS));

    UICursor(const sf::Time &delay, sf::Sprite &sprite);
    UICursor(const sf::Time &delayVisible, const sf::Time &delayInvisible, sf::Sprite &sprite);

    UICursor(const sf::Time &delay, sf::Texture &texture);
    UICursor(const sf::Time &delayVisible, const sf::Time &delayInvisible, sf::Texture &texture);

    UICursor(const sf::Time &delay, sf::Texture &texture, sf::IntRect rect);
    UICursor(const sf::Time &delayVisible, const sf::Time &delayInvisible, sf::Texture &texture, sf::IntRect rect);

    UICursor(const sf::Time &delay, const std::string &filepath, sf::IntRect rect);
    UICursor(const sf::Time &delayVisible, const sf::Time &delayInvisible, const std::string &filepath, sf::IntRect rect);

    void setDelayVisible(const sf::Time &delayVisible);
    sf::Time getDelayVisible() const;

    void setDelayInvisible(const sf::Time &delayInvisible);
    sf::Time getDelayInvisible() const;

    void setDelay(const sf::Time &delay);

    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void setSprite(sf::Sprite &sprite);
    void update(const sf::RenderWindow &window);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

private:
    sf::Clock m_clock;
    sf::Time m_delayVisible;
    sf::Time m_delayInvisible;
    bool m_visible;
    sf::Texture m_cursorTexture;
    sf::Sprite m_cursorSprite;
};

#endif
