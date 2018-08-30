#ifndef KEYBOARD_KEYTABLE_H
#define KEYBOARD_KEYTABLE_H


static const char keyToASCII_default[_USB_KEY_LAST] =
{     0, // Invalid key
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '`', '-', '=','\\',
   '\b', ' ','\t',   0, '[', ']',   0,   0,   0,   0,
      0,   0,   0,   0,   0,'\n',   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0, '/', '*', '-', '+','\n', '.', '0',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', ';',
   '\'', ',', '.', '/',   0
};

static const char keyToASCII_shift[_USB_KEY_LAST] =
{     0, // Invalid key
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', ')', '!', '@', '#',
    '$', '%', '^', '&', '*', '(', '~', '_', '+', '|',
      0,   0,   0,   0, '{', '}',   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0, ':',
    '"', '<', '>', '?',   0
};

static const char keyToASCII_altGr[_USB_KEY_LAST] =
{     0, // Invalid key
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0, '�', '�',   0, '�', '�',   0, '�',   0,
      0,   0,   0,   0, '�', '�',   0, '�', '�', '�',
      0, '�', '�', '�',   0,   0,   0, '�', 'x', '�',
      0,   0,   0,   0, '�', '�',   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0, '�',
      0, '�',   0, '�',   0
};

static const char keyToASCII_shiftAltGr[_USB_KEY_LAST] =
{     0, // Invalid key
      0,   0, '�',   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0, '�',   0, '�', '�',   0, '�',   0,
      0,   0,   0,   0, '�', '�',   0,   0,   0,   0,
    '�',   0,   0,   0,   0,   0,   0,   0, '�',   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0, '�',
      0, '�',   0,   0,   0
};

#endif
