#include "FontManager.h"
// FontManager.cpp
void FontManager::initialize() {
    // Load Cooper Light for log text
    int fontLightId = QFontDatabase::addApplicationFont(":clientapp/fonts/cooperli.ttf");
    // Load Cooper Medium for buttons
    int fontMediumId = QFontDatabase::addApplicationFont(":clientapp/fonts/coopermi.ttf");
    // Load Cooper Black for titles
    int fontBlackId = QFontDatabase::addApplicationFont(":clientapp/fonts/coopbli.ttf");

    QString lightFamily, mediumFamily, blackFamily;
    bool hasLight = false, hasMedium = false, hasBlack = false;

    if (fontLightId != -1) {
        lightFamily = QFontDatabase::applicationFontFamilies(fontLightId).at(0);
        hasLight = true;
    }
    if (fontMediumId != -1) {
        mediumFamily = QFontDatabase::applicationFontFamilies(fontMediumId).at(0);
        hasMedium = true;
    }
    if (fontBlackId != -1) {
        blackFamily = QFontDatabase::applicationFontFamilies(fontBlackId).at(0);
        hasBlack = true;
    }

    // Set fonts with fallback to San Francisco
    buttonFont = hasMedium ? QFont(mediumFamily, 12) : QFont("San Francisco", 12);
    labelFont = hasBlack ? QFont(blackFamily, 14) : QFont("San Francisco", 14);
    logFont = hasLight ? QFont(lightFamily, 11) : QFont("San Francisco", 11);
}