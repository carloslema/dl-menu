#ifndef _MENU_H
#define _MENU_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Button.h>
#include <EEPROM.h>
#include <math.h>

#include "DLNumber.h"
#include "DLTools.h"

#define M_TIMEOUT 15000        // go to default item in the menu after

// Menu classes:
class DLMenu;     // a container for handling MenuItems
class DLMenuItem; // an abstract parent class that other classes will 'polymorph into'
class DLIntMenuItem;
class DLFloatMenuItem;
class DLChoiceMenuItem;
class DLTextMenuItem;

class DLMenu
{
  public:
    DLMenu(LiquidCrystal *lcd,
           Button *bLeft, Button *bRight, Button *bUp, Button *bDown,
           DLMenuItem **items); // list of pointers to menu items
    bool check(void);           // see what happened and 'send a message' to current menu item

  private:
    bool quit(void);

    LiquidCrystal *lcd;
    Button *bUp, *bDown, *bLeft, *bRight;

    DLMenuItem **items;
    int index, len; // index of current item, total number of items

    unsigned long tTouched; // time the last button was pressed
};

class DLMenuItem
{
  public:
    // all MenuItems write to lcd, have a label, an EEPROM address and a default value
    DLMenuItem(LiquidCrystal *d, const char *l, int a) : lcd(d), label(l), address(a){};

    // items are initialized in the beginning and then only shown;
    virtual void show(bool endFirst) { return; }; // endFirst: start with the last section
    virtual void hide(void) { return; };          // cleanup and save

    // left/right button actions: returns false if it is out of scope of this item
    // (the next menu item should be displayed instead)
    virtual bool next(void);// { return false; };
    virtual bool previous(void);// { return false; };
    virtual void placeCursor(void); // is overridden in some menu items

    virtual void increase(void) { return; }; // up and down button actions
    virtual void decrease(void) { return; };

    // functions that can't be virtual because each type of MenuItem has a
    // different type:
    // <whatever> getValue(void){ return <whatever> };
    // void setValue(<whatever>){};
  protected:
    LiquidCrystal *lcd;
    const char *label;
    int address; // EEPROM address
    int s, sections; // number of modifiable sections in a menu item;
                     // depends on the type of the item; s is the current one
};

class DLIntMenuItem : public DLMenuItem
{
  public:
    DLIntMenuItem(LiquidCrystal *lcd, const char *label, int address, int digits);

    void show(bool endFirst);
    void hide(void);

    void increase(void);
    void decrease(void);

    void setValue(unsigned long value);
    unsigned long getValue(void) { return n->getUintValue(); };
    

  private:
    DLNumber *n; // loading, saving, digits...
    int digits;

    void add(int i);

    char *numerals;     // size will be assigned later (depends on Number->digits)
    void explode(void); // gets a float from n and puts it in numerals*exponent

    void printNumber(void);
};

class DLFloatMenuItem : public DLMenuItem
{
  public:
    DLFloatMenuItem(LiquidCrystal *lcd, const char *label, int address, int digits);

    void show(bool endFirst);
    void hide(void);

    void increase(void);
    void decrease(void);

    // warning! this will immediately save to EEPROM
    void setValue(float value);
    float getValue(void) { return n->getFloatValue(); };

  private:
    DLNumber *n; // loading, saving, digits...
    int digits;

    void add(int i);

    char *numerals; // size will be assigned later (depends on Number->digits)
    char exponent;
    void explode(void); // gets a float from n and puts it in numerals*exponent

    void printNumber(void);
    void placeCursor(void); // takes the current section and places the cursor
};

class DLChoiceMenuItem : public DLMenuItem
{
  public:
    DLChoiceMenuItem(LiquidCrystal *lcd, const char *label, int address, const char *const *choices, int nChoices);


    bool next(void){ return false; };
    bool previous(void){ return false; }

    void show(bool endFirst);
    void hide(void);

    void increase(void);
    void decrease(void);

    void setValue(byte value);
    byte getValue(void) { return selected; };

  private:
    byte selected; // selected choice
    int len;       // the length of the list
    const char *const *choices; // choices table, saved in PROGMEM

    void add(int n);
    void save(void);
};

class DLTextMenuItem : public DLMenuItem {
  public:
    DLTextMenuItem(LiquidCrystal *lcd, const char *label, int address, const char *allowedCharacters, int length);

    void show(bool endFirst);
    void hide(void);

    void increase(void);
    void decrease(void);


    void setValue(char *value);
    char *getValue(void){ return text; }; // returns a pointer to text

  private:
    int len; // the length of the list
    const char *allowedChars; // array of allowed characters, saved in PROGMEM
    int nAllowedChars; // number of characters in allowedChars;
    char *text;

    int findIndex(char c); // finds the index of c in allowedChars
    void normalize(void); // replace non-allowed characters
    void add(int n);
    void save(void);
};

#endif
