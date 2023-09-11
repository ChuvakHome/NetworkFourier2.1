
#include <Fourier/UI/Cursor.hpp>

#include <cmath>

#include <Fourier/Global.hpp>
#include <SFML/Graphics.hpp>

UICursor::UICursor(const sf::Time &delayVisible, const sf::Time &delayInvisible) : m_delayVisible(delayVisible), m_delayInvisible(delayInvisible), m_visible(true) {
    sf::Sprite tempSprite = uiManager.getSpriteByElementType(UIElement::Cursor);
    m_cursorSprite = tempSprite;
}

UICursor::UICursor(const sf::Time &delay, sf::Sprite &sprite)
    : m_delayVisible(delay), m_delayInvisible(delay), m_visible(true), m_cursorSprite(sprite) {}

UICursor::UICursor(const sf::Time &delayVisible, const sf::Time &delayInvisible, sf::Sprite &sprite)
    : m_delayVisible(delayVisible), m_delayInvisible(delayInvisible), m_visible(true), m_cursorSprite(sprite) {}

UICursor::UICursor(const sf::Time &delay, sf::Texture &texture)
    : m_delayVisible(delay), m_delayInvisible(delay), m_visible(true), m_cursorSprite(texture) {}

UICursor::UICursor(const sf::Time &delayVisible, const sf::Time &delayInvisible, sf::Texture &texture)
    : m_delayVisible(delayVisible), m_delayInvisible(delayInvisible), m_visible(true), m_cursorTexture(texture), m_cursorSprite(m_cursorTexture) {}

UICursor::UICursor(const sf::Time &delay, sf::Texture &texture, sf::IntRect rect)
    : m_delayVisible(delay), m_delayInvisible(delay), m_visible(true), m_cursorTexture(texture), m_cursorSprite(m_cursorTexture) {
    m_cursorSprite.setTextureRect(rect);
}

UICursor::UICursor(const sf::Time &delayVisible, const sf::Time &delayInvisible, sf::Texture &texture, sf::IntRect rect)
    : m_delayVisible(delayVisible), m_delayInvisible(delayInvisible), m_visible(true), m_cursorTexture(texture), m_cursorSprite(m_cursorTexture) {
    m_cursorSprite.setTextureRect(rect);
}

UICursor::UICursor(const sf::Time &delay, const std::string &filepath, sf::IntRect rect) : m_delayVisible(delay), m_delayInvisible(delay), m_visible(true) {
    m_cursorTexture.loadFromFile(filepath);
    m_cursorSprite.setTexture(m_cursorTexture);
    m_cursorSprite.setTextureRect(rect);
}

UICursor::UICursor(const sf::Time &delayVisible, const sf::Time &delayInvisible, const std::string &filepath, sf::IntRect rect)
    : m_delayVisible(delayVisible), m_delayInvisible(delayInvisible), m_visible(true) {
    m_cursorTexture.loadFromFile(filepath);
    m_cursorSprite.setTexture(m_cursorTexture);
    m_cursorSprite.setTextureRect(rect);
}

void UICursor::setDelayVisible(const sf::Time &delayVisible) {
    m_delayVisible = delayVisible;
}

sf::Time UICursor::getDelayVisible() const {
    return m_delayVisible;
}

void UICursor::setDelayInvisible(const sf::Time &delayInvisible) {
    m_delayInvisible = delayInvisible;
}

sf::Time UICursor::getDelayInvisible() const {
    return m_delayInvisible;
}

void UICursor::setDelay(const sf::Time &delay) {
    m_delayVisible = m_delayInvisible = delay;
}

void UICursor::setPosition(const sf::Vector2f &pos) {
    m_cursorSprite.setPosition(pos);
}

sf::Vector2f UICursor::getPosition() const {
    return m_cursorSprite.getPosition();
}

void UICursor::setSprite(sf::Sprite &sprite) {
    sf::Vector2f pos = m_cursorSprite.getPosition();
    m_cursorSprite = sprite;
    m_cursorSprite.setPosition(pos);
}

void UICursor::update(const sf::RenderWindow &window) {
    static_cast<void>(window);

    if (m_clock.getElapsedTime() >= (m_visible ? m_delayVisible : m_delayInvisible)) {
        m_visible = !m_visible;
        m_clock.restart();
    }
}

void UICursor::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    if (m_visible)
        target.draw(m_cursorSprite, state);
}
