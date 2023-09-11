
#ifndef _UI_DROPDOWN_HPP_
#define _UI_DROPDOWN_HPP_

#include <vector>

#include <Fourier/UI/Base.hpp>

#include <SFML/Graphics.hpp>

template<typename T>
class UIDropdown : public UIBase {
public:
    struct replacement {
        T oldval;
        T newval;
    };

    using dropdownActionPerformer = void (*)(const replacement repl, sf::RenderWindow &window);

    UIDropdown(dropdownActionPerformer actionPerformer = nullptr, std::size_t maxSymbols = 5);
    void setSize(const sf::Vector2f &size);
    sf::Vector2f getSize() const;
    void setMaxSymbolsCount(std::size_t count);
    void addOption(const std::string &text, const T &option);
    void selectOption(int option);
    T getSelectedOption(void) const;
    void onKeyPressed(sf::RenderWindow &window);
    void onMouseMove(sf::RenderWindow &window);
    void onMouseClick(sf::RenderWindow &window);
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
private:
    float m_x, m_y;
    std::size_t m_maxSymbols;
    std::size_t m_listOption, m_activeOption;
    int m_hoverState;
    bool m_optionListVisible;
    sf::RectangleShape m_textRect;
    sf::RectangleShape m_buttonRect;
    std::vector<std::pair<std::string, T>> m_options;
    dropdownActionPerformer m_dropdownActionPerformer;
};

#endif
