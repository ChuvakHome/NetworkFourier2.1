
#ifndef _UI_MANAGER_HPP_
#define _UI_MANAGER_HPP_

#include <unordered_map>
#include <SFML/Graphics.hpp>

enum class UIElement {
    RadioButtonInactive,
    RadioButtonActive,

    CheckboxInactive,
    CheckboxActive,

    Cursor,

    ArrowLeft,
    ArrowUp,
    ArrowRight,
    ArrowDown,
};

class UIManager {
public:
    void initManager();
    void scaleSymbols(const sf::Vector2f &scale);
    sf::FloatRect getSymbolBounds(void) const;
    sf::Sprite getSpriteByElementType(UIElement type);
    sf::Sprite getSpriteByCharacter(char character) const;
    bool isSymbolPrintable(char symb) const;
private:
    sf::Sprite m_elementSprites[9];
    sf::Sprite m_characterSprites[256];
    sf::FloatRect m_symbolBounds;
};

#endif
