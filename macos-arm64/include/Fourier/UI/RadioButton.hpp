
#ifndef _UI_RADIO_GROUP_HPP_
#define _UI_RADIO_GROUP_HPP_

#include <Fourier/UI/Base.hpp>

#include <vector>
#include <SFML/Graphics.hpp>

class UIRadioButton : public UIBase {
public:
    UIRadioButton(const std::string &text = "", bool selected = false);
    void setHovered(bool flag);
    bool isHovered() const;
    void setSelected(bool flag);
    bool isSelected() const;
    void setTextOffset(float offset);
    sf::FloatRect getHoverRect() const;
    void onMouseMove(const sf::RenderWindow &window);
    void onMouseClick(const sf::RenderWindow &window);
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
private:
    float m_x, m_y;
    float m_offset;
    bool m_hovered, m_selected;
    std::string m_text;
    sf::Sprite m_radioButtonSprite;
};

class UIRadioGroup : public UIBase {
public:
    using radioButtonActionPerformer = void (*)(int oldActiveOption, int newActiveOption);

    UIRadioGroup(radioButtonActionPerformer performer = nullptr);
    void addRadioButton(UIRadioButton &radioButton, const sf::Vector2f &vec);
    int getSelectedOption() const;
    void onMouseMove(const sf::RenderWindow &window);
    void onMouseClick(const sf::RenderWindow &window);
    void setPosition(const sf::Vector2f &pos);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
private:
    std::vector<std::pair<UIRadioButton *, sf::Vector2f>> m_radioButtons;
    float m_x, m_y;
    int m_activeOption;
    // int _hoveredOption;
    radioButtonActionPerformer m_actionPerformer;
};

#endif
