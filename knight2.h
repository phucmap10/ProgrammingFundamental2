#ifndef __KNIGHT2_H__
#define __KNIGHT2_H__

#include "main.h"
// #define DEBUG

enum ItemType {ANTIDOTE = 0, PHOENIX};

class BaseItem;
class Phoenix;
class Antidote;
class BaseKnight;
class Events;

class BaseItem {
protected:
    ItemType itemType;
public:
    virtual ItemType getType() { return itemType; }
    virtual string toString() = 0;
    virtual bool canUse(BaseKnight* knight) = 0;
    virtual void use(BaseKnight* knight) = 0;
    //student method
    virtual  ~BaseItem() {};
};

class Antidote : public BaseItem
{
public:
    Antidote() {
        this->itemType = ANTIDOTE;
    }

    ~Antidote() {}

    string toString() { return "Antidote"; }

    bool canUse(BaseKnight* knight)
    {
        return true;
    }

    void use(BaseKnight* knight)
    {
        return;
    }
};

class Phoenix : public BaseItem
{
protected:
    int typeOfPhoenix;
public:
    Phoenix(int num = 1)
    {
        this->itemType = PHOENIX;
        typeOfPhoenix = num;
    }
    ~Phoenix() {}

    string toString() {
        string arr[4]{ "I", "II", "III", "IV" };
        return "Phoenix" + arr[typeOfPhoenix - 1];
    }

    bool canUse(BaseKnight* knight);

    void use(BaseKnight* knight);

};



class BaseBag {
    struct BagNode {
        BaseItem* item;
        BagNode* next;
        BagNode(BaseItem* inp = nullptr, BagNode* nextptr = nullptr) : item{ inp }, next{ nextptr } {}
        ~BagNode()
        {
            if (item) delete item;
        }
    };
    BagNode* head;
    int count;
    int maxSlot;
public:
    BaseBag(int phoenixdown, int antidote, int max = -1) : maxSlot{ max }, count{ 0 }, head{ nullptr } {
        for (int i{ 0 }; i < phoenixdown; ++i)
        {
            //insert phoenix1 in bag
            BaseItem* tmp = new Phoenix();
            this->insertFirst(tmp);
        }

        for (int i{ 0 }; i < antidote; ++i)
        {
            BaseItem* tmp = new Antidote();
            this->insertFirst(tmp);
        }
    }
    virtual bool insertFirst(BaseItem * item);
    virtual BaseItem * get(ItemType itemType);
    virtual string toString() const;

    ///student's method
    ~BaseBag()
    {
        while (head)
        {
            BagNode* tmp{ head };
            head = head->next;
            delete tmp;
        }
        head = nullptr;
        count = 0;
    }
protected:
    virtual BaseItem* getAndCheck(ItemType itemType, BaseKnight* k9);
public:
    virtual bool useItem(ItemType itemType, BaseKnight* k9);
    virtual void dropLatestItem();
};

class BaseOpponent
{
protected:
    int level;
    int type;
    int gil;
    int baseDamage;
public:
    BaseOpponent(int lv, int num, int gil, int bd) : level{ lv }, type{ num }, gil{gil}, baseDamage{bd}{}
    int getType() { return type; }
    int getLevel() { return level; }
    int getGil() { return gil; }
    int getDame() { return baseDamage; }
};

enum KnightType { PALADIN = 0, LANCELOT, DRAGON, NORMAL };
class BaseKnight {
protected:
    int id;
    int hp;
    int maxhp;
    int level;
    int gil;
    int antidote;
    BaseBag * bag;
    KnightType knightType;
    //student attri
public:
    static BaseKnight * create(int id, int maxhp, int level, int gil, int antidote, int phoenixdownI);
    virtual void fight(BaseOpponent * opponent) = 0;

    string toString() const;

    double baseDameMul;
    //student method
    virtual ~BaseKnight()
    {
        if (bag) delete bag;
        bag = nullptr;
    }
    int getHp() const { return hp; }
    void setHp(int newHp) { this->hp = newHp; }
    int getMaxHp() const{ return maxhp; }
    int getLv() const { return level; }
    void setLv(int lv) { level = lv; }
    KnightType getType() const { return knightType; }
    int getGil() const { return gil; }
    void setGil(int num) { gil = num; }
    BaseBag* getBag() { return bag; }
    bool knightDead() { return hp <= 0; }

    virtual bool handleWhenHpDown();
protected:
    virtual void winningMinor(BaseOpponent* opponent) {
        this->gil += opponent->getGil();
    }
    virtual void losingMinor(BaseOpponent* opponent)
    {
        int damage = opponent->getDame() * (opponent->getLevel() - this->level);
        this->hp -= damage;
        handleWhenHpDown();
    }
    virtual void winningTornbery()
    {
        this->level += 1;
        if (this->level > 10) this->level = 10;
    }
    virtual void losingTornbery()
    {
        if (antidote > 0)
        {
            --antidote;
            bag->useItem(ANTIDOTE, this);
        }
        else
        {
            for (int i{ 0 }; i < 3; ++i) bag->dropLatestItem();
            this->hp -= 10;
            handleWhenHpDown();
        }
    }
    virtual void winningQueen()
    {
        this->gil *= 2;
    }
    virtual void losingQueen()
    {
        this->gil /= 2;
    }
};

class PaladinKnight : public BaseKnight {
public:
    PaladinKnight(int id, int maxhp, int level, int gil, int antidote, int phoenixI)
    {
        this->id = id;	
        this->maxhp = maxhp;
        this->hp = maxhp;
        this->level = level;
        this->gil = gil;
        this->antidote = antidote;
        bag = new BaseBag(phoenixI, antidote);
        this->knightType = PALADIN;
        baseDameMul = 0.06;
    }
    ~PaladinKnight()
    {
        if (bag) delete bag;
        bag = nullptr;
    }
    void fight(BaseOpponent* opponent);
};

class LancelotKnight : public BaseKnight {
public:
    LancelotKnight(int id, int maxhp, int level, int gil, int antidote, int phoenixI)
    {
        this->id = id;
        this->maxhp = this->hp = maxhp;
        this->hp = maxhp;
        this->level = level;
        this->gil = gil;
        this->antidote = antidote;
        bag = new BaseBag(phoenixI, antidote,16);
        this->knightType = LANCELOT;
        baseDameMul = 0.06;
    }
    ~LancelotKnight()
    {
        if (bag) delete bag;
        bag = nullptr;
    }
    void fight(BaseOpponent* opponent);
};

class DragonKnight : public BaseKnight {
public:
    DragonKnight(int id, int maxhp, int level, int gil, int antidote, int phoenixI)
    {
        this->id = id;
        this->maxhp = maxhp;
        this->hp = maxhp;
        this->level = level;
        this->gil = gil;
        this->antidote = antidote;
        bag = new BaseBag(phoenixI, antidote,14);
        this->knightType = DRAGON;
        baseDameMul = 0.075;
    }
    ~DragonKnight()
    {
        if (bag) delete bag;
        bag = nullptr;
    }
    void fight(BaseOpponent* opponent);
};

class NormalKnight : public BaseKnight {
public:
    NormalKnight(int id, int maxhp, int level, int gil, int antidote, int phoenixI)
    {
        this->id = id;
        this->maxhp = maxhp;
        this->hp = maxhp;
        this->level = level;
        this->gil = gil;
        this->antidote = antidote;
        bag = new BaseBag(phoenixI, antidote,19);
        this->knightType = NORMAL;
        baseDameMul = 0;
    }
    ~NormalKnight()
    {
        if (bag) delete bag;
        bag = nullptr;
    }
    void fight(BaseOpponent* opponent);
};


struct Config {
    bool hasShield;
    bool hasSpear;
    bool hasHair;
    bool hasSword;
    bool hadDefeatedOmega;
    bool hadDefeatedHades;
    Config() : 
        hasShield{ false }, hasSpear{ false }, hasHair{ false }, hasSword{ false }, hadDefeatedOmega{ false }, hadDefeatedHades{ false } {}
};

class ArmyKnights {
    BaseKnight** army;
    int initNumOfKnights;
    int numOfKnight;
    Config neededCondition;


public:
    ArmyKnights (const string & file_armyknights);
    ~ArmyKnights();
    bool adventure (Events * events);
    int count() const;
    BaseKnight * lastKnight() const;

    bool hasPaladinShield() const { return neededCondition.hasShield; }
    bool hasLancelotSpear() const { return neededCondition.hasSpear; }
    bool hasGuinevereHair() const { return neededCondition.hasHair; }
    bool hasExcaliburSword() const { return neededCondition.hasSword; }

    void printInfo() const;
    void printResult(bool win) const;

    //student's method
private:
    void deleteKnightAt(int idx);

    void passGilToPreviousKnight(int, int);

    bool eventHanlde(int eventOrder, int eventNumber);

    bool fightMinorMonster(int eventOrder, int monsterType);

    bool fightTornbery(int eventOrder);

    bool fightQueenOfCards(int eventOrder);

    void meetNina();

    void meetDurianGarden();

    bool fightOmegaWeapon();

    bool fightHades();

    bool fightUltimecia();

    void takePhoenix(int type);

    void takeMinorItem(int eventNumber);

    void takeExcalibur();

    
};


class Events {
    int* event;
    int numOfEvent;
public:
    Events(const string& file_events);
    ~Events();
    int count() const;
    int get(int i) const;
};

class KnightAdventure {
private:
    ArmyKnights * armyKnights;
    Events * events;

public:
    KnightAdventure();
    ~KnightAdventure(); // TODO:

    void loadArmyKnights(const string &);
    void loadEvents(const string &);
    void run();
};

#endif // __KNIGHT2_H__
