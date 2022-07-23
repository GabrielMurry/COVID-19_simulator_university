#include <iostream>
#include <math.h>
#include <random>
using namespace std;

void initializing_people(int totalVax, int totalUnvax);
void contact();
int check_test_results();
int getting_tested();
int quarantining();
void print_results(int qCount, int rNum, int healed, int days, int deathCount);
int chance_of_death();
void reset_data();

int population;
int SIZE = 9015;
int vaxedCovidCountDay = 0;
int unvaxedCovidCountDay = 0;
int dailyNewCovidCount = 0;

class Person
{
public:
  bool is_vaxed;
  bool has_covid;
  bool waiting_for_test_results;
  bool is_quaranting;
  int quarantine_day_count;
  bool is_dead;
  Person(bool vaxed, bool covid)
  { // parameterized constructor
    is_vaxed = vaxed;
    has_covid = covid;
    waiting_for_test_results = false;
    is_quaranting = false;
    quarantine_day_count = 0;
  }
  void catching_covid(Person *otherPerson)
  {
    if (has_covid && !otherPerson->has_covid)
    {
      if (otherPerson->is_vaxed && (((rand() % 100) + 1) <= 2))
      { // 2% chance catching covid
        otherPerson->has_covid = true;
        vaxedCovidCountDay++;
        dailyNewCovidCount++;
      }
      else if (!otherPerson->is_vaxed && (((rand() % 100) + 1) <= 60))
      { // 60% chance catching covid
        otherPerson->has_covid = true;
        unvaxedCovidCountDay++;
        dailyNewCovidCount++;
      }
    }
    else if (!has_covid && otherPerson->has_covid)
    {
      if (is_vaxed && (((rand() % 100) + 1) <= 2))
      { // 2% chance catching covid
        has_covid = true;
        vaxedCovidCountDay++;
        dailyNewCovidCount++;
      }
      else if (!is_vaxed && (((rand() % 100) + 1) <= 60))
      { // 60% chance catching covid
        has_covid = true;
        unvaxedCovidCountDay++;
        dailyNewCovidCount++;
      }
    }
  }
  void takes_a_test()
  {
    waiting_for_test_results = true;
  }

  void know_results()
  {
    int rNum = rand() % 100 + 1;
    if (rNum <= 85 && has_covid)
    {                       // test is 85% effective (nytimes)
      is_quaranting = true; // immediate quarantine; for 14 days (i will simulate 7 for now)
    }
    waiting_for_test_results = false; // whether you have covid or not, test came back negative
  }

  void quarantine_check()
  {
    if (quarantine_day_count == 7)
    { // usually 14 days but am doing 7 for now
      has_covid = false;
      is_quaranting = false;
      quarantine_day_count = 0;
    }
    else
    {
      quarantine_day_count++;
    }
  }
};

Person **personArray = new Person *[SIZE];

int main()
{
  srand(time(NULL));
  population = SIZE;
  int totalVax = rint(population * .79);   // round to integer
  int totalUnvax = rint(population * .21); // round to integer
  initializing_people(totalVax, totalUnvax);

  int days;
  for (days = 0; days < 30; days++)
  { // 5 days to start
    contact();

    int qCount = check_test_results();
    int rNum = getting_tested();
    int healed = quarantining();

    int deathCount = chance_of_death();

    // PRINT RESULTS OF EACH DAY HERE... add total new infections for each day
    print_results(qCount, rNum, healed, days, deathCount);
    reset_data();
  }
}

int check_test_results()
{
  // put here when someone knows their results of the test... test sensitivity of 85% (nytimes)
  // wait one day to know results
  int qCount = 0;
  for (int i = 0; i < SIZE; i++)
  {
    if (!personArray[i]->is_dead && personArray[i]->waiting_for_test_results)
    {
      personArray[i]->know_results();
      if (personArray[i]->is_quaranting)
      {
        qCount++;
      }
    }
  }
  return qCount;
}

int getting_tested()
{
  // random num of people out of 1000 getting tested each day give or take
  int rNum = rand() % 1000;
  for (int i = 0; i < rNum; i++)
  {                        // lets say 500 people get tested
    int m = rand() % SIZE; // choose a random person for each loop
    if (!personArray[m]->is_dead && !personArray[m]->waiting_for_test_results && !personArray[m]->is_quaranting)
    { // dont need waiting_for_test_results
      personArray[m]->takes_a_test();
    }
    else
    {
      i--;
    }
  }
  return rNum;
}

int quarantining()
{
  int covidHealedCount = 0;
  for (int i = 0; i < SIZE; i++)
  {
    if (!personArray[i]->is_dead && personArray[i]->is_quaranting)
    {
      personArray[i]->quarantine_check();
      if (!personArray[i]->has_covid)
      { // if person recovers from covid after quarantining
        covidHealedCount++;
      }
    }
  }
  return covidHealedCount;
}

void contact()
{ // how many people does each person come into contact per day (random) (50?)
  for (int i = 0; i < SIZE; i++)
  {
    int n = rand() % 40; // 0 to 40 (random # of people subject comes into contact per day)
    if (!personArray[i]->is_dead && !personArray[i]->is_quaranting)
    {
      for (int j = 0; j < n; j++)
      {                        // ex: 30 loops, pick 30 random people from personArray
        int m = rand() % SIZE; // random person from personArray
        if (!personArray[m]->is_dead && !personArray[m]->is_quaranting && (i != m))
        { // not dead, not quarantining, and is different person
          personArray[i]->catching_covid(personArray[m]);
        }
        else
        { // if dead or is same person, then restart the same loop
          j--;
        }
      }
    }
  }
}

void initializing_people(int totalVax, int totalUnvax)
{ // declaring all people objects and if vaxxed or unvaxxed starts with covid
  int v = 0;
  int uv = 0;
  if ((rand() % 100 + 1) <= 5)
  { // 5% chance that a vaccinated person gets covid
    personArray[0] = new Person(true, true);
    vaxedCovidCountDay++;
    dailyNewCovidCount++;
    v++;
  }
  else
  { // 95% chance that a unvaccinated person gets covid
    personArray[0] = new Person(false, true);
    unvaxedCovidCountDay++;
    dailyNewCovidCount++;
    uv++;
  }
  int i;
  for (i = 1; i <= totalVax; i++)
  {
    personArray[i] = new Person(true, false);
  }
  for (int j = i; j < population; j++)
  {
    personArray[j] = new Person(false, false);
  }
  unsigned seed = rand();
  shuffle(personArray, personArray + population, default_random_engine(seed));
}

void print_results(int qCount, int rNum, int healed, int days, int deathCount)
{
  int countCovid = 0;
  int countVaccinated = 0;
  int countUnvaccinated = 0;
  for (int i = 0; i < SIZE; i++)
  {
    if (personArray[i]->has_covid && personArray[i]->is_vaxed)
    {
      countVaccinated++;
      countCovid++;
    }
    if (personArray[i]->has_covid && !personArray[i]->is_vaxed)
    {
      countUnvaccinated++;
      countCovid++;
    }
  }
  cout << endl
       << "Day " << days + 1 << endl;
  cout << "total covid cases: " << countCovid << endl;
  cout << "total vaccinated people with covid: " << countVaccinated << endl;
  cout << "total unvaccinated people with covid: " << countUnvaccinated << endl;
  cout << "COVID cases increased today by " << dailyNewCovidCount << endl;
  // IMPORTANT: account for deaths, may be less idk
  cout << "Vaccinated COVID cases increased today by " << vaxedCovidCountDay << endl;
  cout << "Unvaccinated COVID cases increased today by " << unvaxedCovidCountDay << endl;
  cout << qCount << " people are quarantining" << endl;
  cout << rNum << " random people are getting tested" << endl;
  cout << healed << " people are healed from COVID" << endl;
  cout << deathCount << " people died today" << endl
       << endl;
}

int chance_of_death()
{ // death rate for vaxxed versus anti vax; over 2,000 deaths per day in US
  int deathCount = 0;
  for (int i = 0; i < 5; i++)
  { // ex: up to 5 people have a 1% chance of dying per day
    int n = rand() % SIZE;
    if (!personArray[n]->is_dead && personArray[n]->has_covid)
    {
      int r = rand() % 100 + 1;
      if (r == 1)
      {
        personArray[n]->is_dead = true;
        deathCount++;
      }
    }
  }
  return deathCount; // show who exactly died- vaxed or not vaxed etc.
}

void reset_data()
{
  vaxedCovidCountDay = 0;
  unvaxedCovidCountDay = 0;
  dailyNewCovidCount = 0;
}

// total enrollment: 9015 -- 7120 vaccinated and 1895 not vaccinated
// a simulation of santa clara COVID outbreak
// one student tests positive for COVID... how long until the entire school is positive?
// random chance for someone getting covid when positive person randomly comes into contact with another
// random and eventual chance of positive person quarantining (not coming into contact with others)
// that person will eventually not have covid
// 79% of students are vaccinated - random chance of running into vaxxed or unvaxxed
// chances of unvaxxed catching covid is higher than unvaxxed
// chance of severity of covid being higher for some people (find percentage online to input here)
// Counting days, each day shows statistics: how many total infected, how many quarantined, how many got better, how many new infected people that day
// put statistics in text file then graph the trends etc.
// NOT TESTED ON WEEKENDS
// MAKE A VISUAL REPESENTATION FOR EACH DAY (VISUAL- show people and infected people and people quaratnining)
