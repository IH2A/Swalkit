#ifndef _BASICDISPLAY_H_
#define _BASICDISPLAY_H_

#include <cstdint>
#include <WString.h>

class BasicDisplay {
public:
    void Init();

    void SetBackgroundColor(uint32_t color, bool update = true);
    void SetBorderColor(uint32_t color, bool update = true);
    void SetFrontColors(uint32_t left_color, uint32_t right_color, bool update = true);
    void SetCenterColor(uint32_t color, bool update = true);
    void SetMessage(const String &text, bool update = true);
    void SetQRCode(const String &text);
    void SetError(const String &text);

    enum class Flags : uint8_t {
        None = 0b00000000,
        Background = 1 << 0,
        Border = 1 << 1,
        Center = 1 << 2,
        Front = 1 << 3,
        Message = 1 << 4,
        QRCode = 1 << 5,
        Error = 1 << 6,
        All = 0b01111111
    };

protected:
    int32_t width, height;
    int32_t borderSize;
    int32_t centerRadius;
    int32_t frontRadius;
    
    uint32_t messageColor;
    uint32_t errorColor;
    uint32_t errorTextColor;

private:
    void Update();
    void Set(Flags flag);
    void Clear(Flags flag);
    bool IsSet(Flags flag);

    void DrawBackground();
    void DrawBorder();
    void DrawCenter();
    void DrawFront();
    void DrawMessage();
    void DrawQRCode();
    void DrawError();

    void DrawText(const char *text);

    uint32_t backgroundColor;
    uint32_t borderColor;
    uint32_t centerColor;
    uint32_t frontLeftColor;
    uint32_t frontRightColor;

    String messageText;
    String qrCodeText;
    String errorText;
    uint8_t flags;
};

constexpr BasicDisplay::Flags operator |(const BasicDisplay::Flags left, const BasicDisplay::Flags right) {
    return static_cast<BasicDisplay::Flags>(static_cast<uint8_t>(left) | static_cast<uint8_t>(right));
} 
constexpr BasicDisplay::Flags operator &(const BasicDisplay::Flags left, const BasicDisplay::Flags right) {
    return static_cast<BasicDisplay::Flags>(static_cast<uint8_t>(left) & static_cast<uint8_t>(right));
} 
constexpr BasicDisplay::Flags operator ~(const BasicDisplay::Flags flag) {
    return static_cast<BasicDisplay::Flags>(~static_cast<uint8_t>(flag));
} 

#endif