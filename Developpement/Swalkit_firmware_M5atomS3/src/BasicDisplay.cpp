#include "BasicDisplay.h"
#include <M5AtomS3.h>

/* 
 * Layout : 
 * Background : level  0
 * Border : level 1. Around Center, Message & QRCode.
 * Center : level 1.
 * Message : level 2.
 * QRCode : level 3.
 * Error : level 4. Covers everything.
 */

void BasicDisplay::Init() {
    width = M5.Lcd.width();
    height = M5.Lcd.height();
    borderSize = 8;
    centerRadius = 40;

    backgroundColor = TFT_WHITE;
    borderColor = TFT_TRANSPARENT;
    centerColor = TFT_TRANSPARENT;
    messageColor = TFT_BLACK;
    errorColor = TFT_RED;
    errorTextColor = TFT_BLACK;

    messageText = nullptr;
    qrCodeText = nullptr;
    errorText = nullptr;
    flags = 0;

    M5.Lcd.clear(backgroundColor);
}

void BasicDisplay::SetBackgroundColor(uint32_t color, bool update) {
    if (color != backgroundColor) {
        backgroundColor = color;
        Set(Flags::Background);
        if (update) {
            Update();
        }
    }
}

void BasicDisplay::SetBorderColor(uint32_t color, bool update) {
    if (color != borderColor) {
        borderColor = color;
        Set(Flags::Border);
        if (update) {
            Update();
        }
    }
}

void BasicDisplay::SetCenterColor(uint32_t color, bool update) {
    if (color != centerColor) {
        centerColor = color;
        Set(Flags::Center);
        if (update) {
            Update();
        }
    }
}

void BasicDisplay::SetMessage(const char *text, bool update) {
    if (text != messageText) {
        messageText = text; // no copy. Just consider we're using static strings.
        Set(Flags::Background);
        if (update) {
            Update();
        }
    }
}

void BasicDisplay::SetQRCode(const char *text) {
    if (text != qrCodeText) {
        qrCodeText = text;
        if (qrCodeText != nullptr) {
            Set(Flags::QRCode);
        } else {
            Set(Flags::Background);
        }
        Update();
    }
}

void BasicDisplay::SetError(const char *text) {
    if (text != errorText) {
        errorText = text; // no copy. Just consider we're using static strings.
        if (errorText != nullptr) {
            Set(Flags::Error);
        } else {
            Set(Flags::Background);
        }
        Update();
    }
}

void BasicDisplay::Update() {
    if (IsSet(Flags::Error)) {
        Clear(Flags::Error);
        DrawError();
    }
    if (errorText != nullptr) {
        // error message is top priority. Don't draw anything else.
        return;
    }

    if (IsSet(Flags::Background)) {
        Clear(Flags::Background);
        DrawBackground();
        Set(Flags::Border | Flags::Center | Flags::Message | Flags::QRCode);
    }

    if (IsSet(Flags::Border)) {
        Clear(Flags::Border);
        DrawBorder();
    }

    if (IsSet(Flags::QRCode)) {
        Clear(Flags::QRCode);
        DrawQRCode();
    }

    if (qrCodeText == nullptr) {
        if (IsSet(Flags::Center | Flags::Message)) {
            Clear(Flags::Center | Flags::Message);
            DrawCenter();
            DrawMessage();
        }
    }
}

void BasicDisplay::Set(BasicDisplay::Flags setFlags) {
    flags |= static_cast<uint8_t>(setFlags);
}
void BasicDisplay::Clear(BasicDisplay::Flags clearFlags) {
    flags &= ~static_cast<uint8_t>(clearFlags);
}
bool BasicDisplay::IsSet(BasicDisplay::Flags flag) {
    return 0 != (flags & static_cast<uint8_t>(flag));
}


void BasicDisplay::DrawBackground() {
    M5.Lcd.clear(backgroundColor);
}

void BasicDisplay::DrawBorder() {
    uint16_t color = (borderColor != TFT_TRANSPARENT) ? borderColor : backgroundColor;
    M5.Lcd.fillRect(0, 0, borderSize, height, color);
    M5.Lcd.fillRect(width - borderSize, 0, borderSize, height, color);
    M5.Lcd.fillRect(0, 0, width, borderSize, color);
    M5.Lcd.fillRect(0, height - borderSize, width, borderSize, color);
}

void BasicDisplay::DrawCenter() {
    M5.Lcd.fillCircle(width >> 1, height >> 1, centerRadius, (centerColor != TFT_TRANSPARENT) ? centerColor : backgroundColor);
}

void BasicDisplay::DrawMessage() {
    if (messageText != nullptr) {
        M5.Lcd.setTextColor(messageColor);
        M5.Lcd.setTextSize(2);
        DrawText(messageText);
    }
}

void BasicDisplay::DrawQRCode() {
    if (qrCodeText != nullptr) {
        M5.Lcd.qrcode(qrCodeText, borderSize, borderSize, width - (borderSize << 1));
    }
}

void BasicDisplay::DrawError() {
    if (errorText != nullptr) {
        M5.Lcd.clear(errorColor);
        M5.Lcd.setTextColor(errorTextColor);
        M5.Lcd.setTextSize(2);
        DrawText(errorText);
    }
}

/* Draws text on multiple lines, trying to break on spaces whenever possible */
void BasicDisplay::DrawText(const char *text) {
    int16_t availableWidth = width - (borderSize << 1);
    int16_t y = borderSize + 2;
    char buffer[32];
    int16_t textWidth;
    size_t i;
    size_t suitableTextLength;
    const char* suitableTextBreak;
    M5.Lcd.setTextWrap(false);

    while (*text) {
        textWidth = M5.Lcd.textWidth(text);
        if (availableWidth >= textWidth) {  // draw all (remaining) text if it fits
            M5.Lcd.setCursor(availableWidth - textWidth >> 1, y);
            M5.Lcd.print(text);
            break;
        } else {
            suitableTextLength = 0;
            suitableTextBreak = nullptr;
            i = 0;
            do {
                // fill in buffer with any beginning space...
                for (; *text && *text == ' ' && i < sizeof(buffer) - 1; ++i) {
                    buffer[i] = *text++;
                }
                // ... and all chars until next space
                for (; *text && *text != ' ' && i < sizeof(buffer) - 1; ++i) {
                    buffer[i] = *text++;
                }
                buffer[i] = '\0';
                textWidth = M5.Lcd.textWidth(buffer);
                if (textWidth <= availableWidth) {  // if the computed text width fits, remember it
                    suitableTextLength = i;
                    suitableTextBreak = text;
                }
            } while (*text && textWidth <= availableWidth && i < sizeof(buffer) - 1);

            if (textWidth > availableWidth && suitableTextLength > 0) { // gone too far ? backtrack to suitable width if any
                buffer[suitableTextLength] = '\0';
                text = suitableTextBreak;
                textWidth = M5.Lcd.textWidth(buffer);
            }
            M5.Lcd.setCursor(borderSize + (availableWidth - textWidth >> 1), y);
            M5.Lcd.print(buffer);
            y += M5.Lcd.fontHeight();
        }
    }
}