
#include <Fourier/UI/Manager.hpp>

#include <cstdlib>
#include <cstring>

#include <Fourier/Defines.hpp>
#include <Fourier/Global.hpp>

#ifdef SFML_SYSTEM_WINDOWS
    #include <conio.h>
#else
    #include <unistd.h>
#endif

#include <SFML/Graphics.hpp>

static const char charset[6][100] = {
    " !\"#$%&`()*+,-./",
    "0123456789:;<=>?",
    "@ABCDEFGHIJKLMNO",
    "PQRSTUVWXYZ[\\]^_",
    "'abcdefghijklmno",
    "pqrstuvwxyz{|}\""
};

template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

void UIManager::initManager() {
    static sf::Texture charsTexture;

    // char buf[1024];
    // memset(buf, 0, 1024);
    // getcwd(buf, 1024);

    // std::cout << buf << std::endl;

    charsTexture.loadFromFile(getResourcePath("data/chars.tga"));

    m_elementSprites[to_underlying(UIElement::CheckboxInactive)] = sf::Sprite(charsTexture, sf::IntRect(128 - 4 * 8, 0, 8, 8));
    m_elementSprites[to_underlying(UIElement::CheckboxActive)] = sf::Sprite(charsTexture, sf::IntRect(128 - 5 * 8, 0, 8, 8));
    m_elementSprites[to_underlying(UIElement::RadioButtonInactive)] = sf::Sprite(charsTexture, sf::IntRect(128 - 6 * 8, 0, 8, 8));
    m_elementSprites[to_underlying(UIElement::RadioButtonActive)] = sf::Sprite(charsTexture, sf::IntRect(128 - 7 * 8, 0, 8, 8));
    m_elementSprites[to_underlying(UIElement::Cursor)] = sf::Sprite(charsTexture, sf::IntRect(128 - 8 * 8, 0, 8, 8));

    m_elementSprites[to_underlying(UIElement::ArrowUp)] = sf::Sprite(charsTexture, sf::IntRect(112, 8, 8, 8));
    m_elementSprites[to_underlying(UIElement::ArrowDown)] = sf::Sprite(charsTexture, sf::IntRect(120, 8, 8, 8));
    m_elementSprites[to_underlying(UIElement::ArrowLeft)] = sf::Sprite(charsTexture, sf::IntRect(8, 8, 8, 8));
    m_elementSprites[to_underlying(UIElement::ArrowRight)] = sf::Sprite(charsTexture, sf::IntRect(0, 8, 8, 8));

    for (std::size_t i = 0; i < sizeof(charset) / sizeof(charset[0]); ++i) {
        for (std::size_t j = 0; j < strlen(charset[i]); ++j)
            m_characterSprites[static_cast<int>(charset[i][j])] = sf::Sprite(charsTexture, sf::IntRect(j << 3, (i + 2) << 3, 8, 8));
    }

    m_symbolBounds = m_characterSprites[static_cast<int>('0')].getLocalBounds();
}

void UIManager::scaleSymbols(const sf::Vector2f &scale) {
    m_elementSprites[to_underlying(UIElement::CheckboxInactive)].setScale(scale);
    m_elementSprites[to_underlying(UIElement::CheckboxActive)].setScale(scale);
    m_elementSprites[to_underlying(UIElement::RadioButtonInactive)].setScale(scale);
    m_elementSprites[to_underlying(UIElement::RadioButtonActive)].setScale(scale);
    m_elementSprites[to_underlying(UIElement::Cursor)].setScale(scale);

    m_elementSprites[to_underlying(UIElement::ArrowUp)].setScale(scale);
    m_elementSprites[to_underlying(UIElement::ArrowDown)].setScale(scale);
    m_elementSprites[to_underlying(UIElement::ArrowLeft)].setScale(scale);
    m_elementSprites[to_underlying(UIElement::ArrowRight)].setScale(scale);

    for (std::size_t i = 0; i < sizeof(charset) / sizeof(charset[0]); ++i) {
        for (std::size_t j = 0; j < strlen(charset[i]); ++j)
            m_characterSprites[static_cast<int>(charset[i][j])].scale(scale);
    }

    m_symbolBounds = m_characterSprites[static_cast<int>('0')].getLocalBounds();
}

sf::FloatRect UIManager::getSymbolBounds(void) const {
    return m_symbolBounds;
}

bool UIManager::isSymbolPrintable(char symb) const {
    for (std::size_t i = 0; i < sizeof(charset) / sizeof(charset[0]); ++i) {
        for (std::size_t j = 0; j < strlen(charset[i]); ++j) {
            if (charset[i][j] == symb)
                return true;
        }
    }

    return false;
}

sf::Sprite UIManager::getSpriteByElementType(UIElement type) {
    return m_elementSprites[to_underlying(type)];
}

sf::Sprite UIManager::getSpriteByCharacter(char character) const {
    return m_characterSprites[isSymbolPrintable(character) ? static_cast<int>(character) : ' '];
}
