M5Stack SimpleBeep library
===
This library can simply beep for M5Stack.
M5Stack用 シンプルビープ音 生成ライブラリ。

## Overview
Just call the function ("Beep") and you will hear a beep.
Beep関数を呼び出すだけでM5Stackからビープ音が鳴ります。
Detail Information ↓
https://harattamouse.hatenablog.com/entry/2020/08/12/m5stack-beep

## How it Works
The timer interrupt of esp32 is used. The DAC port is toggling output.
ESP32のタイマー割り込みにて、DACポートをトグル出力することで、ビープ音を発生させています。

![explan](images/output_image.jpg "output_image")

## Tune items
volume (0-127) : Adjust beep volume.
freqency[Hz] (0-65535) : Adjust the pitch of the beep sound.
duration[ms] (0-65535) : Adjust beep time.

## How to use
初期化関数("init")を実行した後は、Beep関数を呼び出すと鳴らせます。

The following code will beep once every second.
以下のソースコードでは、一秒おきにビープ音が鳴ります。

```
#include <M5Stack.h>
#include "SimpleBeep.h"

void setup(void){

    M5.begin();
    sb.init();          // initialze SimpleBeep.
}

void loop(void){

    sb.Beep(5,A4,100);  // beep (volume 5, pitch A, duration 100ms)
    delay(1000);        // Wait for one second.
}
```

## Licence
[MIT](https://github.com/haratta27/M5Stack_SimpleBeep/LICENSE)
## Author
[haratta27](https://twitter.com/harattaMouse)
