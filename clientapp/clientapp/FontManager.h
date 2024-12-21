#include <QFont.h>
#include <QFontDatabase>
#include <QString>
#include <QList>
class FontManager {
public:
    static FontManager& getInstance() {
        static FontManager instance;
        return instance;
    }

    void initialize();
    const QFont& getButtonFont() const { return buttonFont; }
    const QFont& getLabelFont() const { return labelFont; }
    const QFont& getLogFont() const { return logFont; }

private:
    FontManager() {} // Private constructor

    QFont buttonFont;  // Cooper Medium
    QFont labelFont;   // Cooper Black
    QFont logFont;     // Cooper Light
};