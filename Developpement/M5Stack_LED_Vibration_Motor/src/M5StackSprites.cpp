#include "M5StackSprites.h"

void M5StackSprites::drawImage(char image) {
    switch (image)
    {
        case 'S':
            createImage(imageSmile, WHITE);
            break;

        case 'H':
            createImage(imageHeart, RED);
            break;
        
        case 'B':
            createImage(imageBluetooh, BLUE);
            break;

        default:
            break;
    }
}

void M5StackSprites::createImage(std::vector<bool> image, uint32_t color) {
    M5.Lcd.fillScreen(BLACK);
    for (int i = 0; i < image.size(); i++) {
        // Create white base
        if (image[i]) {
            int x = (i-i%16)/16*20;
            int y = i%16*15;
            M5.Lcd.fillRoundRect(x-2, y-2, 24, 19, 3, WHITE);
        }
    }
    for (int i = 0; i < image.size(); i++) {
        // Create the image with the desired color
        if (image[i]) {
            int x = (i-i%16)/16*20;
            int y = i%16*15;
            M5.Lcd.fillRoundRect(x, y, 20, 15, 3, color);
        }
    }
}