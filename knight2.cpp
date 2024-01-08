#include "knight2.h"

/* * * BEGIN implementation of class BaseItem * * */

bool Phoenix::canUse(BaseKnight* knight)
{
    int hp{ knight->getHp() };
    int maxHp{ knight->getMaxHp() };
    switch (typeOfPhoenix)
    {
    case 1:
        if (hp <= 0) return true;
        else return false;
    case 2:
        if (hp < (maxHp / 4)) return true;
        else return false;
    case 3:
        if (hp < (maxHp / 3)) return true;
        else return false;
    case 4:
        if (hp < (maxHp / 2)) return true;
        else return false;
    }
    return false;
}

void Phoenix::use(BaseKnight* knight)
{
    int hp{ knight->getHp() };
    int maxHp{ knight->getMaxHp() };
    switch (typeOfPhoenix)
    {
    case 1:
    case 2:
        knight->setHp(maxHp);
        return;
    case 3:
        if (hp <= 0) knight->setHp((maxHp / 3));
        else knight->setHp((maxHp / 4) + hp);
        return;
    case 4:
        if (hp <= 0) knight->setHp((maxHp / 2));
        else knight->setHp((maxHp / 5) + hp);
        return;
    }
}

/* * * END implementation of class BaseItem * * */





/* * * BEGIN implementation of class BaseBag * * */

bool BaseBag::insertFirst(BaseItem* item) {
    if (count == 0)
    {
        head = new BagNode(item);
        ++count;
        return true;
    }
    else if (maxSlot == -1 || count<maxSlot)
    {
        BagNode* tmp = new BagNode(item, head);
        head = tmp;
        ++count;
        return true;
    }
    else return false;
}

BaseItem* BaseBag::get(ItemType itemType) {
    BagNode* tmp{ head };
    while (tmp)
    {
        if (tmp->item->getType() == itemType)
        {
            BaseItem* tmpItem{ tmp->item };
            return tmpItem;
        }
        tmp = tmp->next;
    }
    return nullptr;
}

string BaseBag::toString() const {
    string res{ "Bag[count=" };
    res = res + to_string(count) + ';';
    BagNode* tmp{ head };
    while (tmp)
    {
        res = res + tmp->item->toString() + ',';
        tmp = tmp->next;
    }
    if (res[res.length() - 1] == ',') res[res.length() - 1] = ']';
    else res += ']';
    // res[res.length() - 1] = ']';
    return res;
}

///student's method

BaseItem* BaseBag::getAndCheck(ItemType itemType, BaseKnight* k9) {
    BagNode* tmp{ head };
    while (tmp)
    {
        if (tmp->item->getType() == itemType && tmp->item->canUse(k9))
        {
            BaseItem* tmpItem{ tmp->item };
            tmp->item = head->item;
            head->item = tmpItem;
            return tmpItem;
        }
        tmp = tmp->next;
    }
    return nullptr;
}

bool BaseBag::useItem(ItemType itemType, BaseKnight* k9) {
    BaseItem* itemToUse = getAndCheck(itemType, k9);
    if (itemToUse)
    {
        itemToUse->use(k9);
        BagNode* tmp = head;
        head = head->next;
        --count;
        delete tmp;
        return true;
    }
    return false;
}

void BaseBag::dropLatestItem()
{
    if (count>0)
    {
        BagNode* tmp = head;
        head = head->next;
        --count;
    }
}

/* * * END implementation of class BaseBag * * */


/* * * BEGIN implementation of class BaseOpponent * * */
///There is nothing here yet. Maybe u want to write some opponent by yourself

/* * * END implementation of class BaseOpponent * * */



/* * * BEGIN implementation of class BaseKnight * * */
string BaseKnight::toString() const {
    string typeString[4] = {"PALADIN", "LANCELOT", "DRAGON", "NORMAL"};
    // inefficient version, students can change these code
    //      but the format output must be the same
    string s("");
    s += "[Knight:id:" + to_string(id) 
        + ",hp:" + to_string(hp) 
        + ",maxhp:" + to_string(maxhp)
        + ",level:" + to_string(level)
        + ",gil:" + to_string(gil) + ','
        + bag->toString()
        + ",knight_type:" + typeString[knightType]
        + "]";
    return s;
}

bool isPrime(int n)
{
    if (n < 2) return false;
    for (int i{ 2 }; i < sqrt(n); ++i)
    {
        if (n%i==0) return false;
    }
    return true;
}

bool isPythago(int n)
{
    int a = n % 10;
    int b = (n / 10) % 10;
    int c = n / 100;
    if (a * b * c == 0) return false;
    else if (a * a + b * b == c * c) return true;
    else if (b * b + c * c == a * a) return true;
    else if (c * c + a * a == b * b) return true;
    return false;
}

BaseKnight* BaseKnight::create(int id, int maxhp, int level, int gil, int antidote, int phoenixdownI)
{
    BaseKnight* res{ nullptr };
    if (maxhp == 888)
    {
        res = new LancelotKnight(id, maxhp, level, gil, antidote, phoenixdownI);
    }
    else if (isPrime(maxhp))
    {
        res = new PaladinKnight(id, maxhp, level, gil, antidote, phoenixdownI);
    }
    else if (maxhp > 99 && maxhp <= 999 && isPythago(maxhp))
    {
        //whether we should pass this knight antidote for previous knight?
        res = new DragonKnight(id, maxhp, level, gil, 0, phoenixdownI);
    }
    else
    {
        res = new NormalKnight(id, maxhp, level, gil, antidote, phoenixdownI);
    }
    return res;
}

bool BaseKnight::handleWhenHpDown()
{
    if (bag->useItem(PHOENIX, this)) return true;
    else if (this->hp <= 0 && this->gil >= 100)
    {
        this->hp = this->maxhp / 2;
        this->gil -= 100;
        return true;
    }
    return false;
}

void PaladinKnight::fight(BaseOpponent* opponent)
{
    int monsterType = opponent->getType();
    if (monsterType <= 5)
    {
        winningMinor(opponent);
    }
    else if (this->level >= opponent->getLevel())
    {
        if (monsterType == 6)
        {
            winningTornbery();
        }
        else winningQueen();
    }
    else if (monsterType==6)
    {
        losingTornbery();
    }
}

void LancelotKnight::fight(BaseOpponent* opponent)
{
    int monsterType = opponent->getType();
    if (monsterType <= 5)
    {
        winningMinor(opponent);
    }
    else if (this->level >= opponent->getLevel())
    {
        if (monsterType == 6)
        {
            winningTornbery();
        }
        else winningQueen();
    }
    else if (monsterType == 6)
    {
        losingTornbery();
    }
    else losingQueen();
}

void DragonKnight::fight(BaseOpponent* opponent)
{
    int monsterType = opponent->getType();
    if (this->level >= opponent->getLevel())

    {
        if (monsterType <= 5) winningMinor(opponent);
        else if (monsterType == 6)
        {
            winningTornbery();
        }
        else winningQueen();
    }
    else if (monsterType <= 5)
    {
        losingMinor(opponent);
    }
    else if (monsterType == 7) losingQueen();
}

void NormalKnight::fight(BaseOpponent* opponent)
{
    int monsterType = opponent->getType();
    if (this->level >= opponent->getLevel())
    {
        if (monsterType <= 5) winningMinor(opponent);
        else if (monsterType == 6)
        {
            winningTornbery();
        }
        else winningQueen();
    }
    else if (monsterType <= 5)
    {
        losingMinor(opponent);
    }
    else if (monsterType == 6) losingTornbery();
    else losingQueen();
}

/* * * END implementation of class BaseKnight * * */







/* * * BEGIN implementation of class ArmyKnights * * */

ArmyKnights::ArmyKnights(const string & file_armyknights): neededCondition{}
{
    ifstream inf{ file_armyknights };
    army = nullptr;
    initNumOfKnights = 0;
    numOfKnight = 0;
    if (!inf)
    {
        // Print an error and exit
        std::cerr << "File_amryknights could not be opened for reading!\n";
        return ;
    }

    inf >> numOfKnight;
    initNumOfKnights = numOfKnight;
    army = new BaseKnight*[numOfKnight];
    for (int i{0}; i<numOfKnight; ++i)
    {
        int hp, level, phoenix, gil, antidote;
        inf >> hp >> level >> phoenix >> gil >>antidote;
        army[i] = BaseKnight::create(i+1,hp,level,gil,antidote,phoenix);
    }
    inf.close();
}

ArmyKnights::~ArmyKnights() 
{
    for (int i{ 0 }; i < initNumOfKnights; ++i)
    {
        if (army[i]) delete army[i];
    }
    delete[] army;
    army = nullptr;
    numOfKnight = 0;
}

bool ArmyKnights::adventure(Events* events) {
    int numOfEvents{ events->count() };
    bool result{ true };
    for (int i{ 0 }; i < numOfEvents; ++i)
    {
        int eventNumber{ events->get(i) };
        if (numOfKnight <= 0)
        {
            printInfo();
            continue;
        }
        bool res = eventHanlde(i, eventNumber);

        printInfo();
        if (!res) result = false;
    }
    return result;
}

int ArmyKnights::count() const {
    return numOfKnight;
}

BaseKnight* ArmyKnights::lastKnight() const {
    return army[numOfKnight - 1];
}

void ArmyKnights::printInfo() const {
    cout << "No. knights: " << this->count();
    if (this->count() > 0) {
        BaseKnight* lknight = lastKnight(); // last knight
        cout << ";" << lknight->toString();
    }
    cout << ";PaladinShield:" << this->hasPaladinShield()
        << ";LancelotSpear:" << this->hasLancelotSpear()
        << ";GuinevereHair:" << this->hasGuinevereHair()
        << ";ExcaliburSword:" << this->hasExcaliburSword()
        << endl
        << string(50, '-') << endl;
}

void ArmyKnights::printResult(bool win) const {
    cout << (win ? "WIN" : "LOSE");
}

//student's method

void ArmyKnights::deleteKnightAt(int idx)
{
    if (army && army[idx])
    {
        delete army[idx];
        army[idx] = nullptr;
        --numOfKnight;
    }
}

void ArmyKnights::passGilToPreviousKnight(int numOfGil, int startIdx)
{
    while (startIdx>=0 && numOfGil>0)
    {
        int curGil = army[startIdx]->getGil();
        curGil += numOfGil;
        numOfGil = 0;
        if (curGil > 999)
        {
            numOfGil = curGil - 999;
            curGil = 999;
        }
        army[startIdx]->setGil(curGil);
        --startIdx;
    }
}

bool ArmyKnights::fightMinorMonster(int eventOrder, int monsterType)
{
    int gilOfMonster[5]{ 100,150,450,750,800 };
    int bdOfMonster[5]{ 10,15,45,75,95 };
    int monsterLevel = (eventOrder + monsterType) % 10 + 1;
    BaseOpponent* monster = new BaseOpponent(monsterLevel, monsterType, gilOfMonster[monsterType - 1], bdOfMonster[monsterType - 1]);
    for (int i{ numOfKnight - 1 }; i >= 0; --i)
    {
        army[i]->fight(monster);
        if (army[i]->getHp() <= 0) deleteKnightAt(i);
        else
        {
            int gil = army[i]->getGil();
            if (gil > 999)
            {
                army[i]->setGil(999);
                passGilToPreviousKnight(gil - 999, i - 1);
            }
            break;
        }
        
    }
    delete monster;
    if (numOfKnight <= 0) return false;
    return true;
}

bool ArmyKnights::fightTornbery(int eventOrder)
{
    int monsterLevel = (eventOrder + 6) % 10 + 1;
    BaseOpponent* monster = new BaseOpponent(monsterLevel, 6, 0, 0);
    for (int i{ numOfKnight - 1 }; i >= 0; --i)
    {
        army[i]->fight(monster);
        if (army[i]->getHp() <= 0) deleteKnightAt(i);
        else break;
    }
    delete monster;
    if (numOfKnight <= 0) return false;
    return true;
}

bool ArmyKnights::fightQueenOfCards(int eventOrder)
{
    int monsterLevel = (eventOrder + 7) % 10 + 1;
    BaseOpponent* monster = new BaseOpponent(monsterLevel, 7, 0, 0);
    BaseKnight* lastK9 = this->lastKnight();
    lastK9->fight(monster);
    int gil = lastK9->getGil();
    if (gil > 999)
    {
        lastK9->setGil(999);
        passGilToPreviousKnight(gil - 999, numOfKnight - 2);
    }
    delete monster;
    return true;
}

void ArmyKnights::meetNina()
{
    for (int i{ 0 }; i < numOfKnight; ++i)
    {
        BaseKnight* tmp = this->army[i];
        //BaseKnight* tmp = this->lastKnight();
        int curGil = tmp->getGil();
        int curHp = tmp->getHp();
        int maxHp = tmp->getMaxHp();
        if (tmp->getType() == PALADIN && curHp < (maxHp / 3))
        {
            tmp->setHp(curHp + maxHp / 5);
        }
        else if (curGil >= 50 && curHp < (maxHp / 3))
        {
            tmp->setGil(curGil - 50);
            tmp->setHp(curHp + maxHp / 5);
        }
    }
}

void ArmyKnights::meetDurianGarden()
{
    for (int i{ 0 }; i < numOfKnight; ++i)
    {
        BaseKnight* tmp = this->army[i];
        tmp->setHp(tmp->getMaxHp());
    }
}

bool ArmyKnights::fightOmegaWeapon()
{
    if (neededCondition.hadDefeatedOmega) return true;
    for (int i{ numOfKnight - 1 }; i >= 0; --i)
    {
        if (army[i]->getType() == DRAGON ||
            army[i]->getLv()==10 && (army[i]->getHp() == army[i]->getMaxHp()))
        {
            neededCondition.hadDefeatedOmega = true;
            army[i]->setLv(10);
            army[i]->setGil(999);
            return true;
        }
        else
        {
            army[i]->setHp(0);
            army[i]->handleWhenHpDown();
            if (army[i]->getHp() <= 0) deleteKnightAt(i);
            else return true;
        }
    }
    if (numOfKnight <= 0) return false;
    return true;
}

bool ArmyKnights::fightHades()
{
    if (neededCondition.hadDefeatedHades) return true;
    for (int i{ numOfKnight - 1 }; i >= 0; --i)
    {
        if (army[i]->getLv() == 10 ||
            army[i]->getType() == PALADIN && army[i]->getLv() > 7 )
        {
            neededCondition.hadDefeatedHades = true;
            neededCondition.hasShield = true;
            return true;
        }
        else
        {
            army[i]->setHp(0);
            army[i]->handleWhenHpDown();
            if (army[i]->getHp() <= 0) deleteKnightAt(i);
            else return true;
        }
    }
    if (numOfKnight <= 0) return false;
    return true;
}

bool ArmyKnights::fightUltimecia()
{
    if (neededCondition.hasSword) return true;
    if (!hasPaladinShield() || !hasLancelotSpear() || !hasGuinevereHair()) return false;
    int iniHp{ 5000 };
    for (int i{ numOfKnight-1 }; i >= 0; --i)
    {
        if (army[i]->getType() != NORMAL)
        {
            int damage = (int)(army[i]->getLv() * army[i]->getHp() * army[i]->baseDameMul);
            iniHp -= damage;
            if (iniHp <= 0) return true;
            deleteKnightAt(i);
        }
        else deleteKnightAt(i);
    }
    
    return false;
}

void ArmyKnights::takePhoenix(int type)
{
    BaseItem* tmp = new Phoenix(type);
    for (int i{ numOfKnight - 1 }; i >= 0; --i)
    {
        if (army[i]->getBag()->insertFirst(tmp)) return;
    }
    delete tmp;
    return;
}

void ArmyKnights::takeMinorItem(int eventNumber)
{
    switch (eventNumber)
    {
    case 95: 
        neededCondition.hasShield = true;
        return;
    case 96:
        neededCondition.hasSpear = true;
        return;
    case 97:
        neededCondition.hasHair = true;
        return;
    }
}

void ArmyKnights::takeExcalibur()
{
    neededCondition.hasSword = neededCondition.hasShield && neededCondition.hasSpear && neededCondition.hasHair;
}

bool ArmyKnights::eventHanlde(int eventOrder, int eventNumber)
{
    if (eventNumber <= 5)
    {
        return fightMinorMonster(eventOrder, eventNumber);
    }
    else if (eventNumber == 6) return fightTornbery(eventOrder);
    else if (eventNumber == 7) return fightQueenOfCards(eventOrder);
    else if (eventNumber == 8)
    {
        meetNina();
        return true;
    }
    else if (eventNumber == 9)
    {
        meetDurianGarden();
        return true;
    }
    else if (eventNumber == 10) return fightOmegaWeapon();
    else if (eventNumber == 11) return fightHades();
    else if (eventNumber <= 97)
    {
        takeMinorItem(eventNumber);
        return true;
    }
    else if (eventNumber == 98)
    {
        takeExcalibur();
        return true;
    }
    else if (eventNumber == 99) return fightUltimecia();
    else
    {
        takePhoenix(eventNumber - 110);
        return true;
    }
    return true;
}






/* * * END implementation of class ArmyKnights * * */


/* * * BEGIN implementation of class Events * * */

Events::Events(const string& file_events) {
    ifstream inf{ file_events };
    numOfEvent = 0;
    event = nullptr;
    if (!inf)
    {
        // Print an error and exit
        std::cerr << "File_events could not be opened for reading!\n";
        return;
    }

    inf >> numOfEvent;
    event = new int[numOfEvent];
    for (int i{ 0 }; i < numOfEvent; ++i)
    {
        inf >> event[i];
    }
    inf.close();
}

Events::~Events() {
    delete[] this->event;
    this->event = nullptr;
    numOfEvent = 0;
}

int Events::count() const {
    return numOfEvent;
}

int Events::get(int i) const {
    return this->event[i];
}

/* * * END implementation of class Events * * */



/* * * BEGIN implementation of class KnightAdventure * * */
KnightAdventure::KnightAdventure() {
    armyKnights = nullptr;
    events = nullptr;
}

KnightAdventure::~KnightAdventure() {
    if (armyKnights) delete armyKnights;
    if (events) delete events;
}

void KnightAdventure::loadArmyKnights(const string& inf)
{
    armyKnights = new ArmyKnights(inf);
}

void KnightAdventure::loadEvents(const string& inf)
{
    events = new Events(inf);
}

void KnightAdventure::run()
{
    if (armyKnights && events)
    {
        bool res = armyKnights->adventure(events);
        armyKnights->printResult(res);
        ///
        //compareOutput(0, 0, myOutput);
        ///
    }
}

/* * * END implementation of class KnightAdventure * * */










