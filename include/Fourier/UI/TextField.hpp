
#ifndef _UI_TEXT_FIELD_HPP_
#define _UI_TEXT_FIELD_HPP_

#include <Fourier/UI/Cursor.hpp>
#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>

class UITextField : public UIBase {
public:
    UITextField(std::size_t maxSymbolsCount = 16, bool focused = false);
    void setText(const std::string &text);
    std::string getText() const;
    void addSymbol(char symb);
    void removeSymbol();
    void clearText();
    void setFocused(bool focused);
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void setCursorDelayVisible(const sf::Time &delayVisible);
    void setCursorDelayInvisible(const sf::Time &delayInvisible);
    void setCursorDelay(const sf::Time &delay);
    void update(const sf::RenderWindow &window);
    void onMouseClick(const sf::RenderWindow &window);
    void draw(sf::RenderTarget &target, sf::RenderStates state) const;
private:
    std::string m_text;
    std::size_t m_maxSymbols;
    sf::RectangleShape m_fieldRect;
    UICursor m_fieldCursor;
    float m_x, m_y;
    bool m_focused;
};

#endif
