#include <iostream>
#include <random>
#include <vector>
#include <algorithm> 

using namespace std;

double truncated_normal(double mean, double stddev, double lower, double upper) {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> distribution(mean, stddev);

    double value;
    do {
        value = distribution(gen);
    } while (value < lower || value > upper);

    return value;
}

class Politician {
public:
    int age;
    int yearsInOffice;
    int office; // 0 - Quaestor, 1 - Aedile, 2 - Praetor, 3 - Consul
    int lastConsulYear; 

    Politician(int initialAge, int initialOffice) : age(initialAge), yearsInOffice(0), office(initialOffice), lastConsulYear(-1) {}

    bool isEligible(int minAge, int minYears) {
        return age >= minAge && yearsInOffice >= minYears;
    }
};

int main() {
    const int SIMULATION_YEARS = 200;

 
    const double LIFE_EXPECTANCY_MEAN = 55.0;
    const double LIFE_EXPECTANCY_STDDEV = 10.0;
    const int MIN_LIFE_EXPECTANCY = 25;
    const int MAX_LIFE_EXPECTANCY = 80;


    const int QUAESTOR_MIN_AGE = 30;
    const int AEDILE_MIN_AGE = 36;
    const int AEDILE_MIN_YEARS = 2;
    const int PRAETOR_MIN_AGE = 39;
    const int PRAETOR_MIN_YEARS = 2;
    const int CONSUL_MIN_AGE = 42;
    const int CONSUL_MIN_YEARS = 2;
    const int CONSUL_REELECTION_INTERVAL = 10;

    const int NUM_QUAESTORS = 20;
    const int NUM_AEDILES = 10;
    const int NUM_PRAETORS = 8;
    const int NUM_CONSULS = 2;

    int psi = 100;
    const int PENALTY_PER_UNFILLED_POSITION = -5;
    const int PENALTY_FOR_CONSUL_REELECTION = -10;


    const double NEW_CANDIDATES_MEAN = 15.0;
    const double NEW_CANDIDATES_STDDEV = 5.0;

 
    vector<Politician> politicians;
    for (int i = 0; i < NUM_QUAESTORS; i++) {
        politicians.push_back(Politician(QUAESTOR_MIN_AGE, 0));
    }
    for (int i = 0; i < NUM_AEDILES; i++) {
        politicians.push_back(Politician(AEDILE_MIN_AGE, 1));
    }
    for (int i = 0; i < NUM_PRAETORS; i++) {
        politicians.push_back(Politician(PRAETOR_MIN_AGE, 2));
    }
    for (int i = 0; i < NUM_CONSULS; i++) {
        politicians.push_back(Politician(CONSUL_MIN_AGE, 3));
    }


    random_device rd;
    mt19937 gen(rd());

    for (int year = 0; year < SIMULATION_YEARS; year++) {
 
        int newCandidates = round(truncated_normal(NEW_CANDIDATES_MEAN, NEW_CANDIDATES_STDDEV, 0.0, 100.0));
        for (int i = 0; i < newCandidates; i++) {
            politicians.push_back(Politician(QUAESTOR_MIN_AGE - 1, 0)); // Adjust age for new candidates entering next year
        }


        vector<int> filledQuaestors(NUM_QUAESTORS, -1);
        vector<int> filledAediles(NUM_AEDILES, -1);
        vector<int> filledPraetors(NUM_PRAETORS, -1);
        vector<int> filledConsuls(NUM_CONSULS, -1);

        // Fill offices from highest to lowest rank
        for (int i = politicians.size() - 1; i >= 0; i--) {
            Politician& politician = politicians[i];

            // Consul
            if (politician.office == 3 && politician.yearsInOffice >= CONSUL_MIN_YEARS && year - politician.lastConsulYear >= CONSUL_REELECTION_INTERVAL) {
                if (find(filledConsuls.begin(), filledConsuls.end(), i) == filledConsuls.end()) {
                    filledConsuls[gen() % NUM_CONSULS] = i; // Using mt19937 for randomization
                    politician.yearsInOffice = 0;
                    politician.lastConsulYear = year;
                }
            }

            // Praetor
            else if (politician.office == 2 && politician.yearsInOffice >= PRAETOR_MIN_YEARS) {
                if (find(filledPraetors.begin(), filledPraetors.end(), i) == filledPraetors.end()) {
                    filledPraetors[gen() % NUM_PRAETORS] = i;
                    politician.office = 3;
                    politician.yearsInOffice = 0;
                }
            }

            // Aedile
            else if (politician.office == 1 && politician.yearsInOffice >= AEDILE_MIN_YEARS) {
                if (find(filledAediles.begin(), filledAediles.end(), i) == filledAediles.end()) {
                    filledAediles[gen() % NUM_AEDILES] = i;
                    politician.office = 2;
                    politician.yearsInOffice = 0;
                }
            }

            // Quaestor
            else if (politician.office == 0) {
                if (find(filledQuaestors.begin(), filledQuaestors.end(), i) == filledQuaestors.end()) {
                    filledQuaestors[gen() % NUM_QUAESTORS] = i;
                    politician.office = 1;
                    politician.yearsInOffice = 0;
                }
            }
        }

        // Update PSI
        int unfilledPositions = 0;
        for (int i : filledQuaestors) {
            if (i == -1) {
                unfilledPositions++;
            }
        }
        for (int i : filledAediles) {
            if (i == -1) {
                unfilledPositions++;
            }
        }
        for (int i : filledPraetors) {
            if (i == -1) {
                unfilledPositions++;
            }
        }
        for (int i : filledConsuls) {
            if (i == -1) {
                unfilledPositions++;
            }
        }
        psi -= PENALTY_PER_UNFILLED_POSITION * unfilledPositions;

        
        for (Politician& politician : politicians) {
            if (politician.office == 3 && politician.yearsInOffice == 0 && year - politician.lastConsulYear < CONSUL_REELECTION_INTERVAL) {
                psi -= PENALTY_FOR_CONSUL_REELECTION;
            }
        }

        for (int i = politicians.size() - 1; i >= 0; i--) {
            Politician& politician = politicians[i];
            politician.age++;
            politician.yearsInOffice++;

            if (politician.age > truncated_normal(LIFE_EXPECTANCY_MEAN, LIFE_EXPECTANCY_STDDEV, MIN_LIFE_EXPECTANCY, MAX_LIFE_EXPECTANCY)) {
                politicians.erase(politicians.begin() + i);
            }
        }
    }

  
    int totalQuaestorsFilled = 0;
    int totalAedilesFilled = 0;
    int totalPraetorsFilled = 0;
    int totalConsulsFilled = 0;

    for (Politician& politician : politicians) {
        if (politician.office == 0) {
            totalQuaestorsFilled++;
        }
        else if (politician.office == 1) {
            totalAedilesFilled++;
        }
        else if (politician.office == 2) {
            totalPraetorsFilled++;
        }
        else if (politician.office == 3) {
            totalConsulsFilled++;
        }
    }

    cout << "End-of-simulation PSI: " << psi << endl;
    cout << "Annual fill rate:" << endl;
    cout << "Quaestor: " << (double)totalQuaestorsFilled / (SIMULATION_YEARS * NUM_QUAESTORS) * 100 << "%" << endl;
    cout << "Aedile: " << (double)totalAedilesFilled / (SIMULATION_YEARS * NUM_AEDILES) * 100 << "%" << endl;
    cout << "Praetor: " << (double)totalPraetorsFilled / (SIMULATION_YEARS * NUM_PRAETORS) * 100 << "%" << endl;
    cout << "Consul: " << (double)totalConsulsFilled / (SIMULATION_YEARS * NUM_CONSULS) * 100 << "%" << endl;

    cout << "Age distribution:" << endl;
    cout << "Quaestor: ";
    for (Politician& politician : politicians) {
        if (politician.office == 0) {
            cout << politician.age << " ";
        }
    }
    cout << endl;
    cout << "Aedile: ";
    for (Politician& politician : politicians) {
        if (politician.office == 1) {
            cout << politician.age << " ";
        }
    }
    cout << endl;
    cout << "Praetor: ";
    for (Politician& politician : politicians) {
        if (politician.office == 2) {
            cout << politician.age << " ";
        }
    }
    cout << endl;
    cout << "Consul: ";
    for (Politician& politician : politicians) {
        if (politician.office == 3) {
            cout << politician.age << " ";
        }
    }
    cout << endl;

    return 0;
}
