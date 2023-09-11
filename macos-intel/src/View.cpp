
#include <Fourier/View.hpp>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include <conv/utf8_to_cp1251.h>

#include <Fourier/Global.hpp>

#include <Fourier/Model/Fourier.hpp>

#include <Fourier/UI/Button.hpp>
#include <Fourier/UI/Checkbox.hpp>
#include <Fourier/UI/Cursor.hpp>
#include <Fourier/UI/DiscreteDiagram.hpp>
#include <Fourier/UI/Dropdown.hpp>
#include <Fourier/UI/FourierDiagram.hpp>
#include <Fourier/UI/Panel.hpp>
#include <Fourier/UI/RadioButton.hpp>
#include <Fourier/UI/Spin.hpp>
#include <Fourier/UI/TextField.hpp>
#include <Fourier/UI/TextLabel.hpp>
#include <Fourier/UI/TextPanel.hpp>
#include <Fourier/Utils.hpp>

#include <NFD/nfd.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "UI/Dropdown.cpp"
#include "UI/Spin.cpp"

#define PROGRAM_NAME "Network Fourier 2.1"
#define FRAMERATE_LIMIT 50

#define INIT_WINDOW_WIDTH 800
#define INIT_WINDOW_HEIGHT 600

using logicMethodEncoder = bit_message (*)(bit_message);
using physMethodEncoder = phys_encode_result (*)(bit_message);
using physMethodDecoder = bit_message (*)(fourier_diagram, std::size_t, receiver_settings);
using logicMethodDecoder = bit_message (*)(bit_message);
using samplersFiller = void (*)(const std::size_t bits_count);

static void fillSamplersNRZ(const std::size_t bits_count);
static void fillSamplersRZ(const std::size_t bits_count);
static void fillSamplersAMI(const std::size_t bits_count);
static void fillSamplersM2(const std::size_t bits_count);

static logicMethodEncoder logicEncoders[] = {
    logic_encode_phys,
    logic_encode_super45,
    logic_encode_scrambling
};

static physMethodEncoder physEncoders[] = {
    phys_encode_nrz,
    phys_encode_rz,
    phys_encode_ami,
    phys_encode_m2
};

static physMethodDecoder physDecoders[] = {
    phys_decode_nrz,
    phys_decode_rz,
    phys_decode_ami,
    phys_decode_m2
};

static logicMethodDecoder logicDecoders[] = {
    logic_decode_phys,
    logic_decode_super45,
    logic_decode_scrambling
};

static samplersFiller samplersFillers[] = {
    fillSamplersNRZ,
    fillSamplersRZ,
    fillSamplersAMI,
    fillSamplersM2
};

struct video_mode {
    unsigned int width;
    unsigned int height;
};

static bool doExit = false;

static UIPanel mainControlPanel;
static UIButton exitButton, aboutButton, takeScreenshotButton;
static UIDropdown<video_mode> videoModeDropdown;

static UIPanel senderControlPanel;
static UITextPanel senderControlPanelText;

static UITextLabel enterMessageLabel;
static UITextField textField;
static UIButton transmitButton;

static UITextLabel spectrumHarmonicsLabel;
static UISpin<int> highestHarmonicSpin, lowestHarmonicSpin;
static UITextLabel spectrumStatsLabel;

static UIRadioGroup physMethodsRadioGroup;
static UIRadioButton methodNRZRadioButton, methodRZRadioButton, methodAMIRadioButton, methodManchesterRadioButton;

static UIRadioGroup logicMethodsRadioGroup;
static UIRadioButton physicalRadioButton, super45radioButton, scramblingRadioButton;

/* Signal panel */
static UIPanel signalPanel;

static UITextPanel signalPanelText;

static UIDiscreteDiagram signalDiagram;

/* Fourier series panel */
static UIPanel fourierSeriesPanel;
static UITextPanel fourierSeriesPanelText;
static UIFourierDiagram fourierSeriesDiagram;

/* Received Signal panel */
static UIPanel receivedSignalPanel;

static UITextPanel receivedSignalPanelText;

static UIDiscreteDiagram receivedSignalDiagram;

/* State panel */
static UIPanel statePanel;

static UITextPanel statePanelText;

static UITextLabel msgLabel;

static UITextLabel hexMsgLabel, lengthLabel, bitrateLabel, recvMsgLabel, recvHexMsgLabel, bitsCountLabel, errorCountLabel, errorLabel, showTextLabel;

static UICheckbox showFourierSeriesCheckbox, showNoiseCheckbox, showSamplersCheckbox, showVoltageCheckbox;

static UIButton resetStatsButton;

/* Receiver panel */
static UIPanel receiverPanel;

static UITextPanel receiverPanelText;

static UISpin<float> noiseSpin;
static UISpin<float> desyncSpin;
static UISpin<float> voltageSpin;

/* Footer panel */
static UIPanel footerPanel;

static sf::RectangleShape voltageRectangle;

static phys_encode_result physicalEncodeResult;

static int previousLowHarmonic, previousHighHarmonic;
static bit_message transmitedMessage;
static std::size_t totalBitsCount = 1, errorBitsCount = 0;

static discrete_diagram emptySignalDiagram;
static fourier_diagram emptyFourierDiagram;

static fourier_diagram fourierDiagram;
static fourier_diagram noiseDiagram;
static fourier_diagram fourierNoiseDiagram;

static UIPanel aboutBoxPanel;
static bool aboutBoxActive;

static void hideUngraphSymbols(char *message) {
    for (std::size_t i = 0; message[i] != 0; ++i) {
        if (!isgraph(message[i]))
            message[i] = ' ';
    }
}

static void proceedAboutClick(const sf::RenderWindow &window) {
    (void) window;

    std::cout << "Button \"About\" pressed!" << std::endl;

    aboutBoxActive = true;
}

static void drawAboutBox(sf::RenderWindow &window) {
    const char *aboutText =
    "Network Fourier 2.0\r\n"
    "\r\n"
            "coded by : Alexey Bezgodov\r\n"
            "also known as Demiurg-HG\r\n"
    "\r\n"
    "write to : demiurg-hg@yandex.ru\r\n"
    "\r\n"
    "CS Department\r\n"
    "IFMO\r\n"
    "2005\r\n"
    "\r\n"
    "\r\n"
    "Network Fourier 2.1\r\n"
    "\r\n"
            "based on : Network Fourier 2.0\r\n"
            "coded by : Timur Demenev\r\n"
    "\r\n"
    "write to : demenev.timur@yandex.ru\r\n"
    "\r\n"
    "CS Department\r\n"
    "IFMO\r\n"
    "2023\r\n"
    "\r\n"
    "click...\r\n";

    aboutBoxPanel = UIPanel(401, 250, sf::Color(0x0, 0x0, 0x0, 0x7f));

    UITextPanel aboutTitle("About", 400);
    aboutBoxPanel.addUIElement(aboutTitle, sf::Vector2f(1.0f, 1.0f));

    UITextLabel aboutTextLabel(aboutText);
    aboutBoxPanel.addUIElement(aboutTextLabel, sf::Vector2f(16.0f, 26.0f));

    aboutBoxPanel.setPositionWithChildren(sf::Vector2f(200, 150));

    window.draw(aboutBoxPanel);
}

static void proceedExitClick(const sf::RenderWindow &window) {
    (void) window;

    std::cout << "Button \"Exit\" pressed!" << std::endl;

    doExit = true;
}

static void setSpectrumStatistics() {
    float length = static_cast<float>(transmitedMessage.bits_count);
    float lowFreq = lowestHarmonicSpin.getValue() / length * 5.0f;
    float highFreq = highestHarmonicSpin.getValue() / length * 5.0f;

    char buf[128];

    print_format(buf, 128, "%2.1f /  %2.1f MHz", lowFreq, highFreq);
    spectrumStatsLabel.setText(buf);
}

static void showFourier(fourier_diagram diagram, std::size_t bits_count) {
    fourierDiagram = diagram;

    fourierSeriesDiagram.setXDrawMode(bits_count, 10);
    fourierSeriesDiagram.setDiagram(diagram);
}

static void computeFourier(phys_encode_result physResult) {
    float length = physResult.bits_count * 1.0f;

    float min = 0.0f;
    float max = length;
    float step = length / 500.0f;

    fourier_diagram diagram;
    diagram.diagram_points_count = 500;

    std::size_t i = 0;

    for (float x = min; x <= max; x += step) {
        diagram.diagram_points[i] = { x, FourierEngine::fourierSeriesFunc(x) };
        i++;
    }

    showFourier(diagram, physResult.bits_count);
}

static void recomputeFourier() {
    fourier_diagram diagram = fourierDiagram;

    for (std::size_t i = 0; i < diagram.diagram_points_count; ++i) {
        diagram_point point = diagram.diagram_points[i];
        diagram.diagram_points[i] = { point.x, FourierEngine::fourierSeriesFuncFast(point.x, point.y, previousLowHarmonic, previousHighHarmonic) };
    }

    showFourier(diagram, physicalEncodeResult.bits_count);
}

static fourier_diagram genNoise() {
    fourier_diagram diagram = fourierDiagram;

    for (std::size_t i = 0; i < diagram.diagram_points_count; ++i) {
        diagram_point point = diagram.diagram_points[i];
        diagram.diagram_points[i].y = FourierEngine::noiseFunc(point.x);
    }

    return diagram;
}

static fourier_diagram addNoise() {
    fourier_diagram diagram = fourierDiagram;

    for (std::size_t i = 0; i < diagram.diagram_points_count; ++i)
        diagram.diagram_points[i].y += noiseDiagram.diagram_points[i].y;

    return diagram;
}

static void showReceiveStatistics() {
    char buf[128];
    print_format(buf, 128, "bits count  : %zu", totalBitsCount);
    bitsCountLabel.setText(buf);

    print_format(buf, 128, "error count : %zu", errorBitsCount);
    errorCountLabel.setText(buf);

    print_format(buf, 128, "error %%     : %1.2f %%", static_cast<float>(errorBitsCount) / totalBitsCount * 100.0f);
    errorLabel.setText(buf);
}

static void receiveMessage() {
    bit_message decoded_message = physDecoders[physMethodsRadioGroup.getSelectedOption()](fourierNoiseDiagram, transmitedMessage.bits_count, { desyncSpin.getValue(), voltageSpin.getValue() });

    discrete_diagram receive_signal_diagram;
    receive_signal_diagram.diagram_points_count = decoded_message.bits_count + 1;

    receive_signal_diagram.diagram_points[0] = { 0.0f, 0.0f };

    for (std::size_t i = 0; i < decoded_message.bits_count; ++i)
        receive_signal_diagram.diagram_points[i + 1] = { static_cast<float>(i + 1), decoded_message.msg_bits[i] < 0 ? 0.0f : 2.0f * decoded_message.msg_bits[i] - 1.0f };

    // receivedSignalDiagram.setDrawMode(decoded_message.bits_count, super45radioButton.isSelected() ? 10 : 8);
    receivedSignalDiagram.setDrawMode(decoded_message.bits_count, 8);
    receivedSignalDiagram.setDiagram(receive_signal_diagram);

    for (std::size_t i = 0; i < decoded_message.bits_count; ++i) {
        if (transmitedMessage.msg_bits[i] != decoded_message.msg_bits[i])
            ++errorBitsCount;

        ++totalBitsCount;
    }

    showReceiveStatistics();

    char msgbuf[128];
    memset(msgbuf, 0, 128);

    bit_message logic_decoded_bit_message = logicDecoders[logicMethodsRadioGroup.getSelectedOption()](decoded_message);
    message decoded_printable_message = parse_decoded_message(logic_decoded_bit_message);

    string_copy(msgbuf, decoded_printable_message.msg, decoded_printable_message.length + 1);
    hideUngraphSymbols(decoded_printable_message.msg);

    char buf[128];
    print_format(buf, 128, "recv msg : '%s'", decoded_printable_message.msg);
    recvMsgLabel.setText(buf);

    print_format(buf, 128, "%02X%02X%02X%02X %02X%02X%02X%02X", msgbuf[0] & 0xFF, msgbuf[1] & 0xFF,
                                                           msgbuf[2] & 0xFF, msgbuf[3] & 0xFF,
                                                           msgbuf[4] & 0xFF, msgbuf[5] & 0xFF,
                                                           msgbuf[6] & 0xFF, msgbuf[7] & 0xFF);

    recvHexMsgLabel.setText(buf);
}

static void fillSamplersNRZ(const std::size_t bits_count) {
    fourierSeriesDiagram.clearSamplers();
    fourierSeriesDiagram.setDesync(desyncSpin.getValue());

    for (float x = 0.0f; x < bits_count; x += 1.0f)
        fourierSeriesDiagram.addSamplerX(x + 0.5f);
}

static void fillSamplersRZ(const std::size_t bits_count) {
    fourierSeriesDiagram.clearSamplers();
    fourierSeriesDiagram.setDesync(desyncSpin.getValue() * 0.5f);

    for (float x = 0.0f; x < bits_count; x += 1.0f) {
        fourierSeriesDiagram.addSamplerX(x + 0.25f);
        fourierSeriesDiagram.addSamplerX(x + 0.75f);
    }
}

static void fillSamplersAMI(const std::size_t bits_count) {
    fourierSeriesDiagram.clearSamplers();
    fourierSeriesDiagram.setDesync(desyncSpin.getValue());

    for (float x = 0.0f; x < bits_count; x += 1.0f)
        fourierSeriesDiagram.addSamplerX(x + 0.5f);
}

static void fillSamplersM2(const std::size_t bits_count) {
    fourierSeriesDiagram.clearSamplers();
    fourierSeriesDiagram.setDesync(desyncSpin.getValue() * 0.5f);

    for (float x = 0.0f; x < bits_count; x += 1.0f) {
        fourierSeriesDiagram.addSamplerX(x + 0.25f);
        fourierSeriesDiagram.addSamplerX(x + 0.75f);
    }
}

static void transmitMessage() {
    std::cout << "Message: " << textField.getText() << std::endl;

    message msg = parse_message(textField.getText().c_str());

    int low = lowestHarmonicSpin.getValue();
    int high = highestHarmonicSpin.getValue();
    FourierEngine::setHarmonics(low, high);

    char buf[256];

    std::cout << "MSG: " << msg.msg << std::endl << "LEN: " << msg.length << std::endl;

    print_format(buf, 256, "msg : '%s'", msg.msg);
    msgLabel.setText(buf);

    print_format(buf, 256, "%02X%02X%02X%02X %02X%02X%02X%02X", msg.msg[0] & 0xFF, msg.msg[1] & 0xFF, msg.msg[2] & 0xFF, msg.msg[3] & 0xFF,
                                                           msg.msg[4] & 0xFF, msg.msg[5] & 0xFF, msg.msg[6] & 0xFF, msg.msg[7] & 0xFF);

    hexMsgLabel.setText(buf);

    bit_message codedMessage = code_message(msg);
    bit_message bitmsg = logicEncoders[logicMethodsRadioGroup.getSelectedOption()](codedMessage);

    transmitedMessage = bitmsg;
    setSpectrumStatistics();

    phys_encode_result physEncodeResult = physEncoders[physMethodsRadioGroup.getSelectedOption()](bitmsg);
    physicalEncodeResult = physEncodeResult;

    std::cout << super45radioButton.isSelected() << " " << (super45radioButton.isSelected() ? 10 : 8) << std::endl;

    signalDiagram.setDrawMode(physEncodeResult.bits_count, super45radioButton.isSelected() ? 10 : 8);
    signalDiagram.setDiagram(physEncodeResult.result_diagram);

    print_format(buf, 256, "length: %zu", msg.length);

    lengthLabel.setText(buf);

    discrete_diagram discrete_fourier_diagram = physEncodeResult.result_diagram;

    std::size_t msg_len = discrete_fourier_diagram.diagram_points_count;
    int msgbuf[MAX_FOURIER_DIAGRAM_POINTS_COUNT];

    for (std::size_t i = 0; i < msg_len; ++i)
        msgbuf[i] = static_cast<int>(discrete_fourier_diagram.diagram_points[i].y);

    FourierEngine::buildFourierSeries(msgbuf, physEncodeResult.bits_count);
    computeFourier(physEncodeResult);
    receivedSignalDiagram.setShowDiagram(true);

    samplersFillers[physMethodsRadioGroup.getSelectedOption()](physEncodeResult.bits_count);

    /*if (msg.empty) {
        std::size_t pointsCount = super45radioButton.isSelected() ? 10 : 8;
        std::size_t diagramPointsCount = pointsCount + 1;

        discrete_diagram signal_diagram = emptySignalDiagram;
        signal_diagram.diagram_points_count = diagramPointsCount;

        signalDiagram.setDrawMode(pointsCount, pointsCount);
        signalDiagram.setDiagram(signal_diagram);
        receivedSignalDiagram.setShowDiagram(false);

        fourierDiagram = emptyFourierDiagram;
        fourierDiagram.diagram_points_count = diagramPointsCount;
        fourierSeriesDiagram.setXDrawMode(pointsCount, 10);

        samplersFillers[physMethodsRadioGroup.getSelectedOption()](pointsCount);
    }
    else {*/
        // char buf[256];
        //
        // std::cout << "MSG: " << msg.msg << std::endl << "LEN: " << msg.length << std::endl;
        //
        // // sprintf(buf, "msg : '%s'", msg.msg);
        // print_format(buf, 256, "msg : '%s'", msg.msg);
        // msgLabel.setText(buf);
        //
        // // sprintf(buf, "%02X%02X%02X%02X %02X%02X%02X%02X", msg.msg[0] & 0xFF, msg.msg[1] & 0xFF, msg.msg[2] & 0xFF, msg.msg[3] & 0xFF,
        // //                                                   msg.msg[4] & 0xFF, msg.msg[5] & 0xFF, msg.msg[6] & 0xFF, msg.msg[7] & 0xFF);
        // print_format(buf, 256, "%02X%02X%02X%02X %02X%02X%02X%02X", msg.msg[0] & 0xFF, msg.msg[1] & 0xFF, msg.msg[2] & 0xFF, msg.msg[3] & 0xFF,
        //                                                        msg.msg[4] & 0xFF, msg.msg[5] & 0xFF, msg.msg[6] & 0xFF, msg.msg[7] & 0xFF);
        //
        // hexMsgLabel.setText(buf);
        //
        // bit_message codedMessage = code_message(msg);
        // bit_message bitmsg = logicEncoders[logicMethodsRadioGroup.getSelectedOption()](codedMessage);
        //
        // transmitedMessage = bitmsg;
        // setSpectrumStatistics();
        //
        // phys_encode_result physEncodeResult = physEncoders[physMethodsRadioGroup.getSelectedOption()](bitmsg);
        // physicalEncodeResult = physEncodeResult;
        //
        // std::cout << super45radioButton.isSelected() << " " << (super45radioButton.isSelected() ? 10 : 8) << std::endl;
        //
        // signalDiagram.setDrawMode(physEncodeResult.bits_count, super45radioButton.isSelected() ? 10 : 8);
        // signalDiagram.setDiagram(physEncodeResult.result_diagram);
        //
        // // sprintf(buf, "length: %zu", msg.length);
        // print_format(buf, 256, "length: %zu", msg.length);
        //
        // lengthLabel.setText(buf);
        //
        // discrete_diagram discrete_fourier_diagram = physEncodeResult.result_diagram;
        //
        // std::size_t msg_len = discrete_fourier_diagram.diagram_points_count;
        // int msgbuf[MAX_FOURIER_DIAGRAM_POINTS_COUNT];
        //
        // for (std::size_t i = 0; i < msg_len; ++i)
        //     msgbuf[i] = static_cast<int>(discrete_fourier_diagram.diagram_points[i].y);
        //
        // FourierEngine::buildFourierSeries(msgbuf, physEncodeResult.bits_count);
        // computeFourier(physEncodeResult);
        // receivedSignalDiagram.setShowDiagram(true);
        //
        // samplersFillers[physMethodsRadioGroup.getSelectedOption()](physEncodeResult.bits_count);
    // }
}

static void proceedHarmonicSpinChange(const UISpin<int>::spin_change change, const sf::RenderWindow &window) {
    (void) change;
    (void) window;

    previousLowHarmonic = FourierEngine::getLowHarmonic();
    previousHighHarmonic = FourierEngine::getHighHarmonic();

    int low = lowestHarmonicSpin.getValue();
    int high = highestHarmonicSpin.getValue();
    FourierEngine::setHarmonics(low, high);

    setSpectrumStatistics();
    recomputeFourier();

    /*if (messageEmpty) {
        std::size_t pointsCount = super45radioButton.isSelected() ? 10 : 8;
        std::size_t diagramPointsCount = pointsCount + 1;

        fourierDiagram = emptyFourierDiagram;
        fourierDiagram.diagram_points_count = diagramPointsCount;

        fourierSeriesDiagram.setXDrawMode(pointsCount, 10);
        fourierSeriesDiagram.setDiagram(emptyFourierDiagram);
    }
    else {*/
        // setSpectrumStatistics();
        // recomputeFourier();
    // }
}

static void proceedDesyncSpinChange(const UISpin<float>::spin_change change, const sf::RenderWindow &window) {
    (void) window;

    switch (physMethodsRadioGroup.getSelectedOption()) {
        case 0:
        case 2:
            fourierSeriesDiagram.setDesync(change.newval);
            break;
        case 1:
        case 3:
            fourierSeriesDiagram.setDesync(change.newval * 0.5f);
            break;
        default:
            break;
    }
}

static void proceedPhysMethodChange(int oldopt, int newopt) {
    (void) oldopt;
    (void) newopt;

    std::cout << "Physical method changed" << std::endl;

    transmitMessage();
}

static void proceedLogicMethodChange(int oldopt, int newopt) {
    (void) oldopt;
    (void) newopt;

    std::cout << "Logical method changed" << std::endl;

    transmitMessage();
}

static void proceedTrasmitButtonClick(const sf::RenderWindow &window) {
    (void) window;

    std::cout << "Button \"Transmit!\" pressed!" << std::endl;

    transmitMessage();
}

static void proceedResetStatsButtonClick(const sf::RenderWindow &window) {
    (void) window;

    std::cout << "Button \"Reset stats!\" pressed!" << std::endl;

    totalBitsCount = 1;
    errorBitsCount = 0;
    showReceiveStatistics();
}

static void proceedVideoModeDropdownChanged(const UIDropdown<video_mode>::replacement repl, sf::RenderWindow &window) {
    std::cout << "Video mode changed to " << repl.newval.width << "x" << repl.newval.height << std::endl;

    sf::VideoMode desktopVideoMode = sf::VideoMode::getDesktopMode();

    float k = static_cast<float>(repl.newval.width) / static_cast<float>(repl.oldval.width);

    std::cout << "K: " << k << std::endl;

    window.setSize(sf::Vector2u(repl.newval.width, repl.newval.height));
    window.setPosition(sf::Vector2i((desktopVideoMode.width - repl.newval.width) / 2, (desktopVideoMode.height - repl.newval.height) / 2));
}

static void takeScreenshot(const sf::RenderWindow &window) {
    auto width = window.getSize().x;
    auto height = window.getSize().y;

    sf::Texture screenTexture;
    screenTexture.create(width, height);
    screenTexture.update(window);

    sf::Image screenImage = screenTexture.copyToImage();

    NFD::Init();

    nfdchar_t *outPath;
    nfdfilteritem_t filters[1] = { { "Images", "jpg,png,tiff,bmp" } };
    const nfdchar_t *defaultPath = getExternalPath().c_str();
    const nfdchar_t *defaultName = PROGRAM_NAME " Screenshot.png";
    nfdresult_t result = NFD::SaveDialog(outPath, filters, 1, defaultPath, defaultName);

    bool altFileSave = true;

    switch (result) {
        case NFD_OKAY: {
            std::cout << "Saving screenshot..." << std::endl;

            #ifdef SFML_SYSTEM_WINDOWS
                char savePath[1024];
                convertUtf8ToCp1251(outPath, savePath);
            #else
                char *savePath = outPath;
            #endif

            if (screenImage.saveToFile(savePath)) {
                std::cout << "Screenshot (" << savePath << ") saved" << std::endl;
                altFileSave = false;
            }

            NFD::FreePath(outPath);
            break;
        }
        case NFD_CANCEL:
            std::cout << "User canceled SaveDialog" << std::endl;
            altFileSave = false;
            break;
        default:
            std::cout << "Error: " << NFD::GetError() << std::endl;
            break;
    }

    NFD::Quit();

    if (altFileSave) {
        std::string screenshotFilename = PROGRAM_NAME " Screenshot";

        if (file_exists(screenshotFilename + ".png")) {
            std::size_t i;

            for (i = 1; file_exists(screenshotFilename + " " + std::to_string(i) + ".png"); ++i);

            screenshotFilename += + " " + std::to_string(i);
        }

         screenshotFilename += ".png";

        if (screenImage.saveToFile(screenshotFilename))
            std::cout << "Screenshot (" << screenshotFilename << ") alternatively saved" << std::endl;
    }
}

static void proceedTakeScreenshotClick(const sf::RenderWindow &window) {
    takeScreenshot(window);
}

static void drawVoltageRectangle(const sf::RenderWindow &window) {
    sf::Vector2f pos = fourierSeriesDiagram.getPosition();
    sf::Vector2f size = fourierSeriesDiagram.getSize();
    float maxHeight = (size.y - 10.0f) * 0.25f;
    float rectHeight = maxHeight * voltageSpin.getValue();

    sf::RectangleShape voltageRectangle(sf::Vector2f(size.x, rectHeight));
    voltageRectangle.setFillColor(sf::Color(0xff, 0xff, 0xff, 0x26));

    voltageRectangle.setPosition(sf::Vector2f(pos.x, pos.y + size.y * 0.5f - rectHeight));
    const_cast<sf::RenderWindow &>(window).draw(voltageRectangle);

    voltageRectangle.setPosition(sf::Vector2f(pos.x, pos.y + size.y * 0.5f));
    const_cast<sf::RenderWindow &>(window).draw(voltageRectangle);
}

static void init_ui_elements(void) {
    /* Main Control Panel */
    mainControlPanel = UIPanel(INIT_WINDOW_WIDTH - 10, 26);

    exitButton = UIButton("Exit", proceedExitClick);
    mainControlPanel.addUIElement(exitButton, sf::Vector2f(4.0f, 4.0f));

    aboutButton = UIButton("About", proceedAboutClick);
    mainControlPanel.addUIElement(aboutButton, sf::Vector2f(4.0f + exitButton.getWidth() + 18.0f, 4.0f));

    takeScreenshotButton = UIButton("Screenshot", proceedTakeScreenshotClick);
    mainControlPanel.addUIElement(takeScreenshotButton, sf::Vector2f(4.0f + exitButton.getWidth() + 18.0f + exitButton.getWidth() + 18.0f, 4.0f));

    videoModeDropdown = UIDropdown<video_mode>(proceedVideoModeDropdownChanged, 9);
    videoModeDropdown.addOption("800x600", { 800, 600 });
    videoModeDropdown.addOption("1024x768", { 1024, 768 });
    videoModeDropdown.addOption("1280x1024", { 1280, 1024 });
    videoModeDropdown.addOption("1600x1280", { 1600, 1200 });
    videoModeDropdown.addOption("2048x1536", { 2048, 1536 });
    mainControlPanel.addUIElement(videoModeDropdown, sf::Vector2f(670.0f, 8.0f));

    mainControlPanel.setPositionWithChildren(sf::Vector2f(5.0f, 5.0f));

    /* Sender panel */
    senderControlPanel = UIPanel(159, 289);

    senderControlPanelText = UITextPanel("Sender", 155, 10);
    senderControlPanel.addUIElement(senderControlPanelText, sf::Vector2f(2.0f, 2.0f));

    enterMessageLabel = UITextLabel("Enter Message:");
    senderControlPanel.addUIElement(enterMessageLabel, sf::Vector2f(11.0f, 23.0f));

    textField = UITextField();
    textField.setCursorDelay(sf::milliseconds(207));
    textField.setFocused(true);
    senderControlPanel.addUIElement(textField, sf::Vector2f(12.0f, 39.0f));

    transmitButton = UIButton("Transmit!", proceedTrasmitButtonClick);
    senderControlPanel.addUIElement(transmitButton, sf::Vector2f(12.0f, 64.0f));

    spectrumHarmonicsLabel = UITextLabel("Spectrum\n\rharmonics:");
    senderControlPanel.addUIElement(spectrumHarmonicsLabel, sf::Vector2f(11.0f, 93.0f));

    highestHarmonicSpin = UISpin<int>(0, 255, 1,10, "highest:", proceedHarmonicSpinChange);
    highestHarmonicSpin.setTextOffset(4);
    highestHarmonicSpin.setValue(128);
    senderControlPanel.addUIElement(highestHarmonicSpin, sf::Vector2f(91.0f, 114.0f));

    lowestHarmonicSpin = UISpin<int>(0, 255, 1,10, "lowest:", proceedHarmonicSpinChange);
    lowestHarmonicSpin.setTextOffset(13);
    lowestHarmonicSpin.setValue(0);
    senderControlPanel.addUIElement(lowestHarmonicSpin, sf::Vector2f(91.0f, 131.0f));

    spectrumStatsLabel = UITextLabel("0.0 /  80.0 MHz");
    senderControlPanel.addUIElement(spectrumStatsLabel, sf::Vector2f(25.0f, 156.0f));

    physMethodsRadioGroup = UIRadioGroup(proceedPhysMethodChange);

    methodNRZRadioButton = UIRadioButton("NRZ", true);
    physMethodsRadioGroup.addRadioButton(methodNRZRadioButton, sf::Vector2f(0.0f, 0.0f));

    methodRZRadioButton = UIRadioButton("RZ", false);
    physMethodsRadioGroup.addRadioButton(methodRZRadioButton, sf::Vector2f(0.0f, 12.0f));

    methodAMIRadioButton = UIRadioButton("AMI", false);
    physMethodsRadioGroup.addRadioButton(methodAMIRadioButton, sf::Vector2f(0.0f, 24.0f));

    methodManchesterRadioButton = UIRadioButton("Manchester", false);
    physMethodsRadioGroup.addRadioButton(methodManchesterRadioButton, sf::Vector2f(0.0f, 36.0f));

    senderControlPanel.addUIElement(physMethodsRadioGroup, sf::Vector2f(12.0f, 180.0f));

    logicMethodsRadioGroup = UIRadioGroup(proceedLogicMethodChange);

    physicalRadioButton = UIRadioButton("Physical", true);
    logicMethodsRadioGroup.addRadioButton(physicalRadioButton, sf::Vector2f(0.0f, 0.0f));

    super45radioButton = UIRadioButton("Super 4/5", false);
    logicMethodsRadioGroup.addRadioButton(super45radioButton, sf::Vector2f(0.0f, 12.0f));

    scramblingRadioButton = UIRadioButton("Scrambling", false);
    logicMethodsRadioGroup.addRadioButton(scramblingRadioButton, sf::Vector2f(0.0f, 24.0f));

    senderControlPanel.addUIElement(logicMethodsRadioGroup, sf::Vector2f(12.0f, 244.0f));
    senderControlPanel.setPositionWithChildren(sf::Vector2f(635.0f, mainControlPanel.getGlobalBounds().top + mainControlPanel.getHeight() + 11.0f));

    /* Signal panel */
    signalPanel = UIPanel(619, 139, sf::Color(0x60, 0x60, 0x60, 0x60));

    signalPanelText = UITextPanel("Signal", signalPanel.getWidth() - 4, 10);
    signalPanel.addUIElement(signalPanelText, sf::Vector2f(2.0f, 2.0f));

    signalDiagram = UIDiscreteDiagram(560, 111);
    signalPanel.addUIElement(signalDiagram, sf::Vector2f(34.0f, 20.0f));

    signalPanel.setPositionWithChildren(sf::Vector2f(5.0f, mainControlPanel.getPosition().y + mainControlPanel.getHeight() + 11.0f));

    /* Fourier series panel */
    fourierSeriesPanel = UIPanel(signalPanel.getWidth(), 139, sf::Color(0x60, 0x60, 0x60, 0x60));

    fourierSeriesPanelText = UITextPanel("Fourier Series", fourierSeriesPanel.getWidth() - 4, 10);
    fourierSeriesPanel.addUIElement(fourierSeriesPanelText, sf::Vector2f(2.0f, 2.0f));

    fourierSeriesDiagram = UIFourierDiagram(560, 111);
    fourierSeriesDiagram.setMinY(-2.5f);
    fourierSeriesDiagram.setMaxY(2.5f);
    fourierSeriesPanel.addUIElement(fourierSeriesDiagram, sf::Vector2f(34.0f, 20.0f));

    fourierSeriesPanel.setPositionWithChildren(sf::Vector2f(5.0f, signalPanel.getGlobalBounds().top + signalPanel.getHeight() + 11.0f));

    /* Received Signal panel */
    receivedSignalPanel = UIPanel(signalPanel.getWidth(), 99, sf::Color(0x60, 0x60, 0x60, 0x60));

    receivedSignalPanelText = UITextPanel("Received Signal", receivedSignalPanel.getWidth() - 4, 10);
    receivedSignalPanel.addUIElement(receivedSignalPanelText, sf::Vector2f(2.0f, 2.0f));

    receivedSignalDiagram = UIDiscreteDiagram(560, 75);
    receivedSignalDiagram.setShowDiagram(false);
    receivedSignalPanel.addUIElement(receivedSignalDiagram, sf::Vector2f(34.0f, 20.0f));

    receivedSignalPanel.setPositionWithChildren(sf::Vector2f(5.0f, fourierSeriesPanel.getGlobalBounds().top + fourierSeriesPanel.getHeight() + 11.0f));

    /* State panel */
    statePanel = UIPanel(signalPanel.getWidth(), 114);

    statePanelText = UITextPanel("State", statePanel.getWidth() - 4, 10);
    statePanel.addUIElement(statePanelText, sf::Vector2f(2.0f, 2.0f));

    msgLabel = UITextLabel("msg : ''");
    statePanel.addUIElement(msgLabel, sf::Vector2f(7.0f, 24.0f));

    hexMsgLabel = UITextLabel("00000000 00000000");
    statePanel.addUIElement(hexMsgLabel, sf::Vector2f(7.0f, 31.0f));

    lengthLabel = UITextLabel("length: 1");
    statePanel.addUIElement(lengthLabel, sf::Vector2f(8.0f, 39.0f));

    bitrateLabel = UITextLabel("bitrate : 10 Mb/s");
    statePanel.addUIElement(bitrateLabel, sf::Vector2f(7.0f, 55.0f));

    recvMsgLabel = UITextLabel("recv msg : ''");
    statePanel.addUIElement(recvMsgLabel, sf::Vector2f(202.0f, 24.0f));

    recvHexMsgLabel = UITextLabel("00000000 00000000");
    statePanel.addUIElement(recvHexMsgLabel, sf::Vector2f(202.0f, 31.0f));

    bitsCountLabel = UITextLabel("bits count  : 0");
    statePanel.addUIElement(bitsCountLabel, sf::Vector2f(202.0f, 47.0f));

    errorCountLabel = UITextLabel("error count : 0");
    statePanel.addUIElement(errorCountLabel, sf::Vector2f(202.0f, 56.0f));

    errorLabel = UITextLabel("error %     : 0.00 %");
    statePanel.addUIElement(errorLabel, sf::Vector2f(202.0f, 64.0f));

    showTextLabel = UITextLabel("Show:");
    statePanel.addUIElement(showTextLabel, sf::Vector2f(402.0f, 24.0f));

    showFourierSeriesCheckbox = UICheckbox(true, "Fourier series");
    showFourierSeriesCheckbox.setTextOffset(8.0f);
    statePanel.addUIElement(showFourierSeriesCheckbox, sf::Vector2f(402.0f, 37.0f));

    showNoiseCheckbox = UICheckbox(true, "Noise");
    showNoiseCheckbox.setTextOffset(8.0f);
    statePanel.addUIElement(showNoiseCheckbox, sf::Vector2f(402.0f, 49.0f));

    showSamplersCheckbox = UICheckbox(true, "Samplers");
    showSamplersCheckbox.setTextOffset(8.0f);
    statePanel.addUIElement(showSamplersCheckbox, sf::Vector2f(402.0f, 61.0f));

    showVoltageCheckbox = UICheckbox(true, "Voltage");
    showVoltageCheckbox.setTextOffset(8.0f);
    statePanel.addUIElement(showVoltageCheckbox, sf::Vector2f(402.0f, 73.0f));

    resetStatsButton = UIButton("Reset stats", proceedResetStatsButtonClick);
    statePanel.addUIElement(resetStatsButton, sf::Vector2f(199.0f, 80.0f));

    statePanel.setPositionWithChildren(sf::Vector2f(5.0f, receivedSignalPanel.getGlobalBounds().top + receivedSignalPanel.getHeight() + 11.0f));

    /* Receiver panel */
    receiverPanel = UIPanel(senderControlPanel.getWidth(), 224);

    receiverPanelText = UITextPanel("Receiver", receiverPanel.getWidth() - 4, 10);
    receiverPanel.addUIElement(receiverPanelText, sf::Vector2f(2.0f, 2.0f));

    noiseSpin = UISpin<float>(0.00f, 2.00f, 0.01f, 0.1f, "Noise:");
    noiseSpin.setMaxSymbolsCount(4);
    noiseSpin.setTextOffset(20.0f);
    receiverPanel.addUIElement(noiseSpin, sf::Vector2f(91.0f, 24.0f));

    desyncSpin = UISpin<float>(0.00f, 1.00f, 0.01f, 0.1f, "Desync:", proceedDesyncSpinChange);
    desyncSpin.setMaxSymbolsCount(4);
    desyncSpin.setTextOffset(12.0f);
    receiverPanel.addUIElement(desyncSpin, sf::Vector2f(91.0f, 44.0f));

    voltageSpin = UISpin<float>(0.00f, 1.00f, 0.01f, 0.1f, "Voltage:");
    voltageSpin.setMaxSymbolsCount(4);
    voltageSpin.setTextOffset(4.0f);
    receiverPanel.addUIElement(voltageSpin, sf::Vector2f(91.0f, 64.0f));

    receiverPanel.setPositionWithChildren(sf::Vector2f(senderControlPanel.getGlobalBounds().left,
                                           senderControlPanel.getGlobalBounds().top + senderControlPanel.getHeight() + 11.0f));

    /* Footer panel */
    footerPanel = UIPanel(INIT_WINDOW_WIDTH - 10, 19);
    footerPanel.setPositionWithChildren(sf::Vector2f(5.0f, INIT_WINDOW_HEIGHT - 7.0f - footerPanel.getHeight()));
}

void init_view(void) {
    auto timepoint = std::chrono::high_resolution_clock::now();

    sf::RenderWindow window(sf::VideoMode(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT), PROGRAM_NAME, sf::Style::Titlebar);

    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile(getResourcePath("data/circuit.tga"));
    backgroundTexture.setRepeated(true);

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setTextureRect(sf::IntRect(0, 0, INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT));

    init_ui_elements();

    for (std::size_t i = 0; i < 11; ++i) {
        emptySignalDiagram.diagram_points[i] = { static_cast<float>(i), 0.0f };
        emptyFourierDiagram.diagram_points[i] = { static_cast<float>(i), -1.0f };
    }

    emptySignalDiagram.diagram_points_count = 9;
    emptyFourierDiagram.diagram_points_count = 9;
    fourierDiagram = emptyFourierDiagram;

    signalDiagram.setDiagram(emptySignalDiagram);
    fourierSeriesDiagram.setDiagram(emptyFourierDiagram);

    for (float i = 0; i < 8; ++i)
        fourierSeriesDiagram.addSamplerX(i + 0.5f);

    window.setFramerateLimit(FRAMERATE_LIMIT);

    transmitMessage();

    while (window.isOpen() && !doExit) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseMoved:
                    exitButton.onMouseMove(window);
                    aboutButton.onMouseMove(window);
                    takeScreenshotButton.onMouseMove(window);
                    transmitButton.onMouseMove(window);
                    resetStatsButton.onMouseMove(window);

                    physMethodsRadioGroup.onMouseMove(window);
                    logicMethodsRadioGroup.onMouseMove(window);

                    showFourierSeriesCheckbox.onMouseMove(window);
                    showNoiseCheckbox.onMouseMove(window);
                    showSamplersCheckbox.onMouseMove(window);
                    showVoltageCheckbox.onMouseMove(window);

                    highestHarmonicSpin.onMouseMove(window);
                    lowestHarmonicSpin.onMouseMove(window);

                    noiseSpin.onMouseMove(window);
                    desyncSpin.onMouseMove(window);
                    voltageSpin.onMouseMove(window);

                    senderControlPanelText.onMouseMove(window);
                    signalPanelText.onMouseMove(window);
                    fourierSeriesPanelText.onMouseMove(window);
                    receivedSignalPanelText.onMouseMove(window);
                    statePanelText.onMouseMove(window);
                    receiverPanelText.onMouseMove(window);

                    videoModeDropdown.onMouseMove(window);
                    break;
                case sf::Event::MouseButtonPressed:
                    if (aboutBoxActive) {
                        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                        aboutBoxActive = !(aboutBoxPanel.getGlobalBounds().contains(mousePosition) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left));
                    }
                    else {
                        exitButton.onMouseClick(window);
                        aboutButton.onMouseClick(window);
                        takeScreenshotButton.onMouseClick(window);
                        textField.onMouseClick(window);
                        transmitButton.onMouseClick(window);
                        resetStatsButton.onMouseClick(window);

                        physMethodsRadioGroup.onMouseClick(window);
                        logicMethodsRadioGroup.onMouseClick(window);

                        showFourierSeriesCheckbox.onMouseClick(window);
                        showNoiseCheckbox.onMouseClick(window);
                        showSamplersCheckbox.onMouseClick(window);
                        showVoltageCheckbox.onMouseClick(window);

                        highestHarmonicSpin.onMouseClick(window);
                        lowestHarmonicSpin.onMouseClick(window);

                        noiseSpin.onMouseClick(window);
                        desyncSpin.onMouseClick(window);
                        voltageSpin.onMouseClick(window);

                        senderControlPanelText.onMouseClick(window);
                        signalPanelText.onMouseClick(window);
                        fourierSeriesPanelText.onMouseClick(window);
                        receivedSignalPanelText.onMouseClick(window);
                        statePanelText.onMouseClick(window);
                        receiverPanelText.onMouseClick(window);

                        videoModeDropdown.onMouseClick(window);
                    }
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::F12 || (is_special_syskey_pressed() && is_shift_key_pressed() && sf::Keyboard::isKeyPressed(sf::Keyboard::S)))
                        takeScreenshot(window);
                    else if (event.key.code == sf::Keyboard::Backspace) {
                        if (is_special_syskey_pressed())
                            textField.clearText();
                        else
                            textField.removeSymbol();
                    }

                    videoModeDropdown.onKeyPressed(window);

                    highestHarmonicSpin.onKeyPressed(window);
                    lowestHarmonicSpin.onKeyPressed(window);

                    noiseSpin.onKeyPressed(window);
                    desyncSpin.onKeyPressed(window);
                    voltageSpin.onKeyPressed(window);
                    break;
                case sf::Event::TextEntered:
                    if (event.text.unicode < 128) {
                        char symbol = static_cast<char>(event.text.unicode);

                        if (uiManager.isSymbolPrintable(symbol))
                            textField.addSymbol(symbol);
                    }
                    break;
                default:
                    break;
            }
        }

        textField.update(window);

        auto t2 = std::chrono::high_resolution_clock::now();
        FourierEngine::setTime(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - timepoint).count() / 1000.0f);
        FourierEngine::setNoiseAmplitude(noiseSpin.getValue());
        noiseDiagram = genNoise();
        fourierNoiseDiagram = addNoise();

        fourier_diagram diagrams[] = {
            emptyFourierDiagram,
            noiseDiagram,
            fourierDiagram,
            fourierNoiseDiagram
        };

        int diagNumber = (showFourierSeriesCheckbox.isSelected() ? 1 : 0) << 1 | (showNoiseCheckbox.isSelected() ? 1 : 0);

        fourierSeriesDiagram.setShowDiagram(diagNumber != 0);
        fourierSeriesDiagram.setDiagram(diagrams[diagNumber]);

        receiveMessage();

        fourierSeriesDiagram.setDrawSamplers(showSamplersCheckbox.isSelected());

        window.clear();
        window.draw(backgroundSprite);
        window.draw(senderControlPanel);
        window.draw(physMethodsRadioGroup);
        window.draw(logicMethodsRadioGroup);
        window.draw(signalPanel);
        window.draw(fourierSeriesPanel);

        if (showVoltageCheckbox.isSelected())
            drawVoltageRectangle(window);

        window.draw(receivedSignalPanel);
        window.draw(statePanel);
        window.draw(receiverPanel);
        window.draw(footerPanel);
        window.draw(mainControlPanel);

        if (aboutBoxActive)
            drawAboutBox(window);

        window.display();
    }
}
