# hhkb_jp_keymap

This is a keymap setting for ["Alt Controller Board for HHKB JP"](https://geekhack.org/index.php?topic=71517.0), it functions as part of the ["qmk_firmware"](https://github.com/qmk/qmk_firmware).

## Built-in and compile method

Execute the following command in the qmk_firmware directory.
```
$ git submodule add https://github.com/rai-suta/hhkb_jp_keymap.git "keyboards/hhkb/keymaps/raisuta_jp"
$ make KEYBOARD=hhkb KEYMAP=raisuta_jp
```
