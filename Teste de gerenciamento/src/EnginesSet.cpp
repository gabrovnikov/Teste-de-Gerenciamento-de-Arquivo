#include "EnginesSet.hpp"

EnginesSet::EnginesSet()
{
  mGlobalTargets = 30;
  //   mExitLoop = 0;
  //   mButtonUpState = digitalRead(buttonUp);
  //   mButtonSelectState = digitalRead(buttonSelect);
  //   mButtonDownState = digitalRead(buttonDown);
}

void EnginesSet::insertMotor(char pGuitarString, int pStep, int pDir)
{
  Engine *engineConstrutor = new Engine(pGuitarString, pStep, pDir);

  this->mEngines.push_back(engineConstrutor);
}

// void EnginesSet::boot(); // Lógica de fim de curso

void EnginesSet::addToenginesToPlay(std::string pGuitarStrings)
{
  for (auto &guitarString : pGuitarStrings)
  {
    for (auto engine : this->mEngines)
    {
      if (engine->getGuitarString() == guitarString)
      {
        // if (mButtonUpState == 0 || mButtonSelectState == 0 || mButtonDownState == 0)
        // {
        //   mExitLoop = 1;
        // }
        this->mEnginesToPlay.push_back(engine);
      }
    }
  }
}

void EnginesSet::playMany()
{

  for (int i = 0; i < mEnginesToPlay.size(); i++) {
    if (mEnginesToPlay[i]->getTarget() > 0) {
      digitalWrite(mEnginesToPlay[i]->getDir(), HIGH);
    }
    if (mEnginesToPlay[i]->getTarget() < 0) {
      digitalWrite(mEnginesToPlay[i]->getDir(), LOW);
    }
  }
  for (int j = 0; j < mGlobalTargets; j++) {

    for (int i = 0; i < mEnginesToPlay.size(); i++) {
      digitalWrite(mEnginesToPlay[i]->getStep(), LOW);
    }
    delayMicroseconds(1000);

    for (int i = 0; i < mEnginesToPlay.size(); i++) {
      digitalWrite(mEnginesToPlay[i]->getStep(), HIGH);

      if (mEnginesToPlay[i]->getTarget() > 0) {
        mEnginesToPlay[i]->mPosition++;
      }
      else if (mEnginesToPlay[i]->getTarget() < 0) {
        mEnginesToPlay[i]->mPosition--;
      }

    }
    delayMicroseconds(1000);
  }

  for (int i = 0; i < mEnginesToPlay.size(); i++)
  {
    mEnginesToPlay[i]->reverseTarget();
  }

  this->mEnginesToPlay.clear();
}

void EnginesSet::parseFile(std::string pStream, int pTune)
{
  char guitarString;
  std::string subStream = "";
  int forControl = 0;
  int strControl = 0;
  for (forControl; forControl < pStream.size(); forControl++)
  {
    guitarString = pStream[forControl];
    if (guitarString == ' ')
    {
      if (pStream[forControl - 1] == 'd')
      {
        runThrough(DOWN);
      }
      else if (pStream[forControl - 1] == 's')
      {
        runThrough(UP);
      }
      else if (pStream[forControl - 1] != ' ')
      {
        for (strControl; strControl < forControl; strControl++)
        {
          subStream = subStream + pStream[strControl];
        }
        this->addToenginesToPlay(subStream);
        Serial.println("Cordas do pulso:");
        Serial.println(subStream.c_str());
        this->playMany();
        subStream = "";
      }
      if (pTune == 0) {
        delay(setDelay());
      }
      else {
        delay(100);
      }
      strControl = forControl + 1;
    }
  }
}

void EnginesSet::runThrough(mDirection pDirection)
{
  Serial.println("Entrou em runThrough");
  if (pDirection == UP)
  {
    Serial.println("Entrou na Subida");
    for (auto it = this->mEngines.begin(); it != this->mEngines.end(); ++it)
    {
      this->mEnginesToPlay.push_back(*it);
      delay(100);
      this->playMany();
    }
  }
  else
  {
    Serial.println("Entrou na Descida");
    for (auto it = this->mEngines.rbegin(); it != this->mEngines.rend(); ++it)
    {
      this->mEnginesToPlay.push_back(*it);
      delay(100);
      this->playMany();
    }
  }
}

void EnginesSet::tune(int pTunePosition)
{
  switch (pTunePosition)
  {
    case 0:
      {
        parseFile("E ", 1);
        break;
      }
    case 1:
      {
        parseFile("A ", 1);
        break;
      }
    case 2:
      {
        parseFile("D ", 1);
        break;
      }
    case 3:
      {
        parseFile("B ", 1);
        break;
      }
    case 4:
      {
        parseFile("G ", 1);
        break;
      }
    case 5:
      {
        parseFile("e ", 1);
        break;
      }
  }
}

void EnginesSet::playOneStep(int pGuitarStringReset, int pSignal)
{
  this->mEngines[pGuitarStringReset]->oneStep(pSignal);

  //se subir = target -30
  if (pSignal > 0) {
    mEngines[pGuitarStringReset]->mTarget = -30;
  }
  //se descer = target +30
  else {
    mEngines[pGuitarStringReset]->mTarget = 30;
  }
}

void EnginesSet::runHalfTarget(int pGuitarString)
{
  if (mEngines[pGuitarString]->getTarget() > 0) {
    digitalWrite(mEngines[pGuitarString]->getDir(), HIGH);
  }
  if (mEngines[pGuitarString]->getTarget() < 0) {
    digitalWrite(mEngines[pGuitarString]->getDir(), LOW);
  }

  for (int i = 0; i < (mGlobalTargets/2); i++) {
    
    digitalWrite(mEngines[pGuitarString]->getStep(), LOW);
    delayMicroseconds(1000);
    digitalWrite(mEngines[pGuitarString]->getStep(), HIGH);

    if (mEngines[pGuitarString]->getTarget() > 0) {
      mEngines[pGuitarString]->mPosition++;
    }
    else if (mEngines[pGuitarString]->getTarget() < 0) {
      mEngines[pGuitarString]->mPosition--;
    }
    delayMicroseconds(1000);
  }

  mEngines[pGuitarString]->reverseTarget();
}

void EnginesSet::getEnginePos(SDCard pSd)
{
  int* pos;
  for (int i = 0; i < mEngines.size(); i++)
  {
    pos = mEngines[i]->Engine::getPosPtr();
    mEnginePos.push_back(*pos);
  }
  pSd.writeInFile(mEnginePos);
  pSd.printPosition("/final_position.txt");
  mEnginePos.clear();
}

void EnginesSet::setEngineSpeed(SDCard pSd)
{
  mBPMSpeed = pSd.SD_SPEED;
}

void EnginesSet::setSubdivision(SDCard pSd)
{
  mSubdivision = pSd.SD_SUBDIVISION;
  Serial.println("Número de subdivisões: ");
  Serial.println(mSubdivision);
}

int EnginesSet::setDelay()
{
  int pDelay;
  pDelay = (60 / mBPMSpeed) * (1 / mSubdivision) * 1000;
  Serial.println("BPM: ");
  Serial.println(mBPMSpeed);
  Serial.println("Sub: ");
  Serial.println(mSubdivision);
  Serial.println("Delay: ");
  Serial.println(pDelay);
  return pDelay;
}

void EnginesSet::setCurrentTarget(String pcurrentTargets)
{
  char inputChars[pcurrentTargets.length() + 1];
  pcurrentTargets.toCharArray(inputChars, sizeof(inputChars));
  char* subString = strtok(inputChars, "\n");
  int i = 0;
  while (subString != NULL)
  {
    int position = stoi(subString);
    mEngines[i]->mPosition = position;
    mEngines[i]->mTarget = -position;
    // Serial.println("Posição anterior:");
    // Serial.println(subString);
    // Serial.println("Novo target:");
    // Serial.println(mEngines[i]->mTarget);
    // Serial.println(" ");
    if (mEngines[i]->mPosition == 0)
    {
      mEngines[i]->mTarget = 15;
    }
    i++;
    subString = strtok(NULL, "\n");
  }
}